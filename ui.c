#include <stdio.h>
#include <ncurses.h>
#include <wchar.h>
#include <locale.h>

#define SQUARE_HEIGHT 3
#define SQUARE_WIDTH 6
#define LABEL_OFFSET_X 3
#define LABEL_OFFSET_Y 1
#define TABLE_HEIGHT (SQUARE_HEIGHT*8)
#define TABLE_WIDTH (SQUARE_WIDTH*8)
#define HEIGHT 40
#define WIDTH 80


#define COLOR_BROWN 8
#define COLOR_BEJ 9
#define COLOR_GREY 10


#define BOARD_SIZE 8
#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"
#define BLK_OFFSET 6

typedef struct {
    int color;
    char type;
    int row;
    int col;
} piece_t;

typedef enum
{
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

typedef struct {
    piece_t board[BOARD_SIZE][BOARD_SIZE];
}board_t;
piece_t get_piece(int color, char type, int row, int col){
    piece_t p;
    p.color = color; // 0 = white, 1 = black, -1 = empty
    p.type  = type; // . = empty
    p.row   = row;
    p.col   = col;
    return p;
}

board_t init_board(board_t b){
    for(int i = 2; i < 6; i++){
        for(int j = 0; j < BOARD_SIZE; j++){
            b.board[i][j] = get_piece(-1, '.', i, j);
        }
    }
    for(int i = 0; i < BOARD_SIZE; i++){
        b.board[1][i] = get_piece(1, 'p', 1, i);
        b.board[6][i] = get_piece(0, 'p', 6, i);
    }
    char pieces[8] = {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'};
    for(int i = 0; i < BOARD_SIZE; i++){
        b.board[0][i] = get_piece(1, pieces[i], 0, i);
        b.board[7][i] = get_piece(0,pieces[i],7,i);
    }
    return b;
}

void print_board(board_t b){
    for(int i = 0; i < BOARD_SIZE; i++){
        printf("--");
    }
    printf("-\n");

    //
    printf("  ");
    for(int i = 0; i < BOARD_SIZE; i++){
        printf("%d ", i);
    }
    printf("\n");

    for(int i = 0; i < BOARD_SIZE + 1; i++){
        for(int j = 0; j < BOARD_SIZE; j++){

            if(i == BOARD_SIZE){
                if(j == 0){
                    printf("  ");
                    continue;
                }
                else{
                    printf("%c ", 'a' + j - 1);
                    if(j == BOARD_SIZE-1){
                        printf("%c ", 'a' + j);
                    }
                }
                continue;
            }

            if(j == 0){
                printf("%d ", BOARD_SIZE - i);

            }
            if(b.board[i][j].color == 1){
                printf(BLUE "%c " RESET, b.board[i][j].type);
            }
            else{
                printf("%c ", b.board[i][j].type);
            }
        }
        printf(" %d\n", i);
    }
    for(int i = 0; i < BOARD_SIZE; i++){
        printf("--");
    }
    printf("-\n");
}

const wchar_t* map_to_unicode(char piece, int color) {
    static wchar_t buf[2];     // [character, null terminator]
    int offset = color ? BLK_OFFSET : 0;
    wchar_t ch = L'?';         // fallback

    switch (piece) {
        case 'p': ch = 0x2659 + offset; break;
        case 'k': ch = 0x2654 + offset; break;
        case 'n': ch = 0x2658 + offset; break;
        case 'b': ch = 0x2657 + offset; break;
        case 'q': ch = 0x2655 + offset; break;
        case 'r': ch = 0x2656 + offset; break;
    }

    buf[0] = ch;
    buf[1] = L'\0';
    return buf;
}

void init_colors() {
    init_color(COLOR_BROWN,585,300,132);
    init_color(COLOR_BEJ,929,859,589);
	init_color(COLOR_GREY, 500, 500, 500);
}

void draw_square(WINDOW* win,int row,int col,board_t board,int color_pair) {
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
	if(board.board[row][col].color !=-1){
        switch(board.board[row][col].color){
            case 0:
                wattron(win,COLOR_PAIR(COLOR_PAIR_HIGHLIGHT_WHITE)|A_BOLD);
                break;
            case 1:
                wattron(win,COLOR_PAIR(COLOR_PAIR_HIGHLIGHT_BLACK)|A_BOLD);
                break;
        }
        mvwaddch(win, y+LABEL_OFFSET_Y,x+LABEL_OFFSET_X,board.board[row][col].type);
        wattroff(win, COLOR_PAIR(COLOR_PAIR_HIGHLIGHT_WHITE)|COLOR_PAIR(COLOR_PAIR_HIGHLIGHT_BLACK)| A_BOLD);
    }
    wrefresh(win);
}

void render_board(WINDOW* win,board_t board) {
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
                wattron(win, COLOR_PAIR(COLOR_PAIR_LIGHT));  // Light
            } else {
                wattron(win, COLOR_PAIR(COLOR_PAIR_DARK));  // Dark
            }

            // Draw square
            for (int i = 0; i < SQUARE_HEIGHT; i++) {
                mvwhline(win, y + i, x, ' ', SQUARE_WIDTH);
            }
			wattroff(win, COLOR_PAIR(COLOR_PAIR_LIGHT)| COLOR_PAIR(COLOR_PAIR_DARK));

			if(board.board[row][col].color !=-1){
				switch(board.board[row][col].color){
                    case 0:
                        if ((row + col) % 2 == 0) {
                            wattron(win, COLOR_PAIR(COLOR_PAIR_WHITE_LIGHT)|A_BOLD);  // Light
                        } else {
                            wattron(win, COLOR_PAIR(COLOR_PAIR_WHITE_DARK)|A_BOLD);  // Dark
                        }
                        break;
                    case 1:
                        if ((row + col) % 2 == 0) {
                            wattron(win, COLOR_PAIR(COLOR_PAIR_BLACK_LIGHT)|A_BOLD);  // Light
                        } else {
                            wattron(win, COLOR_PAIR(COLOR_PAIR_BLACK_DARK)|A_BOLD);  // Dark
                        }
                        break;
                }
				mvwaddch(win, y+LABEL_OFFSET_Y,x+LABEL_OFFSET_X,board.board[row][col].type);
                wattroff(win, COLOR_PAIR(COLOR_PAIR_BLACK_LIGHT) | COLOR_PAIR(COLOR_PAIR_BLACK_DARK) | COLOR_PAIR(COLOR_PAIR_WHITE_LIGHT)|COLOR_PAIR(COLOR_PAIR_WHITE_DARK)| A_BOLD);
			}


            // // Draw coordinates
            wattron(win, COLOR_PAIR(COLOR_PAIR_LABELS));

            if (row == 7) {
                mvwprintw(win, y + SQUARE_HEIGHT+LABEL_OFFSET_Y, x + LABEL_OFFSET_X, "%c", 'a' + col);
            }
            if (col == 0) {

                mvwprintw(win, y + LABEL_OFFSET_Y, x - LABEL_OFFSET_X, "%d", 8 - row);
            }
            wattroff(win, COLOR_PAIR(COLOR_PAIR_LABELS));
        }
    }
    wrefresh(win);


}

