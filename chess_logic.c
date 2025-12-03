#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "chess_logic.h"

int game_running = 1;
int move_succesfull = 1;
int turn = 0;
square_t checking_piece = {-1,-1};
char checking_direction;

square_t second_checking_piece = {-1, -1};
char second_checking_direction;

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

board_t init_board(board_t b, player_t white, player_t black){
    b.white = white;
    b.black = black;
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

char index_to_char(int index) {
    if (index >= 0 && index <= 7) { 
        return 'a' + index;
    }
    return '!';  
}

int is_in_bounds(int i, int j){
    return ( i >= 0 && i < 8 && j >= 0 && j < 8);
}

int does_rook_see_king(board_t b, int from_i, int from_j, char direction){
    for(int i = 1; i < BOARD_SIZE; i++){
        int step_i, step_j;
        switch(direction){
            case 'a':
                step_i = from_i;
                step_j = from_j - i; 
                break;
            case 'd':
                step_i = from_i;
                step_j = from_j + i; 
                break;
            case 'w':
                step_i = from_i - i;
                step_j = from_j; 
                break;
            case 's':
                step_i = from_i + i;
                step_j = from_j; 
                break;
            default:
                break;
        }
        if( is_in_bounds(step_i, step_j) ){
            if(b.board[from_i][from_j].color != b.board[step_i][step_j].color && b.board[step_i][step_j].type == 'k'){
                checking_direction = direction;
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
            case 'L':
                step_i = from_i - i;
                step_j = from_j - i; 
                break;
            case 'R':
                step_i = from_i - i;
                step_j = from_j + i; 
                break;
            case 'l':
                step_i = from_i + i;
                step_j = from_j - i; 
                break;
            case 'r':
                step_i = from_i + i;
                step_j = from_j + i; 
                break;
            default:
                break;
        }
        if( is_in_bounds(step_i, step_j) ){
            if(b.board[from_i][from_j].color != b.board[step_i][step_j].color && b.board[step_i][step_j].type == 'k'){
                checking_direction = direction;
                return 1;
            }
            if(b.board[step_i][step_j].type != '.'){
                return 0;
            }
        }
    }
    return 0;
}

int check_for_check(board_t b, int player_to_be_checked){
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
                                
                                checking_piece.row = i;
                                checking_piece.col = j;
                                return 1;
                             }
                        }
                        if( is_in_bounds(i-1, j+1) ){
                             if(b.board[i-1][j+1].color == 1 && b.board[i-1][j+1].type == 'k'){
                                
                                checking_piece.row = i;
                                checking_piece.col = j;
                                return 1;
                             }
                        }
                    }
                    else{ // black
                        if( is_in_bounds(i+1, j-1) ){
                             if(b.board[i+1][j-1].color == 0 && b.board[i+1][j-1].type == 'k'){
                                
                                checking_piece.row = i;
                                checking_piece.col = j;
                                return 1;
                             }
                        }
                        if( is_in_bounds(i+1, j+1) ){
                             if(b.board[i+1][j+1].color == 0 && b.board[i+1][j+1].type == 'k'){
                                
                                checking_piece.row = i;
                                checking_piece.col = j;
                                return 1;
                            }
                        }
                    }
                }
                else if( b.board[i][j].type == 'r' ){
                    // upwards check
                    if( does_rook_see_king(b, i, j, 'w') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'w';
                        return 1;
                    }
                    // downwards check
                    if( does_rook_see_king(b, i, j, 's') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 's';
                        return 1;
                    }
                    // left check
                    if( does_rook_see_king(b, i, j, 'a') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'a';
                        return 1;
                    }
                    // right check
                    if( does_rook_see_king(b, i, j, 'd') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'd';
                        return 1;
                    }
                }
                else if( b.board[i][j].type == 'n' ){
                    for(int step_i = 0; step_i < 5; (step_i == 1 ? step_i += 2 : step_i++)){
                        if( (step_i - 2) % 2 == 0 ){
                            for(int step_j = 0; step_j <= 2; step_j += 2){
                                if( is_in_bounds(i + (step_i - 2), j + (step_j - 1)) ){
                                    if( b.board[i + step_i - 2][j + step_j - 1].color != player_color && b.board[i + step_i - 2][j + step_j - 1].type == 'k' ){
                                        checking_piece.row = i;
                                        checking_piece.col = j;
                                        return 1;
                                    }
                                }
                            }
                        }
                        else{
                            for(int step_j = 0; step_j <= 4; step_j += 4){
                                if( is_in_bounds(i + (step_i - 2), j + (step_j - 2)) ){
                                    if( b.board[i + step_i - 2][j + step_j - 2].color != player_color && b.board[i + step_i - 2][j + step_j - 2].type == 'k' ){
                                        checking_piece.row = i;
                                        checking_piece.col = j;
                                        return 1;
                                    }
                                }
                            }
                        }
                    }
                }
                else if( b.board[i][j].type == 'b' ){

                    if( does_bishop_see_king(b, i, j, 'L') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'L';
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'l') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'l';
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'R') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'R';
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'r') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'r';
                        return 1;
                    }
                }
                else if( b.board[i][j].type == 'q' ){

                    if( does_bishop_see_king(b, i, j, 'L') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'L';
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'R') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'R';
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'l') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'l';
                        return 1;
                    }
                    if( does_bishop_see_king(b, i, j, 'r') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'r';
                        return 1;
                    }
                    // upwards check
                    if( does_rook_see_king(b, i, j, 'w') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'w';
                        return 1;
                    }
                    // downwards check
                    if( does_rook_see_king(b, i, j, 's') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 's';
                        return 1;
                    }
                    // left check
                    if( does_rook_see_king(b, i, j, 'a') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'a';
                        return 1;
                    }
                    // right check
                    if( does_rook_see_king(b, i, j, 'd') ){
                        checking_piece.row = i;
                        checking_piece.col = j;
                        checking_direction = 'd';
                        return 1;
                    }
                }
            }
        }
    }
    return 0;
}

