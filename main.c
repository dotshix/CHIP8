#include "screen.h"
#include "chip8.h"
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <rom file>\n", argv[0]);
        return 1;
    }

    struct CHIP8State cpu;
    initCHIP8(&cpu);

    // Open the ROM file
    if(loadRom(&cpu, argv[1])){
        return 1;
    }
    setlocale(LC_ALL, "");
    srand(time(NULL)); // set a seed based on the current time
    loadSprites(&cpu);
    initScreen();
    /* for(int i = 0; i < SCREEN_COLS; i++){ */
    /*     for(int x = 0; x < SCREEN_ROWS; x++) */
    /*         setPixel(i, x); */
    /* } */

    /* printScreen(); */

    cycle(&cpu);

    getch();
    endwin();
    return 0;
}
