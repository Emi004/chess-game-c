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
    b = init_board(b, white, black);
    //print_board(b);
    
    
    //printf(BLUE "this is blue\n" RESET);

    char input[128];
    int turn = 0;
    for(int i = 0; i < 10; i++){
        print_board(b);
        printf("turn: %d\n", turn);
        if(fgets(input, sizeof(input), stdin) != NULL){
            int res = chess_main(&b, &turn, input);
            switch(res){
                case 0:
                    printf("invalid move\n");
                    break;
                case 1:
                    printf("valid move\n");
                    break;
                case 2:
                    break;
                case 3:

                    break;
                default:
                    break;
            }
        }
    }
}