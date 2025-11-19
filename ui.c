#include <stdio.h>
#include <ncurses.h>


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


void init_colors() {
    init_color(COLOR_BROWN,585,300,132);
    init_color(COLOR_BEJ,929,859,589);
}

void draw_square(WINDOW* win,int row,int col) {
    int win_width, win_height;
    getmaxyx(win, win_height, win_width);
    int board_start_x = (win_width - TABLE_WIDTH) / 2;
    int board_start_y = (win_height - TABLE_HEIGHT) / 2;
    int y = board_start_y + row * SQUARE_HEIGHT;
    int x = board_start_x + col * SQUARE_WIDTH;
    wattron(win, COLOR_PAIR(1) | COLOR_PAIR(2));
    for (int i = 0; i < SQUARE_HEIGHT; i++) {
        mvwhline(win, y + i, x, ' ', SQUARE_WIDTH);
    }
    wattroff(win, COLOR_PAIR(1) | COLOR_PAIR(2));
}

void my_take(WINDOW* win) {
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
                wattron(win, COLOR_PAIR(5));  // Light
            } else {
                wattron(win, COLOR_PAIR(6));  // Dark
            }

            // Draw square
            for (int i = 0; i < SQUARE_HEIGHT; i++) {
                mvwhline(win, y + i, x, ' ', SQUARE_WIDTH);
            }

            if (row==0 || row ==1) {
                if ((row + col) % 2 == 0) {
                    wattron(win, COLOR_PAIR(1)|A_BOLD);  // Light
                } else {
                    wattron(win, COLOR_PAIR(2)|A_BOLD);  // Dark
                }
                mvwaddch(win, y+LABEL_OFFSET_Y,x+LABEL_OFFSET_X,'P');
                wattroff(win, COLOR_PAIR(1) | COLOR_PAIR(2) | A_BOLD);


            }

            if (row ==6 || row ==7) {
                if ((row + col) % 2 == 0) {
                    wattron(win, COLOR_PAIR(3)|A_BOLD);  // Light
                } else {
                    wattron(win, COLOR_PAIR(4)|A_BOLD);  // Dark
                }
                mvwaddch(win, y+LABEL_OFFSET_Y,x+LABEL_OFFSET_X,'P');
                wattroff(win, COLOR_PAIR(3) | COLOR_PAIR(4) | A_BOLD);

            }

            // // Draw coordinates
            wattroff(win, COLOR_PAIR(5) | COLOR_PAIR(6));
            wattron(win, COLOR_PAIR(7));

            if (row == 7) {
                mvwprintw(win, y + SQUARE_HEIGHT+LABEL_OFFSET_Y, x + LABEL_OFFSET_X, "%c", 'a' + col);
            }
            if (col == 0) {

                mvwprintw(win, y + LABEL_OFFSET_Y, x - LABEL_OFFSET_X, "%d", 8 - row);
            }
            wattroff(win, COLOR_PAIR(7));
        }
    }
    wrefresh(win);


}

void mouse_click(int ch,WINDOW* win) {
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
                        draw_square(win,index_matrix_y,index_matrix_x);


                    }






                    wrefresh(win);
                }

            }


        }
    }
}

int main(void) {
    int ch;

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
        init_pair(1, COLOR_BLACK, COLOR_BEJ);    // Black pieces on light
        init_pair(2,COLOR_BLACK,COLOR_BROWN); //black piece on black
        init_pair(3, COLOR_WHITE, COLOR_BEJ);  // white pieces on light
        init_pair(4, COLOR_WHITE, COLOR_BROWN); //white pieces on LIGHT
        init_pair(5, COLOR_BLACK, COLOR_BEJ);    // Light squares
        init_pair(6, COLOR_WHITE, COLOR_BROWN);  // Dark squares
        init_pair(7, COLOR_WHITE, COLOR_BLACK); //labels
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
    my_take(square);
    while(( ch = getch()) != 27)
    {

        mouse_click(ch,square);
    }
    endwin();
    return 0;
}