void mouse_click(int ch,WINDOW* win,board_t board) {
    mousemask(BUTTON1_PRESSED|BUTTON2_PRESSED, NULL);
    MEVENT event;


    int win_x, win_y, win_width, win_height;
    getbegyx(win, win_y, win_x);
    getmaxyx(win, win_height, win_width);
    int board_start_x = (win_width - TABLE_WIDTH) / 2 +win_x;
    int board_start_y = (win_height - TABLE_HEIGHT) / 2+win_y;
    if(ch == KEY_MOUSE) {
        if(getmouse(&event) == OK) {
            if(event.bstate & BUTTON1_PRESSED) {
                if (event.x>=win_x && event.y>=win_y && event.x<win_x+win_width && event.y<win_y+win_height) {
                    move(2,0);
                    printw("x_board:%d y_board:%d",board_start_x,board_start_y);
                    mvwprintw(win,win_y,win_x,"x:%d y:%d",event.x,event.y);


                    int board_end_x=board_start_x+TABLE_WIDTH;
                    int board_end_y=board_start_y+TABLE_HEIGHT;
                    if (event.x>=board_start_x && event.y>=board_start_y && event.x<board_end_x && event.y<board_end_y) {
                        int index_matrix_x=(event.x-board_start_x)/SQUARE_WIDTH;
                        int index_matrix_y=(event.y-board_start_y)/SQUARE_HEIGHT;
                        move(3,0);
                        printw("%d %d",index_matrix_x,index_matrix_y);
						if(board.board[index_matrix_y][index_matrix_x].color !=-1){
                        	draw_square(win,index_matrix_y,index_matrix_x,board,COLOR_PAIR_HIGHLIGHT);
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

    board_t board ;
    board=init_board(board);
	board.board[3][1]=(piece_t){0,'k',1,3}; //check for piece placement
    setlocale(LC_CTYPE, "");
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    mouseinterval(0);
    curs_set(0); // Hide cursor

    // Enable mouse
    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);
    start_color();
    init_colors();
    if (can_change_color()) {
		//black pieces
       	init_pair(COLOR_PAIR_BLACK_LIGHT, COLOR_GREY, COLOR_BEJ);
		init_pair(COLOR_PAIR_BLACK_DARK, COLOR_GREY, COLOR_BROWN);

		//white pieces
		init_pair(COLOR_PAIR_WHITE_LIGHT, COLOR_WHITE, COLOR_BEJ);
		init_pair(COLOR_PAIR_WHITE_DARK,  COLOR_WHITE, COLOR_BROWN);

        //board colors
		init_pair(COLOR_PAIR_LIGHT, COLOR_BLACK, COLOR_BEJ);
		init_pair(COLOR_PAIR_DARK,  COLOR_WHITE, COLOR_BROWN);

		//labels
		init_pair(COLOR_PAIR_LABELS, COLOR_WHITE, COLOR_BLACK);

		//highlighted square
		init_pair(COLOR_PAIR_HIGHLIGHT,        COLOR_BROWN, COLOR_GREEN);
		init_pair(COLOR_PAIR_HIGHLIGHT_BLACK,  COLOR_GREY,  COLOR_GREEN);
		init_pair(COLOR_PAIR_HIGHLIGHT_WHITE,  COLOR_WHITE, COLOR_GREEN);
    }

    printw("Press ESC to exit");
    refresh();// Disable line buffering
    int starty = (LINES - HEIGHT) / 2;	/* Calculating for a center placement */
    int startx = (COLS - WIDTH) / 2;
    move(1,0);
    printw("%d %d",startx,starty);
    WINDOW* square=newwin(HEIGHT, WIDTH, starty, startx);
    box(square, 0, 0);
    wrefresh(square);
    set_escdelay(0);
    render_board(square,board);
    while(( ch = getch()) != 27)
    {

        mouse_click(ch,square,board);
    }
    endwin();
    return 0;
}

