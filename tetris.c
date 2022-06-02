#include "tetris.h"
#include "util.h"
#include "ui.h"

#include <curses.h>
#include <stdlib.h>
#include <string.h>

/* Function prototypes */

static void fill_bag(void);
static uint8_t get_next(void);

static bool check_line(uint8_t line);
static void move_lines(bool lines[BOARD_HEIGHT], uint8_t amt);
static bool piece_fits(tetromino_t *tetro, pos_t pos);
static void piece_to_board(void);

static uint16_t rotated_index(pos_t pos, shape_t shape);
static void rotate(move_t move);
static void move_horizontal(move_t direction);
static bool move_down(void);


/* Each tetromino pattern/shape with its color */
const tetromino_t tetrominoes[AMT_TETROS] = {
    { TETRO_I, COL_CYAN,    /* I-tetromino */
      "    "
      "####"
      "    "
      "    " },
    { TETRO_J, COL_BLUE,    /* J-tetromino */
      "#   "
      "### "
      "    "
      "    " },
    { TETRO_L, COL_ORANGE,  /* L-tetromino */
      "  # "
      "### "
      "    "
      "    " },
    { TETRO_O, COL_YELLOW,  /* O-tetromino */
      "    "
      " ## "
      " ## "
      "    " },
    { TETRO_S, COL_GREEN,   /* S-tetromino */
      " ## "
      "##  "
      "    "
      "    " },
    { TETRO_Z, COL_RED,     /* Z-tetromino */
      "##  "
      " ## "
      "    "
      "    " },
    { TETRO_T, COL_MAGENTA, /* T-tetromino */
      " #  "
      "### "
      "    "
      "    " }
};


/* Globals :^) */
bool game_over;
state_t game_state;
static bag_t random_bag;
static int counter = 0;
/* TODO: Change difficulty dynamically (based on score or #of pieces?) */
static int difficulty = 20;


/* Initializes board and random piece "generator" */
void tetris_init(void)
{
    /* TODO: Welcome screen? */

    game_state.curr_pos.x = (BOARD_WIDTH/2)-4;
    game_state.curr_pos.y = 0;

    init_rand();
    fill_bag();

    game_state.current = get_next();

    game_state.curr_tetro = tetrominoes[get_next()];

    game_state.next    = get_next();

    memset(game_state.board, 0, sizeof(uint8_t) * 10 * 20);
}


void tetris_end(void)
{
    /* TODO: Make function */
    /* print users score, highscores */
}


void tetris_update(void)
{
    static const uint32_t scores[4] = { 100, 300, 500, 800 };

    /* Since delay is 50ms, and we want to draw every second,
       we need to wait 50ms * 20 */
    if (++counter == difficulty) {
        if (!move_down()) {
            /* place piece on board */
            piece_to_board();


            reset();
        }
        counter = 0;
    }


    bool cleared_lines[BOARD_HEIGHT] = { false };
    uint8_t num_cleared = 0;

    /* CHECK IF SCORE NEEDS TO BE UPDATED */
    for (uint8_t i = 0; i < BOARD_HEIGHT; i++) {
        if (check_line(i)) {
            /* Update board state*/
            cleared_lines[i] = true;
            num_cleared++;
        }
    }

    if (num_cleared > 0)
    {
        /* Update score */
        game_state.score += scores[num_cleared-1];
        /* Animation first? */
        /* Update board */
        move_lines(cleared_lines, num_cleared);
    }
}


static bool check_line(uint8_t line)
{
    for (uint16_t i = 0; i < 10; i++) {
        if (!game_state.board[line*10 + i])
            return false;
    }

#ifdef DEBUG
    DEBUG_PRINT("Line %d cleared", line);
#endif /* DEBUG */
    return true;
}


static void move_lines(bool lines[BOARD_HEIGHT], uint8_t amt)
{
    uint16_t line = BOARD_HEIGHT - 1;
    bool at_cleared = false;

    do {
        if (lines[line]) {
            at_cleared = true;
#ifdef DEBUG
            DEBUG_PRINT("Line: %d", line);
            DEBUG_PRINT("Board \"base addr\": %p", &game_state.board);
            DEBUG_PRINT("Clearing: %p", game_state.board + line*10);
#endif /* DEBUG */
            memset(game_state.board + line*10, 0, sizeof(uint8_t) * 10);
            continue;
        }

        if (at_cleared) {
            memcpy(game_state.board + (line+amt)*10, game_state.board + line*10, sizeof(uint8_t) * 10);
        }
    } while (line-- != 0);
}


static void fill_bag(void)
{
    random_bag.num_left = 0;

    bool exists[AMT_TETROS] = { false };
    uint8_t next_piece;

    for (uint8_t i = 0; i < AMT_TETROS; i++) {
        do {
            next_piece = rand() % AMT_TETROS;
        } while (exists[next_piece]);

        random_bag.bag[i] = next_piece;
        random_bag.num_left += 1;

        exists[next_piece] = true;
    }
}


static uint8_t get_next(void)
{
    if (!random_bag.num_left) {
        fill_bag();
    }

    return random_bag.bag[AMT_TETROS - random_bag.num_left--];
}


