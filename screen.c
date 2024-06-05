#include <ncurses.h>
#include <unistd.h>
#include "screen.h"

int display[SCREEN_ROWS * SCREEN_COLS];

// Init Screen Function
void initScreen(){

    // Set all pixels to "off"
    for(int i = 0; i < SCREEN_ROWS * SCREEN_COLS; i++){
        display[i] = 0;
    }

    //init ncurses screen
    initscr();
}

// Goes through our display and prints if pixel is set
void printScreen(){
    for(int x = 0; x < SCREEN_COLS; x++){
        for(int y = 0; y < SCREEN_ROWS; y++){
            if(display[x + (y * SCREEN_COLS)] == 1){
                /* mvprintw(y, x, "x"); */
                mvaddch(y, x, ACS_BOARD);

            }
        }
    }

    refresh();
    usleep(1000);
}

// Clears the Screen
void clearScreen(){
    // Set all pixels to "off"
    for(int i = 0; i < SCREEN_ROWS * SCREEN_COLS; i++){
        display[i] = 0;
    }

    initscr();
}

// Function to set pixel on display
// returns true if a collision occurs (i.e., if the pixel was already on)
int setPixel(int x, int y) {
    // If needed, wrap around
    if (x >= SCREEN_COLS) x -= SCREEN_COLS;
    else if (x < 0) x += SCREEN_COLS;

    if (y >= SCREEN_ROWS) y -= SCREEN_ROWS;
    else if (y < 0) y += SCREEN_ROWS;

    int index = x + (y * SCREEN_COLS);
    int collision = display[index] == 1;
    display[index] ^= 1;

    return collision;
}
