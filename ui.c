#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <locale.h>
#include <string.h>

#include "chess_logic.h"


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
    COLOR_PAIR_HIGHLIGHT_WHITE
} colorpairs_t;


const char *map_to_unicode(char piece, int color) {
    static char buf[16] = {0};

    switch (piece) {
        case 'p':
            strcpy(buf, color ? "\u265F" : "\u2659"); // ♟ : ♙
            break;
        case 'k':
            strcpy(buf, color ? "\u265A" : "\u2654"); // ♚ : ♔
            break;
        case 'n':
            strcpy(buf, color ? "\u265E" : "\u2658"); // ♞ : ♘
            break;
        case 'b':
            strcpy(buf, color ? "\u265D" : "\u2657"); // ♝ : ♗
            break;
        case 'q':
            strcpy(buf, color ? "\u265B" : "\u2655"); // ♛ : ♕
            break;
        case 'r':
            strcpy(buf, color ? "\u265C" : "\u2656"); // ♜ : ♖
            break;
        default:
            strcpy(buf, "?");
            break;
    }

    return buf;
}

void init_colors() {
    init_color(COLOR_BROWN, 585, 300, 132);
    init_color(COLOR_BEJ, 929, 859, 589);
    init_color(COLOR_GREY, 500, 500, 500);
    //black pieces
    init_pair(COLOR_PAIR_BLACK_LIGHT, COLOR_GREY, COLOR_BEJ);
    init_pair(COLOR_PAIR_BLACK_DARK, COLOR_GREY, COLOR_BROWN);

    //white pieces
    init_pair(COLOR_PAIR_WHITE_LIGHT, COLOR_WHITE, COLOR_BEJ);
    init_pair(COLOR_PAIR_WHITE_DARK, COLOR_WHITE, COLOR_BROWN);

    //board colors
    init_pair(COLOR_PAIR_LIGHT, COLOR_BLACK, COLOR_BEJ);
    init_pair(COLOR_PAIR_DARK, COLOR_WHITE, COLOR_BROWN);

    //labels
    init_pair(COLOR_PAIR_LABELS, COLOR_WHITE, COLOR_BLACK);

    //highlighted square
    init_pair(COLOR_PAIR_HIGHLIGHT, COLOR_BROWN, COLOR_GREEN);
    init_pair(COLOR_PAIR_HIGHLIGHT_BLACK, COLOR_GREY, COLOR_GREEN);
    init_pair(COLOR_PAIR_HIGHLIGHT_WHITE, COLOR_WHITE, COLOR_GREEN);
}

void draw_square(WINDOW *win, int row, int col, board_t board, int color_pair) {
    int win_width, win_height;
    getmaxyx(win, win_height, win_width);
    int board_start_x = (win_width - TABLE_WIDTH) / 2;
    int board_start_y = (win_height - TABLE_HEIGHT) / 2;
    int y = board_start_y + row * SQUARE_HEIGHT;
    int x = board_start_x + col * SQUARE_WIDTH;
    wattron(win, COLOR_PAIR(color_pair));
    for (int i = 0; i < SQUARE_HEIGHT; i++) {
        mvwhline(win, y + i, x, ' ', SQUARE_WIDTH);
    }
    wattroff(win, COLOR_PAIR(color_pair));
    if (board.board[row][col].color != -1) {
        switch (board.board[row][col].color) {
            case 0:
                wattron(win, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT_WHITE)|A_BOLD);
                break;
            case 1:
                wattron(win, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT_BLACK)|A_BOLD);
                break;
        }
        const char *uni = map_to_unicode(board.board[row][col].type, board.board[row][col].color);
        mvwaddstr(win, y+LABEL_OFFSET_Y, x+LABEL_OFFSET_X, uni);
        wattroff(win, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT_WHITE)|COLOR_PAIR(COLOR_PAIR_HIGHLIGHT_BLACK)| A_BOLD);
    }
    wrefresh(win);
}

