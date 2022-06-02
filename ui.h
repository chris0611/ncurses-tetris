#ifndef UI_H
#define UI_H

#include "tetris.h"

#define COLOR_ORANGE 32

enum ui_sizes {
    BLOCK_WIDTH  = 2,
    BLOCK_HEIGHT = 1,
    BORDER_SIZE  = 1,
    INFO_LINES   = 22,
    INFO_COLS    = 20
};


/* Function prototypes */

bool ui_init(void);
void ui_cleanup(void);
void ui_draw(void);

#ifdef DEBUG
#include <stdarg.h>

#define DBG_COLOR 8

#define DEBUG_PRINT(_fmt, ...) do {\
    _debug_print("[%s: %d]: "\
        _fmt, __FILE__, __LINE__, __VA_ARGS__);\
    } while (0)

void _debug_print(const char *dbg_str, ...);

enum dbg_size {
    DBG_HEIGHT = 20,
    DBG_WIDTH = 60,
    DBG_Y = 0,
    DBG_X = 0
};
#endif /* DEBUG */

#endif /* UI_H */
