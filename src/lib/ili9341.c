/* ili9341.c */

#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "ili9341.h"

static struct ili9341_config config;
static uint32_t pin_color_mask;
static uint32_t *framebuffers[2];
static volatile int screen_framebuffer_index;
struct SCREEN screen;

#define ILI9341_CMD_NOP                       0x00
#define ILI9341_CMD_RESET                     0x01
#define ILI9341_CMD_READ_DISPLAY_ID           0x04
#define ILI9341_CMD_READ_DISPLAY_STATUS       0x04
#define ILI9341_CMD_READ_PIXEL_FORMAT         0x0c
#define ILI9341_CMD_READ_SELF_DIAGNOSTIC      0x0f
#define ILI9341_CMD_SLEEP_OUT                 0x11
#define ILI9341_CMD_NORMAL_MODE               0x13
#define ILI9341_CMD_DISPLAY_INVERT_OFF        0x20
#define ILI9341_CMD_DISPLAY_INVERT_ON         0x21
#define ILI9341_CMD_DISPLAY_OFF               0x28
#define ILI9341_CMD_DISPLAY_ON                0x29
#define ILI9341_CMD_WRITE_MEM                 0x2c
#define ILI9341_CMD_SET_COLOR                 0x2d
#define ILI9341_CMD_SET_COLUMN_ADDRESS        0x2a
#define ILI9341_CMD_MEMORY_ACCESS_CONTROL     0x36
#define ILI9341_CMD_IDLE_MODE_OFF             0x38
#define ILI9341_CMD_IDLE_MODE_ON              0x39
#define ILI9341_CMD_SET_PIXEL_FORMAT          0x3a
#define ILI9341_CMD_WRITE_DISPLAY_BRIGHTNESS  0x51
#define ILI9341_CMD_DISPLAY_INVERSION_CONTROL 0xb4
#define ILI9341_CMD_FRAME_RATE_CONTROL        0xb3
#define ILI9341_CMD_BLANKING_PORCH_CONTROL    0xb5

#define DELAY_NS() do {} while (0)
//#define DELAY_NS()  do { for (volatile int i = 0; i < 10; i++); } while (0)
//#define DELAY_NS() sleep_ms(1)

static void ili9341_write_cmd(uint8_t bits)
{
  gpio_put(config.pin_rs, 0);
  for (int i = 0; i < 8; i++) {
    gpio_set_dir(config.pin_cmd[i], GPIO_OUT);
    gpio_put(config.pin_cmd[i], (bits>>i) & 1);
  }
  gpio_put(config.pin_wr, 0);
  DELAY_NS();
  gpio_put(config.pin_wr, 1);
  DELAY_NS();
  gpio_put(config.pin_rs, 1);
  DELAY_NS();
}

static void ili9341_prepare_write_color(void)
{
  for (int i = 0; i < 8; i++) {
    gpio_set_dir(config.pin_color_base+i, GPIO_OUT);
  }
}

static void ili9341_write_color(uint8_t bits)
{
  gpio_put_masked(pin_color_mask, bits<<config.pin_color_base);
  gpio_put(config.pin_wr, 0);
  DELAY_NS();
  gpio_put(config.pin_wr, 1);
  DELAY_NS();
}

static void ili9341_write_cmd_arg(uint8_t bits)
{
  for (int i = 0; i < 8; i++) {
    gpio_set_dir(config.pin_cmd[i], GPIO_OUT);
    gpio_put(config.pin_cmd[i], (bits>>i) & 1);
  }
  gpio_put(config.pin_wr, 0);
  DELAY_NS();
  gpio_put(config.pin_wr, 1);
  DELAY_NS();
}

static void init_pin_out(int pin, int value)
{
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_OUT);
  gpio_put(pin, value);
}

static int init_screen(void)
{
  framebuffers[0] = malloc(320*240);
  framebuffers[1] = malloc(320*240);
  if (! framebuffers[0] || ! framebuffers[1]) {
    free(framebuffers[0]);
    free(framebuffers[1]);
    return 1;
  }
  screen_framebuffer_index = 0;

  screen.width = 320;
  screen.height = 240;
  screen.framebuffer = framebuffers[screen_framebuffer_index];
  
  memset(framebuffers[0], 0, screen.width*screen.height);
  memset(framebuffers[1], 0, screen.width*screen.height);

  return 0;
}

int ili9341_init(struct ili9341_config *init_config)
{
  if (init_screen() != 0) {
    return 1;
  }

  config = *init_config;
  pin_color_mask = 0xff << config.pin_color_base;
  
  // setup pins
  init_pin_out(config.pin_cs, 1);
  init_pin_out(config.pin_rst, 1);
  init_pin_out(config.pin_wr, 1);
  init_pin_out(config.pin_rd, 1);
  init_pin_out(config.pin_rs, 0);
  for (int i = 0; i < config.num_pins; i++) {
    gpio_init(config.pin_base+i);
  }

  // enable ILI9341 chip
  gpio_put(config.pin_cs, 0);

  // hard reset
  sleep_ms(5);
  gpio_put(config.pin_rst, 0);
  sleep_ms(5);
  gpio_put(config.pin_rst, 1);
  sleep_ms(500);

  // soft reset
  ili9341_write_cmd(ILI9341_CMD_RESET);
  sleep_ms(200);

  // wake up
  ili9341_write_cmd(ILI9341_CMD_SLEEP_OUT);
  sleep_ms(10);

  // set normal display mode
  ili9341_write_cmd(ILI9341_CMD_NORMAL_MODE);
  ili9341_write_cmd(ILI9341_CMD_DISPLAY_ON);
  ili9341_write_cmd(ILI9341_CMD_DISPLAY_INVERT_OFF);
  ili9341_write_cmd(ILI9341_CMD_IDLE_MODE_OFF);

  // set pixel format to 16 bits
  ili9341_write_cmd(ILI9341_CMD_SET_PIXEL_FORMAT);
  ili9341_write_cmd_arg(0x05);
  sleep_ms(10);
  
  // rotate 90 degrees (240x320 -> 320x240)
  ili9341_write_cmd(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
  ili9341_write_cmd_arg(0x20);
  ili9341_write_cmd(ILI9341_CMD_SET_COLUMN_ADDRESS);
  ili9341_write_cmd_arg(0);                 // col start = 0
  ili9341_write_cmd_arg(0);
  ili9341_write_cmd_arg((319>>8) & 0xff);   // col end = 319
  ili9341_write_cmd_arg(319 & 0xff);

  sleep_ms(200);
  ili9341_write_cmd(ILI9341_CMD_WRITE_MEM);
  ili9341_prepare_write_color();
  return 0;
}

void ili9341_clear_screen(uint8_t color)
{
  for (int i = 0; i < 320*240; i++) {
    ili9341_write_color(color);
  }
}

void ili9341_write_framebuffer(uint32_t *framebuffer)
{
  unsigned char *pixel = (unsigned char *)framebuffer;
  for (int i = 0; i < 320*240; i++) {
    ili9341_write_color(*pixel++);
  }
}

void ili9341_swap_buffers(void)
{
  screen_framebuffer_index = 1 - screen_framebuffer_index;
  screen.framebuffer = framebuffers[screen_framebuffer_index];
  memset(screen.framebuffer, 0xff, 320*240);
}
