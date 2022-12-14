#ifndef FONT_H_FILE
#define FONT_H_FILE

#define FONT_USE_STDARG 1

#ifdef __cplusplus
extern "C" {
#endif

struct FONT {
  int w;
  int h;
  int first_char;
  int num_chars;
  const unsigned char *data;
};

enum FONT_ALIGNMENT {
  FONT_ALIGN_LEFT,
  FONT_ALIGN_CENTER,
  FONT_ALIGN_RIGHT
};

void font_set_font(const struct FONT *font);
void font_set_color(unsigned int color);
void font_set_border(int enable, unsigned int color);
void font_move(unsigned int x, unsigned int y);
void font_align(enum FONT_ALIGNMENT alignment);

void font_print_int(int num);
void font_print_uint(unsigned int num);
void font_print_float(float num);
void font_print(const char *text);

#if FONT_USE_STDARG
void font_printf(const char *fmt, ...)
  __attribute__ ((format (printf, 1, 2)));
#endif
  
#ifdef __cplusplus
}
#endif
  
#endif /* FONT_H_FILE */
