#ifndef CHESS_UI_H
#define CHESS_UI_H

#include <ncurses.h>

#define SQUARE_HEIGHT 3
#define SQUARE_WIDTH 6
#define LABEL_OFFSET_X 3
#define LABEL_OFFSET_Y 1
#define TABLE_HEIGHT (SQUARE_HEIGHT*8)
#define TABLE_WIDTH (SQUARE_WIDTH*8)
#define WINDOW_SIZE 0.8

#define MIN_ROWS (TABLE_HEIGHT + 4)
#define MIN_COLS (TABLE_WIDTH + 4)

#define COLOR_BROWN 8
#define COLOR_BEJ 9
#define COLOR_GREY 10
#define BOARD_SIZE 8
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"


typedef struct {
    int color;
    char type;
    int row;
    int col;
} piece_t;

typedef struct {
    int can_castle_short; // O-O
    int can_castle_long; // O-O-O
    int color;
    int is_in_check;
    char username[50];
	int connfd;
}player_t;

typedef struct {
    piece_t board[BOARD_SIZE][BOARD_SIZE];
    player_t white;
    player_t black;
}board_t;

typedef struct {
    int from_x, from_y;
    int to_x, to_y;
    int move_made; // 1 if move has been completed
} MOVE_T;

typedef enum {
    COLOR_PAIR_BLACK_LIGHT = 1,
    COLOR_PAIR_BLACK_DARK,
    COLOR_PAIR_WHITE_LIGHT,
    COLOR_PAIR_WHITE_DARK,
    COLOR_PAIR_LIGHT,
    COLOR_PAIR_DARK,
    COLOR_PAIR_LABELS,
    COLOR_PAIR_HIGHLIGHT,
    COLOR_PAIR_HIGHLIGHT_BLACK,
    COLOR_PAIR_HIGHLIGHT_WHITE,
    COLOR_PAIR_HIGHLIGHT_RED,
    COLOR_PAIR_HIGHLIGHT_RED_BLACK,
    COLOR_PAIR_HIGHLIGHT_RED_WHITE
} colorpairs_t;

/**
 * Maps a piece character and color to a Unicode chess symbol
 */
const char* map_to_unicode(char piece, int color);

/**
 * Initializes all custom color pairs used by the UI
 */
void init_colors(void);

/**
 * Draws a single square at (row, col) on the board
 */
void draw_square(int row, int col, board_t board,
                 int color_pair, int foreground_color_w, int foreground_color_b);

/**
 * Renders the entire chess board
 */
void render_board(board_t board);

/*
 * Handles mouse input and returns a MOVE_T when a move has been completed
 */
MOVE_T ui_return_move(board_t board);

/**
 * Renders a move on the board, including highlights for invalid moves
 */
void ui_render_move(MOVE_T move, board_t board, int move_succesfull);

void init_ui(board_t b, char* player_color);

#endif // CHESS_UI_H