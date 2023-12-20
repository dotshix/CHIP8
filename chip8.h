#ifndef CHIP8_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define CHIP8_H_

#define MEMORY_SIZE 4096
#define REGISTER_SIZE 16
#define STACK_SIZE 16
#define ROM_START_ADDRESS 0x200
#define FRAME_RATE 60
#define FRAME_TIME_USEC (1000000 / FRAME_RATE)

struct CHIP8State {
    // Used to store memory
    uint8_t memory[MEMORY_SIZE];

    // Used to store registers
    uint8_t V[REGISTER_SIZE];

    // index register
    uint16_t I;

    // program counter register
    // starts at position 0x200
    uint16_t pc;

    // program stack pointer
    uint8_t sp;

    // pseudo stack using array
    uint16_t stack[STACK_SIZE];

    // delay timer
    uint8_t delayTimer;

    // sound timer
    uint8_t soundTimer;

    uint16_t paused;

    uint16_t cpuSpeed;

   int8_t screenNeedsRedraw;
};

typedef void (*Chip8OpCode)(struct CHIP8State *, uint16_t);

// OPCODE instructions
void op_00E0(struct CHIP8State *cpu, uint16_t op);        // CLS - Clear Display
void op_00EE(struct CHIP8State *cpu, uint16_t op);        // RET - Return from Subroutine

void op_1nnn(struct CHIP8State *cpu, uint16_t op);        // JP addr - Jump to location nnn.
void op_2nnn(struct CHIP8State *cpu, uint16_t op);        // CALL addr - Call subroutine at nnn
void op_3xkk(struct CHIP8State *cpu, uint16_t op);        // SE Vx, byte - Skip next instruction if Vx = kk.
void op_4xkk(struct CHIP8State *cpu, uint16_t op);        // SNE Vx, byte - Skip next instruction if Vx != kk.
void op_5xy0(struct CHIP8State *cpu, uint16_t op);        // SE Vx, Vy - Skip next instruction if Vx = Vy.
void op_6xkk(struct CHIP8State *cpu, uint16_t op);        // LD Vx, byte - Set Vx = kk.
void op_7xkk(struct CHIP8State *cpu, uint16_t op);        // ADD Vx, byte - Set Vx = Vx + kk.

void op_8xy0(struct CHIP8State *cpu, uint16_t op);        // LD Vx, Vy - Set Vx = Vy.
void op_8xy1(struct CHIP8State *cpu, uint16_t op);        // OR Vx, Vy - Set Vx = Vx OR Vy.
void op_8xy2(struct CHIP8State *cpu, uint16_t op);        // AND Vx, Vy - Set Vx = Vx AND Vy.
void op_8xy3(struct CHIP8State *cpu, uint16_t op);        // XOR Vx, Vy - Set Vx = Vx XOR Vy
void op_8xy4(struct CHIP8State *cpu, uint16_t op);        // ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry.
void op_8xy5(struct CHIP8State *cpu, uint16_t op);        // SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow.
void op_8xy6(struct CHIP8State *cpu, uint16_t op);        // SHR Vx {, Vy} - Set Vx = Vx SHR 1.
void op_8xy7(struct CHIP8State *cpu, uint16_t op);        // SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow.
void op_8xyE(struct CHIP8State *cpu, uint16_t op);        // SHL Vx {, Vy} - Set Vx = Vx SHL 1.

void op_9xy0(struct CHIP8State *cpu, uint16_t op);        // SNE Vx, Vy - Skip next instruction if Vx != Vy.
void op_Annn(struct CHIP8State *cpu, uint16_t op);        // LD I, addr - Set I = nnn.
void op_Bnnn(struct CHIP8State *cpu, uint16_t op);        // JP V0, addr - Jump to location nnn + V0.
void op_Cxkk(struct CHIP8State *cpu, uint16_t op);        // RND Vx, byte - Set Vx = random byte AND kk.
void op_Dxyn(struct CHIP8State *cpu, uint16_t op);        // DRW Vx, Vy, nibble - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

/* TODO NEED TO IMPLEMENT OP CODE
 * NEED TO IMPLEMENT KEYBOARD FOR THESE
 * Ex9E, ExA1, */

void op_Fx07(struct CHIP8State *cpu, uint16_t op);        // LD Vx, DT - Set Vx = delay timer value.

// TODO Fx0A

void op_Fx15(struct CHIP8State *cpu, uint16_t op);        // LD DT, Vx - Set delay timer = Vx.
void op_Fx18(struct CHIP8State *cpu, uint16_t op);        // LD ST, Vx - Set sound timer = Vx.
void op_Fx1E(struct CHIP8State *cpu, uint16_t op);        // ADD I, Vx - Set I = I + Vx.
void op_Fx29(struct CHIP8State *cpu, uint16_t op);        // LD F, Vx - Set I = location of sprite for digit Vx.
void op_Fx33(struct CHIP8State *cpu, uint16_t op);        // LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, and I+2.
void op_Fx55(struct CHIP8State *cpu, uint16_t op);        // LD [I], Vx - Store registers V0 through Vx in memory starting at location I.
void op_Fx65(struct CHIP8State *cpu, uint16_t op);        // LD Vx, [I] -Read registers V0 through Vx from memory starting at location I.

void initCHIP8(struct CHIP8State *cpu);                   // funciton to initialize CPU
void loadSprites(struct CHIP8State *cpu);                 // load sprites into memory
int loadRom(struct CHIP8State *cpu, const char *filename);// load  rom into memory
void interpret_instruction(struct CHIP8State *cpu);
void cycle(struct CHIP8State *cpu);
void updateTimer(struct CHIP8State *cpu);
uint8_t wrapping_add(uint8_t a, uint8_t b);

void opfunc_00(struct CHIP8State *cpu, uint16_t op);
void opfunc_8(struct CHIP8State *cpu, uint16_t op);
void cpu_NULL(struct CHIP8State *cpu, uint16_t op);
unsigned long getMicrosecondTimestamp();

#endif // CHIP8_H_
