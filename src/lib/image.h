/* image.h */

#ifndef IMAGE_H_FILE
#define IMAGE_H_FILE

#include <stdint.h>
#include <stdbool.h>

struct IMAGE {
  int width;
  int height;
  unsigned int stride;  // number of words per line
  const uint32_t *data;
};

void image_draw(const struct IMAGE *image, int x, int y, bool transparent);
void image_draw_frame(const struct IMAGE *img, int frame, int x, int y, bool transparent);

#endif /* IMAGE_H_FILE */
