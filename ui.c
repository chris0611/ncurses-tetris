#include "ui.h"
#include "util.h"

#include <string.h>
#include <curses.h>

#define MS_PER_LOOP 50

static WINDOW *board_win;
static WINDOW *info_win;

#ifdef DEBUG
static WINDOW *dbg_win;
#endif

static void setup_colors(void);
static void draw_tetro(tetromino_t *tetro, pos_t pos);
static void draw_board(pos_t start);
static void draw_info(void);

static pos_t board_pos;
static pos_t info_win_pos;
static uint16_t info_width, info_height;


bool ui_init(void)
{
    if (initscr() == NULL) return false;

    clear();
    noecho();
    cbreak();

    keypad(stdscr, TRUE);   /* Enables us to read single function key presses */

    timeout(MS_PER_LOOP);   /* Timing (getch waits 50ms for input) */

    setup_colors();         /* Initializes colors */

    curs_set(0);            /* Makes cursor invisible */

    refresh();

    pos_t center = {
        .x = COLS / 2,
        .y = LINES / 2
    };

    board_pos.x = center.x - (TOTAL_BOARD_WIDTH / 2);
    board_pos.y = center.y - (TOTAL_BOARD_HEIGHT / 2);

    info_win_pos.x = board_pos.x + TOTAL_BOARD_WIDTH + 2;
    info_win_pos.y = board_pos.y;

    info_width = COLS - info_win_pos.x - 1;
    if (info_width > INFO_COLS) info_width = INFO_COLS;

    info_height = INFO_LINES;

    /* initialize ncurses windows */
    board_win = newwin(TOTAL_BOARD_HEIGHT, TOTAL_BOARD_WIDTH, 
                       board_pos.y, board_pos.x);
    info_win = newwin(info_height, info_width, info_win_pos.y, info_win_pos.x);

#ifdef DEBUG
    dbg_win = newwin(LINES, DBG_WIDTH, DBG_Y, DBG_X);
    if (dbg_win == NULL) return false;
#endif /* DEBUG */

    if (board_win == NULL) return false;
    if (info_win == NULL) return false;

    box(board_win, 0, 0);
    box(info_win, 0, 0);

    return true;
}


void ui_cleanup(void)
{
    delwin(board_win);
    delwin(info_win);

#ifdef DEBUG
    delwin(dbg_win);
#endif

    endwin();
}


void ui_draw(void)
{
    pos_t draw_start = { BORDER_SIZE, BORDER_SIZE };

    draw_board(draw_start);
    draw_info();
}


/* Draw board starting at (start.x, start.y) */
static void draw_board(pos_t start)
{
    wmove(board_win, start.y, start.x);

    uint8_t id = 0;

    for (uint16_t i = 0; i < 10*20; i++)
    {
        if ((id = game_state.board[i]) != 0) {
            uint16_t color = tetrominoes[id-1].color;
            wattron(board_win, COLOR_PAIR(color));
            wprintw(board_win, "  ");
            wattroff(board_win, COLOR_PAIR(color));
        } else {
            wprintw(board_win, "  ");
        }

        if ((i + 1) % 10 == 0) {
            wmove(board_win, (i + 1) / 10 + start.y , 0 + start.x);
        }
    }

    /* Adjust for offset */
    pos_t pos_offset = {
        game_state.curr_pos.x + start.x,
        game_state.curr_pos.y + start.y
    };

    draw_tetro(&game_state.curr_tetro, pos_offset);

    wrefresh(board_win);
}


static void draw_info(void)
{
    mvwprintw(info_win, 1, 2, "Score:");
    mvwprintw(info_win, 1, 9, "%d", game_state.score);
    wrefresh(info_win);
}


static void draw_tetro(tetromino_t *tetro, pos_t pos)
{
    uint16_t color_id = tetro->color;
    uint16_t start_x  = pos.x;

    /* Moves the window cursor to the tetromino's position */
    wmove(board_win, pos.y, pos.x);

    for (uint8_t i = 0; i < TETRO_BUF_SIZE; i++) {
        switch(tetro->piece[i])
        {
            case ' ':
                /* empty space, just move cursor over */
                pos.x += 2;
                wmove(board_win, pos.y, pos.x);
                break;
            default:
                /* actually part of a piece, turn on color,
                   add chars and turn off again */
                wattron(board_win, COLOR_PAIR(color_id));
                wprintw(board_win, "  ");
                wattroff(board_win, COLOR_PAIR(color_id));
        }

        if (((i + 1) % TETRO_SIZE) == 0) {
            pos.x = start_x;
            pos.y++;
            wmove(board_win, pos.y, pos.x);
        }
    }
}


static void setup_colors(void)
{
    start_color();

    /* might change to a nicer orange */
    uint16_t colors[3] = { 0 };
    uint32_t orange_rgb = 0xff7700;

    rgb_to_curses_color(orange_rgb, colors);

    init_color(COLOR_ORANGE, colors[0], colors[1], colors[2]);

    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_BLACK, COLOR_BLUE);
    init_pair(3, COLOR_BLACK, COLOR_ORANGE);
    init_pair(4, COLOR_BLACK, COLOR_YELLOW);
    init_pair(5, COLOR_BLACK, COLOR_GREEN);
    init_pair(6, COLOR_BLACK, COLOR_RED);
    init_pair(7, COLOR_BLACK, COLOR_MAGENTA);
#ifdef DEBUG
    init_pair(DBG_COLOR, COLOR_RED, COLOR_BLACK);
#endif /* DEBUG */
}


#ifdef DEBUG
void _debug_print(const char *dbg_str, ...)
{
    static int32_t line = 0;

    va_list arguments;
    va_start(arguments, dbg_str);

    wmove(dbg_win, line % LINES, 0);
    wclrtoeol(dbg_win);
    wrefresh(dbg_win);
    wmove(dbg_win, line % LINES, 0);

    wattron(dbg_win, COLOR_PAIR(DBG_COLOR));
    vw_printw(dbg_win, dbg_str, arguments);
    wattron(dbg_win, COLOR_PAIR(DBG_COLOR));

    va_end(arguments);
    wrefresh(dbg_win);

    line++;
}
#endif /* DEBUG */
