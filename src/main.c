#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/multicore.h"
#include "pico/binary_info.h"

#include "lib/ili9341.h"
#include "screen.h"
#include "core_msg.h"

#define LED_PIN 25

#define LCD_PIN_BASE       2
#define LCD_NUM_PINS       13
#define LCD_PIN_CMD_BASE   2
#define LCD_PIN_COLOR_BASE 7
#define LCD_PIN_RST        15
#define LCD_PIN_CS         16
#define LCD_PIN_RS         17
#define LCD_PIN_WR         18
#define LCD_PIN_RD         19

struct ili9341_config config = {
  .pin_cs = LCD_PIN_CS,
  .pin_wr = LCD_PIN_WR,
  .pin_rd = LCD_PIN_RD,
  .pin_rs = LCD_PIN_RS,
  .pin_rst = LCD_PIN_RST,
  .pin_base = LCD_PIN_BASE,
  .num_pins = LCD_NUM_PINS,
  .pin_color_base = LCD_PIN_COLOR_BASE,
  .pin_cmd = {
    LCD_PIN_CMD_BASE+0,
    LCD_PIN_CMD_BASE+1,
    LCD_PIN_CMD_BASE+5,
    LCD_PIN_CMD_BASE+6,
    LCD_PIN_CMD_BASE+7,
    LCD_PIN_CMD_BASE+2,
    LCD_PIN_CMD_BASE+3,
    LCD_PIN_CMD_BASE+4,
  },
};

static void blink_led(void)
{
  static int led_state = 1;
  static int frame_count = 0;
  if (frame_count++ >= 30) {
    frame_count = 0;
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);
  }
}

static void core1_main(void)
{
  multicore_fifo_push_blocking(0);   // tell core 0 we're ready
  multicore_fifo_pop_blocking();     // wait until core 0 is ready

  while (true) {
    sleep_ms(1);
    union CORE_MSG msg;
    while (core_msg_try_recv(&msg)) {
      switch (msg.msg_header.type) {
      case CORE_MSG_TYPE_SEND_FRAMEBUFFER:
        multicore_fifo_push_blocking(0);
        ili9341_write_framebuffer(msg.send_framebuffer.framebuffer);
        break;
        
      default:
        printf("ERROR processing message: invalid messaged type %d\n", msg.msg_header.type);
        break;
      }
    }
  }
}

int main(void)
{
  stdio_init_all();
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  //sleep_ms(5000);
  //printf("start!\n");

  core_msg_init();
  multicore_launch_core1(core1_main);
  multicore_fifo_pop_blocking();      // wait until core1 is ready
  multicore_fifo_push_blocking(0);    // tell core1 we're ready

  ili9341_init(&config);
  ili9341_clear_screen(0);
  screen_init();

  int last_frame_ms = 0;
  while (1) {
    blink_led();
    screen_render();
    int cur_ms = 0;
    do {
      cur_ms = to_ms_since_boot(get_absolute_time());
    } while (cur_ms - last_frame_ms < 16);
    last_frame_ms = cur_ms;
    msg_send_framebuffer(screen.framebuffer);
    multicore_fifo_pop_blocking();
    ili9341_swap_buffers();
  }
}
