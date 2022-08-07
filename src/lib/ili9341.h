/* ili9341.h */

#ifndef ILI9341_H_FILE
#define ILI9341_H_FILE

#include <stdint.h>
#include <stdbool.h>

struct ili9341_config {
  uint8_t pin_rs;          // cmd/data select (0=cmd, 1=data)
  uint8_t pin_rd;          // read enable (active low)
  uint8_t pin_wr;          // write enable (active low)
  uint8_t pin_cs;          // chip select (active low)
  uint8_t pin_rst;         // reset (active low)
  uint8_t pin_base;        // first pin
  uint8_t num_pins;        // number of pins in total
  uint8_t pin_cmd[8];      // 8 command pins mapped to LCD D0-D7 (may overlap with data)
  uint8_t pin_color_base;  // 8 contiguous color pins mapped to LCD D2,D3,D4,D8,D9,D10,D14,D15
};

struct SCREEN {
  int width;
  int height;
  uint32_t *framebuffer;
};

extern struct SCREEN screen;

int ili9341_init(struct ili9341_config *config);
void ili9341_clear_screen(uint8_t color);
void ili9341_write_framebuffer(uint32_t *framebuffer);
void ili9341_swap_buffers(void);

#endif /* ILI9341_H_FILE */