void render_board(WINDOW *win, board_t board) {
    int win_width, win_height;
    getmaxyx(win, win_height, win_width);
    int board_start_x = (win_width - TABLE_WIDTH) / 2;
    int board_start_y = (win_height - TABLE_HEIGHT) / 2;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int y = board_start_y + row * SQUARE_HEIGHT;
            int x = board_start_x + col * SQUARE_WIDTH;

            // Set color for square
            if ((row + col) % 2 == 0) {
                wattron(win, COLOR_PAIR(COLOR_PAIR_LIGHT)); // Light
            } else {
                wattron(win, COLOR_PAIR(COLOR_PAIR_DARK)); // Dark
            }

            // Draw square
            for (int i = 0; i < SQUARE_HEIGHT; i++) {
                mvwhline(win, y + i, x, ' ', SQUARE_WIDTH);
            }
            wattroff(win, COLOR_PAIR(COLOR_PAIR_LIGHT)| COLOR_PAIR(COLOR_PAIR_DARK));

            if (board.board[row][col].color != -1) {
                switch (board.board[row][col].color) {
                    case 0:
                        if ((row + col) % 2 == 0) {
                            wattron(win, COLOR_PAIR(COLOR_PAIR_WHITE_LIGHT)|A_BOLD); // Light
                        } else {
                            wattron(win, COLOR_PAIR(COLOR_PAIR_WHITE_DARK)|A_BOLD); // Dark
                        }
                        break;
                    case 1:
                        if ((row + col) % 2 == 0) {
                            wattron(win, COLOR_PAIR(COLOR_PAIR_BLACK_LIGHT)|A_BOLD); // Light
                        } else {
                            wattron(win, COLOR_PAIR(COLOR_PAIR_BLACK_DARK)|A_BOLD); // Dark
                        }
                        break;
                }
                const char *uni = map_to_unicode(board.board[row][col].type, board.board[row][col].color);
                mvwaddstr(win, y+LABEL_OFFSET_Y, x+LABEL_OFFSET_X, uni);


                wattroff(
                    win, COLOR_PAIR(COLOR_PAIR_BLACK_LIGHT) | COLOR_PAIR(COLOR_PAIR_BLACK_DARK) | COLOR_PAIR(
                        COLOR_PAIR_WHITE_LIGHT)|COLOR_PAIR(COLOR_PAIR_WHITE_DARK)| A_BOLD);
            }


            // // Draw coordinates
            wattron(win, COLOR_PAIR(COLOR_PAIR_LABELS));

            if (row == 7) {
                mvwprintw(win, y + SQUARE_HEIGHT + LABEL_OFFSET_Y, x + LABEL_OFFSET_X, "%c", 'a' + col);
            }
            if (col == 0) {
                mvwprintw(win, y + LABEL_OFFSET_Y, x - LABEL_OFFSET_X, "%d", 8 - row);
            }
            wattroff(win, COLOR_PAIR(COLOR_PAIR_LABELS));
        }
    }
    wrefresh(win);
}

void mouse_click(int ch, WINDOW *win, board_t board) {
    mousemask(BUTTON1_PRESSED | BUTTON2_PRESSED, NULL);
    MEVENT event;


    int win_x, win_y, win_width, win_height;
    getbegyx(win, win_y, win_x);
    getmaxyx(win, win_height, win_width);
    int board_start_x = (win_width - TABLE_WIDTH) / 2 + win_x;
    int board_start_y = (win_height - TABLE_HEIGHT) / 2 + win_y;
    if (ch == KEY_MOUSE) {
        if (getmouse(&event) == OK) {
            if (event.bstate & BUTTON1_PRESSED) {
                if (event.x >= win_x && event.y >= win_y && event.x < win_x + win_width && event.y < win_y +
                    win_height) {
                    // move(2, 0);
                    // printw("x_board:%d y_board:%d", board_start_x, board_start_y);
                    // mvwprintw(win, win_y, win_x, "x:%d y:%d", event.x, event.y);


                    int board_end_x = board_start_x + TABLE_WIDTH;
                    int board_end_y = board_start_y + TABLE_HEIGHT;
                    if (event.x >= board_start_x && event.y >= board_start_y && event.x < board_end_x && event.y <
                        board_end_y) {
                        int index_matrix_x = (event.x - board_start_x) / SQUARE_WIDTH;
                        int index_matrix_y = (event.y - board_start_y) / SQUARE_HEIGHT;
                        // move(3, 0);
                        // printw("%d %d", index_matrix_x, index_matrix_y);
                        if (board.board[index_matrix_y][index_matrix_x].color != -1) {
                            draw_square(win, index_matrix_y, index_matrix_x, board, COLOR_PAIR_HIGHLIGHT);
                        }
                    }


                    wrefresh(win);
                }
            }
        }
    }
}

int main(void) {
    int ch;

    board_t board;
    board = init_board(board);
    board.board[3][1] = (piece_t){0, 'k', 1, 3};
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    mouseinterval(0);
    curs_set(0);

    int term_rows = LINES;
    int term_cols = COLS;

    if (term_rows < MIN_ROWS || term_cols < MIN_COLS) {
        endwin();
        printf("Terminal too small! Minimum size required: %d rows x %d cols\n",
               MIN_ROWS, MIN_COLS);
        printf("Current size: %d rows x %d cols\n", term_rows, term_cols);
        return 1;
    }

    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);

    start_color();
    if (can_change_color()) {
        init_colors();
    }

    printw("Press ESC to exit");
    refresh();

    int win_height = term_rows * WINDOW_SIZE;
    int win_width = term_cols * WINDOW_SIZE;

    // Ensure window is at least big enough for the board
    if (win_height < MIN_ROWS) win_height = MIN_ROWS;
    if (win_width < MIN_COLS) win_width = MIN_COLS;

    // Center the window
    int starty = (term_rows - win_height) / 2;
    int startx = (term_cols - win_width) / 2;

    // move(1,0);
    // printw("%d %d",startx,starty);

    WINDOW *square = newwin(win_height, win_width, starty, startx);
    box(square, 0, 0);
    wrefresh(square);
    set_escdelay(0);
    render_board(square, board);
    while ((ch = getch()) != 27) {
        mouse_click(ch, square, board);
    }
    endwin();
    return 0;
}
