#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "chess_logic.h"

int game_running = 1;
int move_succesfull = 1;
int turn = 0;


piece_t get_piece(int color, char type, int row, int col){
    piece_t p;
    p.color = color; // 0 = white, 1 = black, -1 = empty
    p.type  = type; // . = empty
    p.row   = row;
    p.col   = col;
    return p;
}

void print_piece(piece_t p){
    printf("%d, %c, %d, %d\n", p.color, p.type, p.row, p.col);
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

int char_to_index(char c) {
    if (c >= 'a' && c <= 'z') { 
        return c - 'a';
    }
    return -1;  
}

char index_to_file(int index) {
    if (index >= 0 && index <= 7) {
        return 'a' + index;
    }
    return '?';
}

int is_in_bounds(int i, int j){
    return ( i >= 0 && i < 8 && j >= 0 && j < 8);
}

int does_rook_see_king(board_t b, int from_i, int from_j, char direction){
    for(int i = 1; i < BOARD_SIZE; i++){
        int step_i, step_j;
        switch(direction){
            case 'l':
                step_i = from_i;
                step_j = from_j - i; 
                break;
            case 'r':
                step_i = from_i;
                step_j = from_j + i; 
                break;
            case 'u':
                step_i = from_i - i;
                step_j = from_j; 
                break;
            case 'd':
                step_i = from_i + i;
                step_j = from_j; 
                break;
            default:
                break;
        }
        if( is_in_bounds(step_i, step_j) ){
            if(b.board[from_i][from_j].color != b.board[step_i][step_j].color && b.board[step_i][step_j].type == 'k'){
                return 1;
            }
            if(b.board[step_i][step_j].type != '.'){
                return 0;
            }
        }
    }
    return 0;
}

int does_bishop_see_king(board_t b, int from_i, int from_j, char direction){
    for(int i = 1; i < BOARD_SIZE; i++){
        int step_i, step_j;
        switch(direction){
            case 'u':
                step_i = from_i - i;
                step_j = from_j - i; 
                break;
            case 'U':
                step_i = from_i - i;
                step_j = from_j + i; 
                break;
            case 'l':
                step_i = from_i + i;
                step_j = from_j - i; 
                break;
            case 'L':
                step_i = from_i + i;
                step_j = from_j + i; 
                break;
            default:
                break;
        }
        if( is_in_bounds(step_i, step_j) ){
            if(b.board[from_i][from_j].color != b.board[step_i][step_j].color && b.board[step_i][step_j].type == 'k'){
                return 1;
            }
            if(b.board[step_i][step_j].type != '.'){
                return 0;
            }
        }
    }
    return 0;
}

int check_for_check(board_t b, int player_to_be_checked, player_t white, player_t black){
    int player_color = 1 - player_to_be_checked;
    //int opponent_color = 1 - player_color;
    for( int i = 0; i < BOARD_SIZE; i++ ){
        for( int j = 0; j < BOARD_SIZE; j++ ){
            if( b.board[i][j].color == player_color 
                && b.board[i][j].type != '.'  
            )
            {
                if( b.board[i][j].type == 'p' ){
                    if( player_color == 0 ){ // white
                        if( is_in_bounds(i-1, j-1) ){
                             if(b.board[i-1][j-1].color == 1 && b.board[i-1][j-1].type == 'k'){
                                //black.is_in_check = 1;
                                return 1;
                             }
                        }
                        if( is_in_bounds(i-1, j+1) ){
                             if(b.board[i-1][j+1].color == 1 && b.board[i-1][j+1].type == 'k'){
                                //black.is_in_check = 1;
                                return 1;
                             }
                        }
                    }
                    else{ // black
                        if( is_in_bounds(i+1, j-1) ){
                             if(b.board[i+1][j-1].color == 0 && b.board[i+1][j-1].type == 'k'){
                                //black.is_in_check = 1;
                                return 1;
                             }
                        }
                        if( is_in_bounds(i+1, j+1) ){
                             if(b.board[i+1][j+1].color == 0 && b.board[i+1][j+1].type == 'k'){
                                //black.is_in_check = 1;
                                return 1;
                            }
                        }
                    }
                }
                else if( b.board[i][j].type == 'r' ){
                    // upwards check
                    if( does_rook_see_king(b, i, j, 'u') ){
                        return 1;
                    }
                    // downwards check
                    if( does_rook_see_king(b, i, j, 'd') ){
                        return 1;
                    }
                    // left check
                    if( does_rook_see_king(b, i, j, 'l') ){
                        return 1;
                    }
                    // right check
                    if( does_rook_see_king(b, i, j, 'r') ){
                        return 1;
                    }
                }
                else if( b.board[i][j].type == 'n' ){
                    for(int step_i = 0; step_i < 5; (step_i == 1 ? step_i += 2 : step_i++)){
                        if( (step_i - 2) % 2 == 0 ){
                            for(int step_j = 0; step_j <= 2; step_j += 2){
                                if( is_in_bounds(i + (step_i - 2), j + (step_j - 1)) ){
                                    if( b.board[i + step_i - 2][j + step_j - 1].color != player_color && b.board[i + step_i - 2][j + step_j - 1].type == 'k' ){
                                        return 1;
                                    }
                                }
                            }
                        }
                        else{
                            for(int step_j = 0; step_j <= 4; step_j += 4){
                                if( is_in_bounds(i + (step_i - 2), j + (step_j - 2)) ){
                                    if( b.board[i + step_i - 2][j + step_j - 2].color != player_color && b.board[i + step_i - 2][j + step_j - 2].type == 'k' ){
                                        return 1;
                                    }
                                }
                            }
                        }
                    }
                }
                else if( b.board[i][j].type == 'b' ){

                    if( does_bishop_see_king(b, i, j, 'u') ){
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'U') ){
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'l') ){
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'L') ){
                        return 1;
                    }
                }
                else if( b.board[i][j].type == 'q' ){

                    if( does_bishop_see_king(b, i, j, 'u') ){
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'U') ){
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'l') ){
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'L') ){
                        return 1;
                    }
                    // upwards check
                    if( does_rook_see_king(b, i, j, 'u') ){
                        return 1;
                    }
                    // downwards check
                    if( does_rook_see_king(b, i, j, 'd') ){
                        return 1;
                    }
                    // left check
                    if( does_rook_see_king(b, i, j, 'l') ){
                        return 1;
                    }
                    // right check
                    if( does_rook_see_king(b, i, j, 'r') ){
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

board_t make_move(board_t b, player_t white, player_t black, char from_col, int from_row, char to_col, int to_row){

    int from_i = (from_row - BOARD_SIZE) * -1;
    int from_j = char_to_index(from_col);
    int to_i   = (to_row - BOARD_SIZE) * -1;
    int to_j   = char_to_index(to_col);

    int color = b.board[from_i][from_j].color;
    char ptype = b.board[from_i][from_j].type;
    piece_t aux = b.board[from_i][from_j];
    b.board[from_i][from_j] = b.board[to_i][to_j];
    b.board[to_i][to_j] = aux; 

    if(color == 0){
        if( check_for_check(b, 0, white, black) ){
            printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(-1, '.', to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, ptype, from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            white.is_in_check = 0;
        }
        if( check_for_check(b, 1, white, black) ){
            printf("black in check.\n");
            black.is_in_check = 1;
            return b;
        }
    }
    if(color == 1){
        if( check_for_check(b, 1, white, black) ){
            printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(-1, '.', to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, ptype, from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else{
            black.is_in_check = 0;
        }
        if( check_for_check(b, 0, white, black) ){
            printf("white in check.\n");
            white.is_in_check = 1;
            return b;
        }
    }

    return b;
}

board_t take_piece(board_t b, player_t white, player_t black, char from_col, int from_row, char to_col, int to_row){

    int from_i = (from_row - BOARD_SIZE) * -1;
    int from_j = char_to_index(from_col);
    int to_i   = (to_row - BOARD_SIZE) * -1;
    int to_j   = char_to_index(to_col);


    int color = b.board[from_i][from_j].color;
    char aptype = b.board[from_i][from_j].type;
    char bptype = b.board[to_i][to_j].type;

    b.board[to_i][to_j] = b.board[from_i][from_j];
    b.board[from_i][from_j] = get_piece(-1, '.', from_i, from_j);

    if(color == 0){
        if( check_for_check(b, 0, white, black) ){
            printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(1 - color, bptype, to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, aptype, from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            white.is_in_check = 0;
        }
        if( check_for_check(b, 1, white, black) ){
            printf("black in check.\n");
            black.is_in_check = 1;
            return b;
        }
    }
    if(color == 1){
        if( check_for_check(b, 1, white, black) ){
            printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(1 - color, bptype, to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, aptype, from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            black.is_in_check = 0;
        }
        if( check_for_check(b, 0, white, black) ){
            printf("white in check.\n");
            white.is_in_check = 1;
            return b;
        }
    }


    return b;
}

board_t promote(board_t b, player_t white, player_t black, int color, char from_col, int from_row, char to_col, int to_row){

    int from_i = (from_row - BOARD_SIZE) * -1;
    int from_j = char_to_index(from_col);
    int to_i   = (to_row - BOARD_SIZE) * -1;
    int to_j   = char_to_index(to_col);

    b.board[to_i][to_j] = get_piece(color, 'q', to_i, to_j);
    b.board[from_i][from_j] = get_piece(-1, '.', from_i, from_j);

    if(color == 0){
        if( check_for_check(b, 0, white, black) ){
            printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(-1, '.', to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, 'p', from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            white.is_in_check = 0;
        }
        if( check_for_check(b, 1, white, black) ){
            printf("black in check.\n");
            black.is_in_check = 1;
            return b;
        }
    }
    if(color == 1){
        if( check_for_check(b, 1, white, black) ){
            printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(-1, '.', to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, 'p', from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            black.is_in_check = 0;
        }
        if( check_for_check(b, 0, white, black) ){
            printf("white in check.\n");
            white.is_in_check = 1;
            return b;
        }
    }

    return b;
}


int is_valid_rook_move(board_t b, int from_i, int from_j, int to_i, int to_j){
    if( from_i == to_i ){ // move horizontally
        if( from_j < to_j ){ // move right
            for( int k = 1; k < 8; k++ ){
                if( to_j == from_j + k ) {
                    return 1;
                }
                else {
                    if( b.board[to_i][from_j + k].type != '.' ) return 0;
                }
            }
        }
        else{ // move left
            for( int k = 1; k < 8; k++ ){
                if( to_j == from_j - k ) {
                    return 1;
                }
                else {
                    if( b.board[to_i][from_j - k].type != '.' ) return 0;
                }
            }
        }
    }
    else if( from_j == to_j ){
        if( from_i < to_i ){ // move downwards
            for( int k = 1; k < 8; k++ ){
                if( to_i == from_i + k ) {
                    return 1;
                }
                else {
                    if( b.board[from_i + k][to_j].type != '.' ) return 0;
                }
            }
        }
        else{ // move upwards
            for( int k = 1; k < 8; k++ ){
                if( to_i == from_i - k ) {
                    return 1;
                }
                else {
                    if( b.board[from_i - k][to_j].type != '.' ) return 0;
                }
            }
        }
    }
    return 0;
}

int is_valid_knight_move(int from_i, int from_j, int to_i, int to_j){
    if(    ((to_i == from_i - 1) && ((to_j == from_j - 2) || (to_j == from_j + 2)))
        || ((to_i == from_i - 2) && ((to_j == from_j - 1) || (to_j == from_j + 1)))
        || ((to_i == from_i + 2) && ((to_j == from_j - 1) || (to_j == from_j + 1)))
        || ((to_i == from_i + 1) && ((to_j == from_j - 2) || (to_j == from_j + 2)))
    ){
        return 1;
    }
    return 0;
}

int is_valid_bishop_move(board_t b, int from_i, int from_j, int to_i, int to_j){
    if( to_i < from_i && to_j < from_j ){
        for(int k = 1; k < 8; k++){
            if( (to_i == from_i - k) && (to_j == from_j - k) ) {
                return 1;
            }
            else {
                if( b.board[from_i - k][from_j - k].type != '.' ) return 0;
            }
        }
    }
    else if( to_i > from_i && to_j < from_j ){
        for(int k = 1; k < 8; k++){
            if( (to_i == from_i + k) && (to_j == from_j - k) ) {
                return 1;
            }
            else {
                if( b.board[from_i + k][from_j - k].type != '.' ) return 0;
            }
        }
    }
    else if( to_i > from_i && to_j > from_j ){
        for(int k = 1; k < 8; k++){
            if( (to_i == from_i + k) && (to_j == from_j + k) ) {
                return 1;
            }
            else {
                if( b.board[from_i + k][from_j + k].type != '.' ) return 0;
            }
        }
    }
    else if( to_i < from_i && to_j > from_j ){
        for(int k = 1; k < 8; k++){
            if( (to_i == from_i - k) && (to_j == from_j + k) ) {
                return 1;
            }
            else {
                if( b.board[from_i - k][from_j + k].type != '.' ) return 0;
            }
        }
    }
    return 0;
}

int is_valid_king_move(board_t b, int from_i, int from_j, int to_i, int to_j){
    if( ( to_i == from_i || to_i == from_i + 1 || to_i == from_i - 1 ) 
        && ( to_j == from_j || to_j == from_j + 1 || to_j == from_j - 1 )
    ){
        return 1;
    }
    return 0;
}

board_t validate_move(board_t b, player_t player, player_t white, player_t black, char from_col, int from_row, char to_col, int to_row){

    int from_i = (from_row - BOARD_SIZE) * -1;
    int from_j = char_to_index(from_col);
    int to_i   = (to_row - BOARD_SIZE) * -1;
    int to_j   = char_to_index(to_col);

    if(player.color != b.board[from_i][from_j].color){
        printf("invalid move: cannot move adversary's piece\n");
        move_succesfull = 0;
        return b;
    }

    if( (from_i < 0 || from_i > 7) ||
        (from_j < 0 || from_j > 7) ||
        (to_i < 0   || to_i > 7)   ||
        (to_j < 0   || to_j > 7)
    ){
        printf("invalid move: out of bounds\n");
        move_succesfull = 0;
        return b;
    }
    if( from_i == to_i && from_j == to_j ){
        printf("invalid move: not a move\n");
        move_succesfull = 0;
        return b;
    }

    if(b.board[from_i][from_j].type == '.' || b.board[from_i][from_j].color == -1){
        print_piece(b.board[from_i][from_j]);
        printf("invalid move\n");
        move_succesfull = 0;
        return b;
    }
    else if(b.board[from_i][from_j].type == 'p'){
        if( b.board[from_i][from_j].color == 0 ){ // white pawn
            if( from_j == to_j ){ // moving forward
                if( b.board[to_i][to_j].type == '.' ){ // can only move if square is empty
                    if( (from_i - to_i == 1)
                    || ((from_i - to_i == 2) && (from_i == 6)) ){
                        if( to_i == 0 ){
                            move_succesfull = 1;
                            return promote(b, white, black, 0, from_col, from_row, to_col, to_row);
                        }
                        move_succesfull = 1;
                        return make_move(b, white, black, from_col, from_row, to_col, to_row);
                    }
                }
            }
            else if( ((to_j == from_j - 1) || (to_j == from_j + 1)) && (from_i - to_i == 1) ){ //taking a piece
                if( b.board[to_i][to_j].color == 1 ){
                    move_succesfull = 1;
                    return take_piece(b, white, black, from_col, from_row, to_col, to_row);
                }
            }
        }
        else if( b.board[from_i][from_j].color == 1 ){ //black pawn
            if( from_j == to_j ){ // moving forward
                if( b.board[to_i][to_j].type == '.' ){ // can only move if square is empty
                    if( (from_i - to_i == -1)
                    || ((from_i - to_i == -2) && from_i == 1) ){
                        if( to_i == 7 ){
                            move_succesfull = 1;
                            return promote(b, white, black, 1, from_col, from_row, to_col, to_row);
                        }
                        move_succesfull = 1;
                        return make_move(b, white, black, from_col, from_row, to_col, to_row);
                    }
                }
            }
            else if( ((to_j == from_j - 1) || (to_j == from_j + 1)) && (to_i - from_i == 1) ){ //taking a piece
                if( b.board[to_i][to_j].color == 0 ){
                    move_succesfull = 1;
                    return take_piece(b, white, black, from_col, from_row, to_col, to_row);       
                }
            }
        }
        printf("invalid move\n");
        move_succesfull = 0;
    }
    else if(b.board[from_i][from_j].type == 'r'){
        if( is_valid_rook_move(b, from_i, from_j, to_i, to_j) ){
            if( b.board[to_i][to_j].type == '.' ){
                move_succesfull = 1;
                return make_move(b, white, black, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, white, black, from_col, from_row, to_col, to_row);
            }
        }
        else {
            move_succesfull = 0;
            return b;
        }
    }
    else if(b.board[from_i][from_j].type == 'n'){
        if( is_valid_knight_move(from_i, from_j, to_i, to_j) ){
            if( b.board[to_i][to_j].type == '.' ){
                move_succesfull = 1;
                return make_move(b, white, black, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, white, black, from_col, from_row, to_col, to_row);
            }
        }
        else{
            move_succesfull = 0;
            return b;
        }
    }
    else if(b.board[from_i][from_j].type == 'b'){
        if( is_valid_bishop_move(b, from_i, from_j, to_i, to_j) ){
            if( b.board[to_i][to_j].type == '.' ){
                move_succesfull = 1;
                return make_move(b, white, black, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, white, black, from_col, from_row, to_col, to_row);
            }
        }
        else{
            move_succesfull = 0;
            return b;
        }
    }
    else if(b.board[from_i][from_j].type == 'q'){
        if( 
            is_valid_bishop_move(b, from_i, from_j, to_i, to_j)
            || is_valid_rook_move(b, from_i, from_j, to_i, to_j)
        ){
            if( b.board[to_i][to_j].type == '.' ){
                move_succesfull = 1;
                return make_move(b, white, black, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, white, black, from_col, from_row, to_col, to_row);
            }
        }
        else{
            move_succesfull = 0;
            return b;
        }
    }
    else if(b.board[from_i][from_j].type == 'k'){
        if( is_valid_king_move(b, from_i, from_j, to_i, to_j) ){
            if( player.can_castle_long == 1 ) player.can_castle_long = 0;
            if( player.can_castle_short == 1 ) player.can_castle_short = 0;
            if( b.board[to_i][to_j].type == '.' ){
                move_succesfull = 1;
                return make_move(b, white, black, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, white, black, from_col, from_row, to_col, to_row);
            }
        } 
        else{
            move_succesfull = 0;
            return b;
        }
    }
    else{
        return b;
    }
    move_succesfull = 0;
    return b;
}

int check_for_checkmate(board_t b, player_t white, player_t black){
    return 0;
}

