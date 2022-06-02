#include "util.h"

#include <stdlib.h>
#include <time.h>
#include <math.h>

#define BYTE_MASK 0xFF
/* Curses' colors uses values 0-999 for each component of the color,
   but RGB uses only 0-255, so we have to scale it */
#define SCALING 3.91765
#define SCALE(x) ((uint16_t)(ceil((double)(x) * SCALING)))

void rgb_to_curses_color(uint32_t rgb, uint16_t color[NUM_COLORS])
{
    uint16_t red = rgb >> 16;
    uint16_t green = (rgb >> 8) & BYTE_MASK;
    uint16_t blue = rgb & BYTE_MASK;

    color[0] = SCALE(red);
    color[1] = SCALE(green);
    color[2] = SCALE(blue);
}

void init_rand(void)
{
    srand((unsigned int)time(NULL));
}
