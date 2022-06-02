#include "tetris.h"
#include "ui.h"

#include <stdlib.h>
#include <curses.h>


int main(void)
{
    int ch;
    game_over = false;

    ui_init();
    tetris_init();

    ui_draw();

    /* --- Game loop --- */
    while (!game_over)
    {
        /* --- User input --- */
        ch  = getch();
        switch (ch)
        {
            case 'a':
                try_move_piece(ROTATE_LEFT);
                break;
            case 'd':
                try_move_piece(ROTATE_RIGHT);
                break;
            case 'r':
                reset();
                break;
            case 'p':
                pause();
                break;
            case 'q':
                game_over = true;
                break;
            case KEY_LEFT:
                try_move_piece(MOVE_LEFT);
                break;
            case KEY_RIGHT:
                try_move_piece(MOVE_RIGHT);
                break;
            case KEY_DOWN:
                try_move_piece(MOVE_DOWN);
                break;
            default:
                if (ch != -1)
                    mvwprintw(stdscr, LINES - 2, 2, "Key pressed: %d", ch);
        }

        /* --- Game logic --- */
        tetris_update();

        /* --- "Render" --- */
        ui_draw();
    }

    tetris_end();
    ui_cleanup();

    return EXIT_SUCCESS;
}
