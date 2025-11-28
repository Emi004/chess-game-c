#ifndef CHESS_LOGIC_H
#define CHESS_LOGIC_H

#define BOARD_SIZE 8

#define BLUE "\x1b[34m"
#define RESET "\x1b[0m"

extern int game_running;
extern int move_succesfull;
extern int turn;

typedef struct {
    int color;
    char type;
    int row;
    int col;
} piece_t;

typedef struct {
    piece_t board[BOARD_SIZE][BOARD_SIZE];
}board_t;

typedef struct {
    int can_castle_short; // O-O
    int can_castle_long; // O-O-O
    int color;
    int is_in_check;
}player_t;


piece_t get_piece(int color, char type, int row, int col);
void print_piece(piece_t p);

board_t init_board(board_t b); // returns a board_t as it is at the beginning of the game
void print_board(board_t b); 
int char_to_index(char c); // takes a chess board column(a-h) and returns a matrix index(0-7)
char index_to_char(int index); // takes a chess board column index and returns a chess board column(a-h)
int is_in_bounds(int i, int j);

int does_rook_see_king(board_t b, int from_i, int from_j, char direction); // helper functions for check_for_check function
int does_bishop_see_king(board_t b, int from_i, int from_j, char direction);

int check_for_check(board_t b, int player_to_be_checked, player_t white, player_t black); // check if the player with color player_to_be_checked is in check, 
                                                                                          // does NOT set player's is_in_check attribute

board_t make_move(board_t b, player_t white, player_t black, char from_col, int from_row, char to_col, int to_row); // moves a piece to an empty square, replaces the piece's starting square with an empty square
board_t take_piece(board_t b, player_t white, player_t black, char from_col, int from_row, char to_col, int to_row); // capture a piece, replaces the capturing piece's starting square with an empty square
board_t promote(board_t b, player_t white, player_t black, int color, char from_col, int from_row, char to_col, int to_row); // promotes a pawn to a queen

/*all the move functions will effectively perform the move, then check if the move puts a player into check:
    if the player who makes the move would be put into check by his own move, the board reverts to its original positon, move_succesfull is set to 0 and an error message is displayed;
    otherwise, if the player who makes the move puts their opponent into check, the opponent's is_in_check is set and a warning is displayed for the opponent
*/

int is_valid_rook_move(board_t b, int from_i, int from_j, int to_i, int to_j); // helper functions for validate_move:
int is_valid_knight_move(int from_i, int from_j, int to_i, int to_j);          // check if the move is valid index-wise 
int is_valid_bishop_move(board_t b, int from_i, int from_j, int to_i, int to_j);
int is_valid_king_move(board_t b, int from_i, int from_j, int to_i, int to_j);

board_t validate_move(board_t b, player_t player, player_t white, player_t black, char from_col, int from_row, char to_col, int to_row); // the first player_t argument named player is the acting player, the function takes a move in format "e2 e4", turns the move to matrix indexes, identifies what kind of piece is at that position and whether or not the player is able to make the move; sets move_succesfull and returns a move making function or the initial board state in case move_succesful is set to 0

int check_for_checkmate(board_t b, player_t white, player_t black); // not yet implemented

#endif