board_t make_move(board_t b, char from_col, int from_row, char to_col, int to_row){

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
        if( check_for_check(b, 0) ){
            DBG printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(-1, '.', to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, ptype, from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            b.white.is_in_check = 0;
        }
        if( check_for_check(b, 1) ){
            DBG printf("black in check.\n");
            b.black.is_in_check = 1;

            DBG printf("DEBUG: checking piece at %d,%d dir=%c\n",
            checking_piece.row, checking_piece.col, checking_direction);

            check_for_checkmate(b, 1 - color);

            return b;
        }
    }
    if(color == 1){
        if( check_for_check(b, 1) ){
            DBG printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(-1, '.', to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, ptype, from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else{
            b.black.is_in_check = 0;
        }
        if( check_for_check(b, 0) ){
            DBG printf("white in check.\n");
            b.white.is_in_check = 1;

            DBG printf("DEBUG: checking piece at %d,%d dir=%c\n",
            checking_piece.row, checking_piece.col, checking_direction);

            check_for_checkmate(b, 1 - color);

            return b;
        }
    }

    return b;
}

board_t take_piece(board_t b, char from_col, int from_row, char to_col, int to_row){

    int from_i = (from_row - BOARD_SIZE) * -1;
    int from_j = char_to_index(from_col);
    int to_i   = (to_row - BOARD_SIZE) * -1;
    int to_j   = char_to_index(to_col);


    int color = b.board[from_i][from_j].color;
    char aptype = b.board[from_i][from_j].type;
    char bptype = b.board[to_i][to_j].type;


    if( b.board[from_i][from_j].type == 'p' ){
        if( color == 0 ){
            if( to_i == 0 ){
                return promote(b, color, from_col, from_row, to_col, to_row);
            }
        }
        else{
            if( to_i == 7 ){
                return promote(b, color, from_col, from_row, to_col, to_row);
            }
        }
    }


    b.board[to_i][to_j] = b.board[from_i][from_j];
    b.board[from_i][from_j] = get_piece(-1, '.', from_i, from_j);

    
    if(color == 0){
        if( check_for_check(b, 0) ){
            DBG printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(1 - color, bptype, to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, aptype, from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            b.white.is_in_check = 0;
        }
        if(  check_for_check(b, 1) ){
            DBG printf("black in check.\n");
            b.black.is_in_check = 1;

            check_for_checkmate(b, 1 - color);

            return b;
        }
    }
    if(color == 1){
        if( check_for_check(b, 1) ){
            DBG printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(1 - color, bptype, to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, aptype, from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            b.black.is_in_check = 0;
        }
        if( check_for_check(b, 0) ){
            DBG printf("white in check.\n");
            b.white.is_in_check = 1;
 
            check_for_checkmate(b, 1 - color);

            return b;
        }
    }


    return b;
}

board_t promote(board_t b, int color, char from_col, int from_row, char to_col, int to_row){

    int from_i = (from_row - BOARD_SIZE) * -1;
    int from_j = char_to_index(from_col);
    int to_i   = (to_row - BOARD_SIZE) * -1;
    int to_j   = char_to_index(to_col);

    b.board[to_i][to_j] = get_piece(color, 'q', to_i, to_j);
    b.board[from_i][from_j] = get_piece(-1, '.', from_i, from_j);

    if(color == 0){
        if( check_for_check(b, 0) ){
            DBG printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(-1, '.', to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, 'p', from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            b.white.is_in_check = 0;
        }
        if( check_for_check(b, 1) ){
            DBG printf("black in check.\n");
            b.black.is_in_check = 1;

            check_for_checkmate(b, 1 - color);

            return b;
        }
    }
    if(color == 1){
        if( check_for_check(b, 1) ){
            DBG printf("invalid move: this move would put you in check.\n");
            b.board[to_i][to_j] = get_piece(-1, '.', to_i, to_j);
            b.board[from_i][from_j] = get_piece(color, 'p', from_i, from_j);
            move_succesfull = 0;
            return b;
        }
        else {
            b.black.is_in_check = 0;
        }
        if( check_for_check(b, 0) ){
            DBG printf("white in check.\n");
            b.white.is_in_check = 1;

            check_for_checkmate(b, 1 - color);

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

int is_valid_pawn_move(int color, int from_i, int from_j, int to_i, int to_j){
    if( color == 1 ){
        if(  ( (to_i == from_i + 1) && (to_j == from_j) )
        ) return 1;
    }
    else{
        if( ( (to_i == from_i - 1) && (to_j == from_j) )
        ) return 1;
    }
    return 0;
}

board_t validate_move(board_t b, player_t player, char from_col, int from_row, char to_col, int to_row){

    int from_i = (from_row - BOARD_SIZE) * -1;
    int from_j = char_to_index(from_col);
    int to_i   = (to_row - BOARD_SIZE) * -1;
    int to_j   = char_to_index(to_col);

    if(player.color != b.board[from_i][from_j].color){
        DBG printf("invalid move: cannot move adversary's piece\n");
        move_succesfull = 0;
        return b;
    }

    if( (from_i < 0 || from_i > 7) ||
        (from_j < 0 || from_j > 7) ||
        (to_i < 0   || to_i > 7)   ||
        (to_j < 0   || to_j > 7)
    ){
        DBG printf("invalid move: out of bounds\n");
        move_succesfull = 0;
        return b;
    }
    if( from_i == to_i && from_j == to_j ){
        DBG printf("invalid move: not a move\n");
        move_succesfull = 0;
        return b;
    }

    if(b.board[from_i][from_j].type == '.' || b.board[from_i][from_j].color == -1){
        print_piece(b.board[from_i][from_j]);
        DBG printf("invalid move\n");
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
                            return promote(b, 0, from_col, from_row, to_col, to_row);
                        }
                        move_succesfull = 1;
                        return make_move(b, from_col, from_row, to_col, to_row);
                    }
                }
            }
            else if( ((to_j == from_j - 1) || (to_j == from_j + 1)) && (from_i - to_i == 1) ){ //taking a piece
                if( b.board[to_i][to_j].color == 1 ){
                    move_succesfull = 1;
                    return take_piece(b, from_col, from_row, to_col, to_row);
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
                            return promote(b, 1, from_col, from_row, to_col, to_row);
                        }
                        move_succesfull = 1;
                        return make_move(b, from_col, from_row, to_col, to_row);
                    }
                }
            }
            else if( ((to_j == from_j - 1) || (to_j == from_j + 1)) && (to_i - from_i == 1) ){ //taking a piece
                if( b.board[to_i][to_j].color == 0 ){
                    move_succesfull = 1;
                    return take_piece(b, from_col, from_row, to_col, to_row);       
                }
            }
        }
        DBG printf("invalid move\n");
        move_succesfull = 0;
    }
    else if(b.board[from_i][from_j].type == 'r'){
        if( is_valid_rook_move(b, from_i, from_j, to_i, to_j) ){
            if( b.board[to_i][to_j].type == '.' ){
                move_succesfull = 1;
                return make_move(b, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, from_col, from_row, to_col, to_row);
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
                return make_move(b, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, from_col, from_row, to_col, to_row);
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
                return make_move(b, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, from_col, from_row, to_col, to_row);
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
                return make_move(b, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, from_col, from_row, to_col, to_row);
            }
        }
        else{
            move_succesfull = 0;
            return b;
        }
    }
    else if(b.board[from_i][from_j].type == 'k'){
        if( is_valid_king_move(b, from_i, from_j, to_i, to_j) ){
            if( player.color ){
                b.black.can_castle_long = 0;
                b.black.can_castle_short = 0;
            }
            else{
                b.white.can_castle_long = 0;
                b.white.can_castle_short = 0;
            }

            if( b.board[to_i][to_j].type == '.' ){
                move_succesfull = 1;
                return make_move(b, from_col, from_row, to_col, to_row);
            }
            else if( b.board[to_i][to_j].color != player.color ){
                move_succesfull = 1;
                return take_piece(b, from_col, from_row, to_col, to_row);
            }
        }
        else {
            if( from_col == 'e' && from_row == 1 && to_col == 'h' && to_row == 1 ){ // white short castle
                if( player.can_castle_short == 1 && player.is_in_check == 0 ){
                    if( b.board[7][5].type == '.' && b.board[7][6].type == '.' ){
                        if( !does_this_move_put_me_in_check(b, player.color, from_i, from_j, 7, 6) && !does_this_move_put_me_in_check(b, player.color, from_i, from_j, 7, 5) ){
                            b.board[7][4] = get_piece(-1, '.', 7, 4);
                            b.board[7][7] = get_piece(-1, '.', 7, 7);
                            b.board[7][5] = get_piece(player.color, 'r', 7, 5);
                            b.board[7][6] = get_piece(player.color, 'k', 7, 6);
                            move_succesfull = 1;
                            b.white.can_castle_short = 0;
                            b.white.can_castle_long = 0;
                            return b;
                        }
                        else{
                            DBG printf("invalid move: this move would put you in check/ passes through a check\n");
                            move_succesfull = 0;
                            return b;
                        }
                    }
                }
            }
            else if( from_col == 'e' && from_row == 1 && to_col == 'a' && to_row == 1 ){ // white long  castle
                if( player.can_castle_long == 1 && player.is_in_check == 0 ){
                    if( b.board[7][3].type == '.' && b.board[7][2].type == '.' && b.board[7][1].type == '.' ){
                        if( !does_this_move_put_me_in_check(b, player.color, from_i, from_j, 7, 2) && !does_this_move_put_me_in_check(b, player.color, from_i, from_j, 7, 3) ){
                            b.board[7][4] = get_piece(-1, '.', 7, 4);
                            b.board[7][0] = get_piece(-1, '.', 7, 0);
                            b.board[7][2] = get_piece(player.color, 'k', 7, 2);
                            b.board[7][3] = get_piece(player.color, 'r', 7, 3);
                            move_succesfull = 1;
                            b.white.can_castle_short = 0;
                            b.white.can_castle_long = 0;
                            return b;
                        }
                        else{
                            DBG printf("invalid move: this move would put you in check/ passes through a check\n");
                            move_succesfull = 0;
                            return b;
                        }
                    }
                }
            }
            else if( from_col == 'e' && from_row == 8 && to_col == 'h' && to_row == 8 ){ // black short castle
                if( player.can_castle_short == 1 && player.is_in_check == 0 ){
                    if( b.board[0][5].type == '.' && b.board[0][6].type == '.' ){
                        if( !does_this_move_put_me_in_check(b, player.color, from_i, from_j, 0, 6) && !does_this_move_put_me_in_check(b, player.color, from_i, from_j, 0, 5) ){
                            b.board[0][4] = get_piece(-1, '.', 0, 4);
                            b.board[0][7] = get_piece(-1, '.', 0, 7);
                            b.board[0][5] = get_piece(player.color, 'r', 0, 5);
                            b.board[0][6] = get_piece(player.color, 'k', 0, 6);
                            move_succesfull = 1;
                            b.black.can_castle_short = 0;
                            b.black.can_castle_long = 0;
                            return b;
                        }
                        else{
                            DBG printf("invalid move: this move would put you in check/ passes through a check\n");
                            move_succesfull = 0;
                            return b;
                        }
                    }
                }
            }
            else if( from_col == 'e' && from_row == 8 && to_col == 'a' && to_row == 8 ){ // black long  castle
                if( player.can_castle_long == 1 && player.is_in_check == 0 ){
                    if( b.board[0][3].type == '.' && b.board[0][2].type == '.' && b.board[0][1].type == '.' ){
                        if( !does_this_move_put_me_in_check(b, player.color, from_i, from_j, 0, 2) && !does_this_move_put_me_in_check(b, player.color, from_i, from_j, 0, 3) ){
                            b.board[0][4] = get_piece(-1, '.', 0, 4);
                            b.board[0][0] = get_piece(-1, '.', 0, 0);
                            b.board[0][2] = get_piece(player.color, 'k', 7, 2);
                            b.board[0][3] = get_piece(player.color, 'r', 7, 3);
                            move_succesfull = 1;
                            b.black.can_castle_short = 0;
                            b.black.can_castle_long = 0;
                            return b;
                        }
                        else{
                            DBG printf("invalid move: this move would put you in check/ passes through a check\n");
                            move_succesfull = 0;
                            return b;
                        }
                    }
                }
            }
            else{
                move_succesfull = 0;
                return b;
            }
        } 
    }
    else{
        return b;
    }
    move_succesfull = 0;
    return b;
}

square_t get_king_position(board_t b, int player_color){
    for(int i = 0; i < BOARD_SIZE; i++){
        for(int j = 0; j < BOARD_SIZE; j++){
            if( b.board[i][j].type == 'k' && b.board[i][j].color == player_color ){
                square_t s;
                s.col = j;
                s.row = i;
                return s;
            }
        }
    }
    square_t s;
    s.col = -1;
    s.row = -1;
    return s;
}

att_squares_t get_attacked_squares(board_t b){
    square_t* att_squares = NULL;
    int index = 0;
    switch(checking_direction){
        case 'w':
            for(int i = 1; i < BOARD_SIZE; i++){
                if( b.board[checking_piece.row - i][checking_piece.col].type == 'k' ) break;
                att_squares = (square_t*)realloc(att_squares, sizeof(square_t)*(index+1));
                att_squares[index] = (square_t){checking_piece.row - i, checking_piece.col};
                index++;
            }
            break;
        case 'a':
            for(int i = 1; i < BOARD_SIZE; i++){
                if( b.board[checking_piece.row][checking_piece.col - i].type == 'k' ) break;
                att_squares = (square_t*)realloc(att_squares, sizeof(square_t)*(index+1));
                att_squares[index] = (square_t){checking_piece.row, checking_piece.col - i};
                index++;
            }
            break;
        case 's':
            for(int i = 1; i < BOARD_SIZE; i++){
                if( b.board[checking_piece.row + i][checking_piece.col].type == 'k' ) break;
                att_squares = (square_t*)realloc(att_squares, sizeof(square_t)*(index+1));
                att_squares[index] = (square_t){checking_piece.row + i, checking_piece.col};
                index++;
            }
            break;
        case 'd':
            for(int i = 1; i < BOARD_SIZE; i++){
                if( b.board[checking_piece.row][checking_piece.col + i].type == 'k' ) break;
                att_squares = (square_t*)realloc(att_squares, sizeof(square_t)*(index+1));
                att_squares[index] = (square_t){checking_piece.row, checking_piece.col + i};
                index++;
            }
            break;
        case 'L':
            for(int i = 1; i < BOARD_SIZE; i++){
                if( b.board[checking_piece.row - i][checking_piece.col - i].type == 'k' ) break;
                att_squares = (square_t*)realloc(att_squares, sizeof(square_t)*(index+1));
                att_squares[index] = (square_t){checking_piece.row - i, checking_piece.col - i};
                index++;
            }
            break;
        case 'R':
            for(int i = 1; i < BOARD_SIZE; i++){
                if( b.board[checking_piece.row - i][checking_piece.col + i].type == 'k' ) break;
                att_squares = (square_t*)realloc(att_squares, sizeof(square_t)*(index+1));
                att_squares[index] = (square_t){checking_piece.row - i, checking_piece.col + i};
                index++;
            }
            break;
        case 'l':
            for(int i = 1; i < BOARD_SIZE; i++){
                if( b.board[checking_piece.row + i][checking_piece.col - i].type == 'k' ) break;
                att_squares = (square_t*)realloc(att_squares, sizeof(square_t)*(index+1));
                att_squares[index] = (square_t){checking_piece.row + i, checking_piece.col - i};
                index++;
            }
            break;
        case 'r':
            for(int i = 1; i < BOARD_SIZE; i++){
                if( b.board[checking_piece.row + i][checking_piece.col + i].type == 'k' ) break;
                att_squares = (square_t*)realloc(att_squares, sizeof(square_t)*(index+1));
                att_squares[index] = (square_t){checking_piece.row + i, checking_piece.col + i};
                index++;
            }
            break;
        default:
            break;
    }
    att_squares_t ret_value = {att_squares, index};
    return ret_value;
}

board_t copy_board(board_t b){
    board_t new_board;
    new_board.black = b.black;
    new_board.white = b.white;
    for(int i = 0; i < BOARD_SIZE; i++){
        for(int j = 0; j < BOARD_SIZE; j++){
            new_board.board[i][j] = b.board[i][j];
        }
    }
    return new_board;
}

int does_this_move_put_me_in_check(board_t b, int player_color, int from_i, int from_j, int to_i, int to_j){
    board_t aux = copy_board(b);

    aux.board[to_i][to_j] = get_piece(player_color, aux.board[from_i][from_j].type, to_i, to_j);
    aux.board[from_i][from_j] = get_piece(-1, '.', from_i, from_j);

    return check_for_check(aux, player_color);
}

int check_for_checkmate(board_t b, int player_color){

    int can_block = 0;
    int can_take = 0;
    int can_move = 0;

    square_t yes_king_moves[8] = { {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {0, -1}, {1, -1}, {1, 0}, {1, 1} };

    DBG printf("DEBUG: checking piece at %d,%d dir=%c\n",
       checking_piece.row, checking_piece.col, checking_direction);

    square_t king_square = get_king_position(b, player_color);
    DBG printf("DEBUG: king at %d,%d (player %d)\n", king_square.row, king_square.col, player_color);

    // int opp_color = 1 - player_color;
    // player_t players[2] = {white, black};

    //square_t king_square = get_king_position(b, player_color);
    if ( !(abs(king_square.row - checking_piece.row) <= 1 && abs(king_square.col - checking_piece.col) <= 1) ){
        att_squares_t att_sq = get_attacked_squares(b);
        if( att_sq.squares == NULL ){
            perror("sum went wrong with the fuckass checkmate function hold on");
            exit(-67);
        }
        
        for(int i = 0; i < att_sq.length; i++){
            printf("%d %d\n", att_sq.squares[i].row, att_sq.squares[i].col);
            for(int k = 0; k < BOARD_SIZE; k++){
                for(int h = 0; h < BOARD_SIZE; h++){
                    if( b.board[k][h].color == player_color ){
                        if( b.board[k][h].type == 'r' ){
                            if(is_valid_rook_move(b, k, h, att_sq.squares[i].row, att_sq.squares[i].col)){
                                if( !does_this_move_put_me_in_check(b, player_color, k, h, att_sq.squares[i].row, att_sq.squares[i].col) )
                                {
                                    DBG printf("can block with piece at %d %d\n", k, h);
                                can_block = 1;
                                } 
                            }
                        }
                        if( b.board[k][h].type == 'b' ){
                            if(is_valid_bishop_move(b, k, h, att_sq.squares[i].row, att_sq.squares[i].col)){
                                if( !does_this_move_put_me_in_check(b, player_color, k, h, att_sq.squares[i].row, att_sq.squares[i].col) ) {
                                    DBG printf("can block with piece at %d %d\n", k, h);
                                can_block = 1;
                                } 
                            }
                        }
                        if( b.board[k][h].type == 'q' ){
                            if(is_valid_bishop_move(b, k, h, att_sq.squares[i].row, att_sq.squares[i].col) || is_valid_rook_move(b, k, h, att_sq.squares[i].row, att_sq.squares[i].col) ){
                                
                                if( !does_this_move_put_me_in_check(b, player_color, k, h, att_sq.squares[i].row, att_sq.squares[i].col) ) {
                                    DBG printf("can block with piece at %d %d\n", k, h);
                                can_block = 1;
                                } 
                            }
                        }
                        if( b.board[k][h].type == 'n' ){
                            if(is_valid_knight_move(k, h, att_sq.squares[i].row, att_sq.squares[i].col)){
                                if( !does_this_move_put_me_in_check(b, player_color, k, h, att_sq.squares[i].row, att_sq.squares[i].col) ) {
                                    DBG printf("can block with piece at %d %d\n", k, h);
                                can_block = 1;
                                } 
                            }
                        }
                        if( b.board[k][h].type == 'p' ){
                            if(is_valid_pawn_move(player_color, k, h, att_sq.squares[i].row, att_sq.squares[i].col)){
                                if( !does_this_move_put_me_in_check(b, player_color, k, h, att_sq.squares[i].row, att_sq.squares[i].col) ) {
                                    DBG printf("can block with piece at %d %d\n", k, h);
                                can_block = 1;
                                } 
                            }
                        }
                    }
                }
            }
        }
        free(att_sq.squares);
    }
    else{
        can_block = 0;
    }
    for(int k = 0; k < BOARD_SIZE; k++){
        for(int h = 0; h < BOARD_SIZE; h++){
            if( b.board[k][h].color == player_color ){
                if( b.board[k][h].type == 'r' ){
                    if(is_valid_rook_move(b, k, h, checking_piece.row, checking_piece.col)){
                        if( !does_this_move_put_me_in_check(b, player_color, k, h, checking_piece.row, checking_piece.col) ) {
                            DBG printf("can take with piece at %d %d\n", k, h);
                            can_take = 1;
                        }
                    }
                }
                if( b.board[k][h].type == 'b' ){
                    if(is_valid_bishop_move(b, k, h, checking_piece.row, checking_piece.col)){
                        if( !does_this_move_put_me_in_check(b, player_color, k, h, checking_piece.row, checking_piece.col) ){
                            DBG printf("can take with piece at %d %d\n", k, h);
                            can_take = 1;
                        } 
                    }
                }
                if( b.board[k][h].type == 'q' ){
                    if(is_valid_bishop_move(b, k, h, checking_piece.row, checking_piece.col) || is_valid_rook_move(b, k, h, checking_piece.row, checking_piece.col) ){
                        if( !does_this_move_put_me_in_check(b, player_color, k, h, checking_piece.row, checking_piece.col) ) {
                            DBG printf("can take with piece at %d %d\n", k, h);
                            can_take = 1;
                        } 
                    }
                }
                if( b.board[k][h].type == 'n' ){
                    if(is_valid_knight_move(k, h, checking_piece.row, checking_piece.col)){
                        if( !does_this_move_put_me_in_check(b, player_color, k, h, checking_piece.row, checking_piece.col) ) {
                            DBG printf("can take with piece at %d %d\n", k, h);
                            can_take = 1;
                        } 
                    }
                }
                if( b.board[k][h].type == 'p' ){
                    if(is_valid_pawn_move(player_color, k, h, checking_piece.row, checking_piece.col)){
                        if( !does_this_move_put_me_in_check(b, player_color, k, h, checking_piece.row, checking_piece.col) ) {
                            DBG printf("can take with piece at %d %d\n", k, h);
                            can_take = 1;
                        } 
                    }
                }
                if( b.board[k][h].type == 'k' ){
                    if(is_valid_king_move(b, k, h, checking_piece.row, checking_piece.col)){
                        if( !does_this_move_put_me_in_check(b, player_color, k, h, checking_piece.row, checking_piece.col) ) {
                            DBG printf("can take with piece at %d %d\n", k, h);
                            can_take = 1;
                        } 
                    }
                }
            }
        }
    }
    for(int i = 0; i < 8; i++){
        if( is_in_bounds(king_square.row + yes_king_moves[i].row, king_square.col + yes_king_moves[i].col) ){
                if( !does_this_move_put_me_in_check(b, player_color, king_square.row, king_square.col, king_square.row + yes_king_moves[i].row, king_square.col + yes_king_moves[i].col) ){
                    if(b.board[king_square.row + yes_king_moves[i].row][king_square.col + yes_king_moves[i].col].type == '.' ){
                        DBG printf("%d %d\n", king_square.row + yes_king_moves[i].row, king_square.col + yes_king_moves[i].col);
                        can_move = 1;
                    }
            }
        }
    }
    if( !can_block && !can_take && !can_move ){
        game_running = 0;
        return 1;
    }
    DBG printf("game_running: %d, can_move: %d, can_take: %d, can_block: %d\n", game_running, can_move, can_take, can_block);
    return 0;
}

board_t attempt_castle(board_t b, int player_color){
    //short/long castle logic
    return b;
}