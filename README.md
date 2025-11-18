# chess-game-c
chess game c with tcp socket, ncurses for ui unfortunately

map pieces(secret code)
```c
wchar_t map_to_unicode(char piece, int color){
    switch(color){
        //alb
        case 0:
            switch(piece){
                case 'p': return 0x2659; break;
                case 'k': return 0x2654; break;
                case 'n': return 0x2658; break;
                case 'b': return 0x2657; break;
                case 'q': return 0x2655; break;
                case 'r': return 0x2656; break;
            }
        //negru
        case 1:
            switch(piece){
                case 'p': return 0x2659+BLK_OFFSET; break;
                case 'k': return 0x2654+BLK_OFFSET; break;
                case 'n': return 0x2658+BLK_OFFSET; break;
                case 'b': return 0x2657+BLK_OFFSET; break;
                case 'q': return 0x2655+BLK_OFFSET; break;
                case 'r': return 0x2656+BLK_OFFSET; break;
            }
    }
}
```
