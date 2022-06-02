#ifndef TETRIS_H
#define TETRIS_H

#ifndef _XOPEN_SOURCE_EXTENDED
    /* Needs to be defined for wide character curses functions */
    #define _XOPEN_SOURCE_EXTENDED
#endif /* _XOPEN_SOURCE_EXTENDED */

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define TOTAL_CHARS (NUM_BLOCKS + NUM_SPACES)
#define NUM_BLOCKS 8
#define BLOCK_BYTE_SIZE 3
#define NUM_SPACES 24
#define SPACE_BYTE_SIZE 1
#define TOTAL_BYTE_SIZE \
    ((NUM_SPACES*SPACE_BYTE_SIZE) + \
     (NUM_BLOCKS*BLOCK_BYTE_SIZE))

/* Width and height in terms of characters in the terminal */
#define BOARD_HEIGHT 20
#define BOARD_WIDTH  20

#define TOTAL_BOARD_WIDTH 22
#define TOTAL_BOARD_HEIGHT 22

#define SIZE 8
#define TETRO_BUF_SIZE (TETRO_SIZE * TETRO_SIZE)
#define TETRO_SIZE 4
#define AMT_TETROS 7


typedef enum shape {
    TETRO_I = 1,
    TETRO_J,
    TETRO_L,
    TETRO_O,
    TETRO_S,
    TETRO_Z,
    TETRO_T
} shape_t;


typedef enum color_attr {
    COL_CYAN = 1,
    COL_BLUE,
    COL_ORANGE,
    COL_YELLOW,
    COL_GREEN,
    COL_RED,
    COL_MAGENTA,
    COL_GHOST           /* Color for a "ghost" piece showing where current piece would land */
} color_attr_t;


/* 2D position structure */
typedef struct pos {
    int16_t x, y;
} pos_t;


/* Tetromino structure describing shape and color */
typedef struct tetromino {
    shape_t shape;                  /* What tetromino */
    uint16_t color;                 /* Color pair id */
    uint8_t piece[TETRO_BUF_SIZE];  /* 4x4 "grid" of the layout */
} tetromino_t;


typedef struct tetris_state {
    uint32_t score;     /* Current score */
    pos_t curr_pos;     /* Current tetromino's position */
    tetromino_t curr_tetro;
    uint8_t next;       /* ID of next tetromino */
    uint8_t current;    /* ID of current tetromino */
    uint8_t board[10*20];
} state_t;


typedef struct bag_state {
    uint8_t bag[AMT_TETROS];
    uint8_t num_left;
} bag_t;


typedef enum move {
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    ROTATE_LEFT,
    ROTATE_RIGHT
} move_t;


extern const tetromino_t tetrominoes[AMT_TETROS];

extern bool game_over;
extern state_t game_state;

/* Function prototypes */

void tetris_init(void);
void tetris_update(void);
void tetris_end(void);

void try_move_piece(move_t move); /* move piece */

void pause(void);
void reset(void);

#endif /* TETRIS_H */