void try_move_piece(move_t move)
{
    switch(move) {
        case MOVE_DOWN:
            move_down();
            break;
        case MOVE_LEFT:
        case MOVE_RIGHT:
            move_horizontal(move);
            break;
        case ROTATE_LEFT:
        case ROTATE_RIGHT:
            rotate(move);
            break;
        default:
            __builtin_unreachable();
    }
}


/* Checks if current piece still fits at position in board */
static bool piece_fits(tetromino_t *tetro, pos_t pos)
{
    for (int16_t i = 0; i < TETRO_BUF_SIZE; i++)
    {
        if (tetro->piece[i] != ' ') {
            /* checks if out-of-bounds of board */
            if ((pos.x/2 + i % TETRO_SIZE) >= 10 ||
                (pos.x/2 + i % TETRO_SIZE) < 0) {
                return false;
            }

            if ((pos.y + i / TETRO_SIZE) >= 20 ||
                (pos.y + i / TETRO_SIZE) < 0) {
                return false;
            }

            /* if not, check if space is empty in board */
            if (game_state.board[((pos.y + i / TETRO_SIZE) * 10) +
                                   pos.x/2 + i % TETRO_SIZE] != 0) {
                return false;
            }
        }
    }

    return true;
}


static void piece_to_board(void)
{
    pos_t pos = game_state.curr_pos;

    for (uint16_t i = 0; i < TETRO_BUF_SIZE; i++) {
        if (game_state.curr_tetro.piece[i] != ' ') {
            game_state.board[((pos.y + i / TETRO_SIZE) * 10) +
                               pos.x/2 + i % TETRO_SIZE]
                               = game_state.curr_tetro.shape;
        }
    }
}


static uint16_t rotated_index(pos_t p, shape_t shape)
{
    switch (shape)
    {
        /* rotate around the middle of the 4x4 */
        case TETRO_I:
        case TETRO_O:
            return 12+p.y-4*p.x;

        /* rotate around the middle of the upper left 3x3 */
        case TETRO_J:
        case TETRO_L:
        case TETRO_S:
        case TETRO_Z:
        case TETRO_T:
            /* check if point is outside of the 3x3, and if so,
            just return its original index */
            if ((p.x == 3) || (p.y == 3)) {
                return p.x + 4*p.y;
            }
            return 8+p.y-4*p.x;
        default:
            __builtin_unreachable();
    }
}


/* Tries to rotate the current tetromino */
static void rotate(move_t move)
{
    tetromino_t rotated = game_state.curr_tetro;

    /* Rotate piece here... */
    if (rotated.shape == TETRO_I || rotated.shape == TETRO_O) {
        /* 4x4 rotation*/
        for (uint16_t i = 0; i < TETRO_SIZE; i++) {
            for (uint16_t j = 0; j < TETRO_SIZE; j++) {
                pos_t p = { i, j };

                rotated.piece[rotated_index(p, game_state.curr_tetro.shape)] =
                    game_state.curr_tetro.piece[i + TETRO_SIZE * j];
            }
        }
    } else {
        /* 3x3 rotation */
        for (uint16_t i = 0; i < TETRO_SIZE; i++) {
            for (uint16_t j = 0; j < TETRO_SIZE; j++) {
                pos_t p = { i, j };

                rotated.piece[rotated_index(p, game_state.curr_tetro.shape)] =
                    game_state.curr_tetro.piece[i + TETRO_SIZE * j];
            }
        }
    }

    if (!piece_fits(&rotated, game_state.curr_pos)) {
        return;
    }

    game_state.curr_tetro = rotated;
}


static void move_horizontal(move_t direction)
{
    pos_t pos = game_state.curr_pos;

    switch(direction)
    {
        case MOVE_LEFT:
            pos.x -= 2;
            if (!piece_fits(&game_state.curr_tetro, pos)) return;
            game_state.curr_pos.x -= 2;
            break;
        case MOVE_RIGHT:
            pos.x += 2;
            if (!piece_fits(&game_state.curr_tetro, pos)) return;
            game_state.curr_pos.x += 2;
            break;
        case MOVE_DOWN:
        case ROTATE_LEFT:
        case ROTATE_RIGHT:
            __builtin_unreachable();
    }
}


static bool move_down(void)
{
    /* TODO: needs a bit of fixing :D */

    pos_t down_pos = game_state.curr_pos;
    down_pos.y++;

    if (piece_fits(&game_state.curr_tetro, down_pos)) {
        game_state.curr_pos.y++;
        return true;
    }

    return false;
}


void pause(void)
{
    /* pauses game until uses presses p again */
    /* TODO: "Game paused" message */
    char input = 0;
    while (input != 'p')
        input = getch();
}


void reset(void)
{
    game_state.current = game_state.next;
    game_state.curr_tetro = tetrominoes[game_state.current];
    game_state.next = get_next();

    game_state.curr_pos.x = (BOARD_WIDTH/2)-4;
    game_state.curr_pos.y = 0;

    if (!piece_fits(&game_state.curr_tetro, game_state.curr_pos)) game_over = true;
}
