#include <ncurses.h>
#include <unistd.h>
#include "screen.h"

int display[SCREEN_ROWS * SCREEN_COLS];
int changed[SCREEN_ROWS * SCREEN_COLS];

// Init Screen Function
void initScreen(){

    // Set all pixels to "off"
    for(int i = 0; i < SCREEN_ROWS * SCREEN_COLS; i++){
        display[i] = 0;
        changed[i] = 1;
    }

    //init ncurses screen
    initscr();
    cbreak();
    noecho();
    curs_set(0);
}

// Goes through our display and prints if pixel is set
void printScreen() {
    for(int y = 0; y < SCREEN_ROWS; y++) {
        for(int x = 0; x < SCREEN_COLS; x++) {
            int index = x + (y * SCREEN_COLS);
            if (changed[index]) {
                move(y, x);
                if(display[index] == 1){
                    addch(ACS_BOARD);
                } else {
                    addch(' ');
                }
                changed[index] = 0;
            }
        }
    }
    refresh();
}

// Clears the Screen
void clearScreen() {
    for (int i = 0; i < SCREEN_ROWS * SCREEN_COLS; i++) {
        display[i] = 0;
        changed[i] = 1;
    }
    clear();
    refresh();
}


// Functiont to set pixel on display
// returns true if pixel is turned on
int setPixel(struct CHIP8State *cpu, int x, int y){

    if (x >= SCREEN_COLS) x -= SCREEN_COLS;
    else if (x < 0) x += SCREEN_COLS;

    if (y >= SCREEN_ROWS) y -= SCREEN_ROWS;
    else if (y < 0) y += SCREEN_ROWS;

    int index = x + (y * SCREEN_COLS);
    display[index] ^= 1;
    changed[index] = 1;
    cpu->screenNeedsRedraw = 1;

    return display[index] != 1;
}
