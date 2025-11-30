#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "chess_logic.h"

int main(void){
    board_t b;
    player_t white = { 1, 1, 0, 0 };
    player_t black = { 1, 1, 1, 0 };
    char from_col, to_col;
    int  from_row, to_row;
    b = init_board(b);
    print_board(b);
    char input[128];
    while(game_running){
        if( !turn ){
            printf("White's turn; ");
        }
        else {
            printf("Black's turn; ");
        }
        printf("Enter move (e.g., e2 e4): ");
        if(fgets(input, sizeof(input), stdin) != NULL){
            if(
                sscanf(input, " %c%d %c%d", &from_col, &from_row, &to_col, &to_row) == 4
            ){
                if( turn == 0 ){
                    if( white.is_in_check == 1 ){
                        printf("checking for checkmate for white\n");
                        check_for_checkmate(b, 0, white, black);
                    }
                    b = validate_move(b, white, white, black, from_col, from_row, to_col, to_row);
                    print_board(b);
                    if( move_succesfull ){
                        turn = 1 - turn;
                    }
                }
                else{
                    if( black.is_in_check == 1 ){
                        printf("checking for checkmate for black\n");
                        check_for_checkmate(b, 1, white, black);
                    }
                    b = validate_move(b, black, white, black, from_col, from_row, to_col, to_row);
                    print_board(b);
                    if( move_succesfull ){
                        turn = 1 - turn;
                    }
                    else{
                        printf("invalid move.\n");
                    }
                }
            }
            else if( strcmp(input, "O-O\n") == 0 || strcmp(input, "o-o\n") == 0 ){
                printf("short castle\n");
            }
            else if( strcmp(input, "O-O-O\n") == 0 || strcmp(input, "o-o-o\n") == 0 ){
                printf("long castle\n");
            }
             else {
                printf("Invalid input format. Use: c2 c4\n");
            }
        }
    }
    //printf(BLUE "this is blue\n" RESET);
}