#include "chip8.h"

#ifndef SCREEN_H
#define SCREEN_H

// Screen size
#define SCREEN_COLS 64
#define SCREEN_ROWS 32

void initScreen();

void printScreen();

void clearScreen();

int setPixel(struct CHIP8State *cpu, int x, int y);

#endif // SCREEN_H
