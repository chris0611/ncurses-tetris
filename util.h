#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>

#define NUM_COLORS 3

/*  Converts a rgb value to the color format used by curses. */
void rgb_to_curses_color(uint32_t rgb, uint16_t color[NUM_COLORS]);

void init_rand(void);

#endif /* UTIL_H */
