#include "input.h"

#include <curses.h>

int current_input;

void read_input(WINDOW *win)
{
    current_input = wgetch(win);

    switch(current_input)
    {
        case 'q':
            // exit game
            break;

        default:
            break;
    }
}