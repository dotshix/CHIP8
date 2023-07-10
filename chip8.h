#ifndef CHIP8_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define CHIP8_H_

#define MEMORY_SIZE 4096
#define REGISTER_SIZE 16
#define STACK_SIZE 16
#define ROM_START_ADDRESS 0x200

typedef void (*Chip8OpCode)(int16_t);

/* void interpret(uint16_t instruction); */

// OPCODE instructions
void op_00E0(int16_t instruction);                       // CLS - Clear Display
void op_00EE(int16_t instruction);                       // RET - Return from Subroutine

void op_1nnn(int16_t instruction);        // JP addr - Jump to location nnn.
void op_2nnn(int16_t instruction);        // CALL addr - Call subroutine at nnn
void op_3xkk(int16_t instruction);        // SE Vx, byte - Skip next instruction if Vx = kk.
void op_4xkk(int16_t instruction);        // SNE Vx, byte - Skip next instruction if Vx != kk.
void op_5xy0(int16_t instruction);        // SE Vx, Vy - Skip next instruction if Vx = Vy.
void op_6xkk(int16_t instruction);        // LD Vx, byte - Set Vx = kk.
void op_7xkk(int16_t instruction);        // ADD Vx, byte - Set Vx = Vx + kk.

void op_8xy0(int16_t instruction);        // LD Vx, Vy - Set Vx = Vy.
void op_8xy1(int16_t instruction);        // OR Vx, Vy - Set Vx = Vx OR Vy.
void op_8xy2(int16_t instruction);        // AND Vx, Vy - Set Vx = Vx AND Vy.
void op_8xy3(int16_t instruction);        // XOR Vx, Vy - Set Vx = Vx XOR Vy
void op_8xy4(int16_t instruction);        // ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry.
void op_8xy5(int16_t instruction);        // SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow.
void op_8xy6(int16_t instruction);        // SHR Vx {, Vy} - Set Vx = Vx SHR 1.
void op_8xy7(int16_t instruction);        // SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow.
void op_8xyE(int16_t instruction);        // SHL Vx {, Vy} - Set Vx = Vx SHL 1.

void op_9xy0(int16_t instruction);        // SNE Vx, Vy - Skip next instruction if Vx != Vy.
void op_Annn(int16_t instruction);        // LD I, addr - Set I = nnn.
void op_Bnnn(int16_t instruction);        // JP V0, addr - Jump to location nnn + V0.
void op_Cxkk(int16_t instruction);        // RND Vx, byte - Set Vx = random byte AND kk.
void op_Dxyn(int16_t instruction);        // DRW Vx, Vy, nibble - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.

/* TODO NEED TO IMPLEMENT OP CODE
 * NEED TO IMPLEMENT KEYBOARD FOR THESE
 * Ex9E, ExA1, */

void op_Fx07(int16_t instruction);        // LD Vx, DT - Set Vx = delay timer value.

// TODO Fx0A

void op_Fx15(int16_t instruction);        // LD DT, Vx - Set delay timer = Vx.
void op_Fx18(int16_t instruction);        // LD ST, Vx - Set sound timer = Vx.
void op_Fx1E(int16_t instruction);        // ADD I, Vx - Set I = I + Vx.
void op_Fx29(int16_t instruction);        // LD F, Vx - Set I = location of sprite for digit Vx.
void op_Fx33(int16_t instruction);        // LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, and I+2.
void op_Fx55(int16_t instruction);        // LD [I], Vx - Store registers V0 through Vx in memory starting at location I.
void op_Fx65(int16_t instruction);        // LD Vx, [I] -Read registers V0 through Vx from memory starting at location I.

void loadSprites(void);                   // load sprites into memory
int loadRom(const char *filename);        // load  rom into memory
void interpret_instruction(void);
void cycle(void);
void updateTimer(void);

void opfunc_00(int16_t instruction);
void opfunc_8(int16_t instruction);
void cpu_NULL(int16_t instruction);
#endif // CHIP8_H_
