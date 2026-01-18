#include <stdio.h>
#include "chess_logic.h"

int main(){

    printf("%ld\n", sizeof(board_t)-  2 * sizeof(player_t));
    return 0;
}