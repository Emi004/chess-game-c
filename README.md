# chess-game-c
chess game c with tcp socket, ncurses for ui unfortunately

map pieces(secret code), non yandere dev version
```c
wchar_t map_to_unicode(char piece, int color){
    int offset=0;
    if (color) offset+=BLK_OFFSET;
   
    switch(piece){
        case 'p': return 0x2659+offset; break;
        case 'k': return 0x2654+offset; break;
        case 'n': return 0x2658+offset; break;
        case 'b': return 0x2657+offset; break;
        case 'q': return 0x2655+offset; break;
        case 'r': return 0x2656+offset; break;
    }
      
}

```
