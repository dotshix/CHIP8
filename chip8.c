#include "chip8.h"
#include "screen.h"

// Used to store memory
uint8_t memory[MEMORY_SIZE] = { 0 };

// Used to store registers
uint8_t V[REGISTER_SIZE] = { 0 };

// index register
int16_t I = 0;

// program counter register
// starts at position 0x200
int16_t pc = 0x200;

// program stack pointer
int8_t sp = 0;

// pseudo stack using array
int16_t stack[STACK_SIZE] = { 0 };

// delay timer
int8_t delayTimer = 0;

// sound timer
int8_t soundTimer = 0;

int16_t paused = 0;

int16_t cpuSpeed = 10;

/* void interpret(uint16_t instruction){ */

/*     pc += 2; */

/*     // X and Y values are used a lot */
/*     // so we are grabbing them early */

/*     const uint16_t x = (x & 0x0F00) >> 8; */
/*     const uint16_t y = (x & 0x00F0) >> 4; */

/* } */

// CLS - Clear Display
void op_00E0(int16_t instruction){
    clearScreen();
}

// RET - Return from Subroutine
/* The interpreter sets the program counter to the address at the top of the stack,
 * then subtracts 1 from the stack pointer. */
void op_00EE(int16_t instruction){
   pc = stack[sp];
   stack[sp] = 0;
   sp--;
}

/* JP addr - Jump to location nnn.
 * Jump to location nnn.
 * The interpreter sets the program counter to nnn. */
void op_1nnn(int16_t instruction){
   pc = instruction & 0x0FFF;
}

/*
 * CALL addr -  Call subroutine at nnn.
 * The interpreter increments the stack pointer,
 * then puts the current PC on the top of the stack. The PC is then set to nnn.
 */
void op_2nnn(int16_t instruction){
    sp++;
    stack[sp] = pc;
    pc = instruction & 0x0FFF;
}

/* SE Vx, byte - Skip next instruction if Vx = kk.
 * The interpreter compares register Vx to kk,
 * and if they are equal, increments the program counter by 2.
 */
void op_3xkk(int16_t instruction){

    /*TODO: Change this to get x */
    if(V[(instruction & 0x0F00) >> 8] == (instruction & 0x00FF)){
        pc += 2;
    }

}

/*
 * SNE Vx, byte - Skip next instruction if Vx != kk.
 * The interpreter compares register Vx to kk,
 * and if they are not equal, increments the program counter by 2.
*/
void op_4xkk(int16_t instruction){
    // TODO: Change to x
    if(V[(instruction & 0x0F00) >> 8] != (instruction & 0x00FF)){
        pc += 2;
    }
}

/* SE Vx, Vy - Skip next instruction if Vx = Vy.
 * The interpreter compares register Vx to register Vy,
 * and if they are equal, increments the program counter by 2.
 */
void op_5xy0(int16_t instruction){
    //TODO: Change x and y

    if(V[(instruction & 0x0F00) >> 8] == V[(instruction & 0x00F0) >> 4])
        pc += 2;
}

/*
 * LD Vx, byte - Set Vx = kk.
 * The interpreter puts the value kk into register Vx.
 */
void op_6xkk(int16_t instruction){
    //TODO: Change x
    V[(instruction & 0x0F00) >> 8] = (instruction & 0x00FF);
}


/*
 * ADD Vx, byte - Set Vx = Vx + kk.
 * Adds the value kk to the value of register Vx,
 * then stores the result in Vx.
 */
void op_7xkk(int16_t instruction){
    //TODO: Change x

    V[(instruction & 0x0F00) >> 8] = V[(instruction & 0x0F00) >> 8] + (instruction & 0x00FF);
}

/*
 * LD Vx, Vy - Set Vx = Vy.
 * Stores the value of register Vy in register Vx.
 */
void op_8xy0(int16_t instruction){

    // TODO: Change x and y

    V[(instruction & 0x0F00) >> 8] = V[(instruction & 0x00F0) >> 4];
}

/*
 * OR Vx, Vy - Set Vx = Vx OR Vy.
 * Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
 */
void op_8xy1(int16_t instruction){
    // TODO: Change x and y

    V[(instruction & 0x0F00) >> 8] = V[(instruction & 0x0F00) >> 8] | V[(instruction & 0x00F0) >> 4];
}


/*
 * AND Vx, Vy - Set Vx = Vx AND Vy.
 * Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
 */
void op_8xy2(int16_t instruction){
    // TODO: Change x and y

    V[(instruction & 0x0F00) >> 8] = V[(instruction & 0x0F00) >> 8] & V[(instruction & 0x00F0) >> 4];
}

/*
 * XOR Vx, Vy - Set Vx = Vx XOR Vy.
 * Performs a bitwise exclusive OR on the values of Vx and Vy,
 * then stores the result in Vx.*/
void op_8xy3(int16_t instruction){
    // TODO: Change x and y

    V[(instruction & 0x0F00) >> 8] = V[(instruction & 0x0F00) >> 8] ^ V[(instruction & 0x00F0) >> 4];
}

/*
 * ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry.
 * The values of Vx and Vy are added together.
 * If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
 * Only the lowest 8 bits of the result are kept, and stored in Vx.
 */
void op_8xy4(int16_t instruction){
    // TODO: Change x and y

    uint16_t result = V[(instruction & 0x0F00) >> 8] + V[(instruction & 0x00F0) >> 4];

    if(result > 255) {
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }

    V[(instruction & 0x0F00) >> 8] = (uint8_t) result;
}


/*
 * SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow.
 * If Vx > Vy, then VF is set to 1, otherwise 0.
 * Then Vy is subtracted from Vx, and the results stored in Vx.
 */
void op_8xy5(int16_t instruction){
    // TODO: Change x and y

    if(V[(instruction & 0x0F00) >> 8] > V[(instruction & 0x00F0) >> 4]){
        V[0xF] = 1;
    } else{
        V[0xF] = 0;
    }

    V[(instruction & 0x0F00) >> 8] = V[(instruction & 0x0F00) >> 8] -  V[(instruction & 0x00F0) >> 4];
}

/*
 * SHR Vx {, Vy} - Set Vx = Vx SHR 1.
 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
 * Then Vx is divided by 2.
 */
void op_8xy6(int16_t instruction){
    // TODO: Change x
    if( (V[(instruction & 0x0F00) >> 8] & 1) == 1 ) {
        V[0xF] = 1;
    } else {
        V[0xF] = 0;
    }

    V[(instruction & 0x0F00) >> 8] >>= 1;
}

/*
 * SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow.
 * If Vy > Vx, then VF is set to 1, otherwise 0.
 * Then Vx is subtracted from Vy, and the results stored in Vx.
 */
void op_8xy7(int16_t instruction){
   // TODO: Change x and y

    if(V[(instruction & 0x00F0) >> 4] > V[(instruction & 0x0F00) >> 8]){
        V[0xF] = 1;
    } else{
        V[0xF] = 0;
    }

    V[(instruction & 0x0F00) >> 8] = V[(instruction & 0x00F0) >> 4] - V[(instruction & 0x0F00) >> 8];
}

/*
 * SHL Vx {, Vy} - Set Vx = Vx SHL 1.
 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
 * Then Vx is multiplied by 2.
 */
void op_8xyE(int16_t instruction){

    // TODO: CHange x
    // Get most significant bit of Vx
    uint8_t msb = (V[(instruction & 0x0F00) >> 8] >> 7) & 0x01;

    // Shift Vx left by 1 bit
    V[(instruction & 0x0F00) >> 8] <<= 1;

    // Set VF to MSB of Vx
    V[0xF] = msb;
}

/*
 * SNE Vx, Vy
 * Skip next instruction if Vx != Vy.
 * The values of Vx and Vy are compared, and if they are not equal,
 * the program counter is increased by 2.*/
void op_9xy0(int16_t instruction){
  // TODO: Change x and y

    if(V[(instruction & 0x0F00) >> 8] != V[(instruction & 0x00F0) >> 4]){
       pc += 2;
    }
}

/*
 * LD I, addr - Set I = nnn.
 * The value of register I is set to nnn.
 */
void op_Annn(int16_t instruction){
    I = instruction & 0x0FFF;
}

/*
 * JP V0, addr - Jump to location nnn + V0.
 * The program counter is set to nnn plus the value of V0.
 */
void op_Bnnn(int16_t instruction){
    pc = (instruction & 0x0FFF) + V[0];
}

/*
 * RND Vx, byte - Set Vx = random byte AND kk.
 * The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
 * The results are stored in Vx.
 */
void op_Cxkk(int16_t instruction){

    //TODO: change x
    int random_num = rand() % 256; // generates a random number between 0 and 255

    V[(instruction & 0x0F00) >> 8] = random_num  & (instruction & 0x00FF);
}

/*
 * DRW Vx, Vy, nibble - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
 * The interpreter reads n bytes from memory, starting at the address stored in I.
 * These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
 * Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
 * If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen.
 */
void op_Dxyn(int16_t instruction){
   // TODO: this is probably wrong, also x and y again

    int height = instruction & 0x000F;
    int width = 8;
    int pixel;

    V[0xF] = 1;

    for(int row = 0; row < height; row++){
        pixel = memory[I + row];

        for(int col = 0; col < width; col++){
            if((pixel & 0x080) > 0){
                if(setPixel(V[(instruction & 0x0F00) >> 8] + col, V[(instruction & 0x00F0) >> 4] +row)){
                    V[0xF] = 1;
                }
            }
            pixel <<= 1;
        }
    }
}

/* TODO NEED TO IMPLEMENT OP CODE
 * NEED TO IMPLEMENT KEYBOARD FOR THESE
 * Ex9E, ExA1, */


/*
 * LD Vx, DT - Set Vx = delay timer value.
 * The value of DT is placed into Vx.
 */
void op_Fx07(int16_t instruction){
    // TODO: Change x
    V[(instruction & 0x0F00) >> 8] = delayTimer;
}


// TODO Fx0A

/*
 * LD DT, Vx - Set delay timer = Vx.
 * DT is set equal to the value of Vx.
 */
void op_Fx15(int16_t instruction){
    // TODO: Change x
    delayTimer = V[(instruction & 0x0F00) >> 8];
}

/*
 * LD ST, Vx - Set sound timer = Vx.
 * ST is set equal to the value of Vx.
 */
void op_Fx18(int16_t instruction){
    // TODO: Change x
    soundTimer = V[(instruction & 0x0F00) >> 8];
}

/*
 * ADD I, Vx - Set I = I + Vx.
 * The values of I and Vx are added, and the results are stored in I.
 */
void op_Fx1E(int16_t instruction){
    // TODO: Change x
    I += V[(instruction & 0x0F00) >> 8];
}

/*
 * LD F, Vx - Set I = location of sprite for digit Vx.
 * The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.*/
void op_Fx29(int16_t instruction){
    // TODO: get x
    I = V[(instruction & 0x0F00) >> 8] * 5; // Each sprite is 5 bytes long
}

/*
 * LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, and I+2.
 * The interpreter takes the decimal value of Vx,
 * and places the hundreds digit in memory at location in I,
 * the tens digit at location I+1, and the ones digit at location I+2.*/
void op_Fx33(int16_t instruction){
    // TODO: get x
    int value = V[(instruction & 0x0F00) >> 8];
    memory[I] = value / 100; // store hundreds digit
    memory[I+1] = (value / 10) % 10; // store tens digit
    memory[I+2] = value % 10; // store ones digit
}

/*
 * LD [I], Vx - Store registers V0 through Vx in memory starting at location I.
 * The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
 */
void op_Fx55(int16_t instruction){

    uint8_t x = (instruction & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++) {
        memory[I + i] = V[i];
    }
}

/*
 * LD Vx, [I] - Read registers V0 through Vx from memory starting at location I.
 * The interpreter reads values from memory starting at location I into registers V0 through Vx.
 */
void op_Fx65(int16_t instruction){

    uint8_t x = (instruction & 0x0F00) >> 8;

    for(int i = 0; i <= x; i++) {
        V[i] = memory[I + i];
    }
}

// Load sprites into memory
void loadSprites(void){
    uint8_t sprites[80] = {  0xF0, 0x90, 0x90, 0x90, 0xF0,
                             0x20, 0x60, 0x20, 0x20, 0x70,
                             0xF0, 0x10, 0xF0, 0x80, 0xF0,
                             0xF0, 0x10, 0xF0, 0x10, 0xF0,
                             0x90, 0x90, 0xF0, 0x10, 0x10,
                             0xF0, 0x80, 0xF0, 0x10, 0xF0,
                             0xF0, 0x80, 0xF0, 0x90, 0xF0,
                             0xF0, 0x10, 0x20, 0x40, 0x40,
                             0xF0, 0x90, 0xF0, 0x90, 0xF0,
                             0xF0, 0x90, 0xF0, 0x10, 0xF0,
                             0xF0, 0x90, 0xF0, 0x90, 0x90,
                             0xE0, 0x90, 0xE0, 0x90, 0xE0,
                             0xF0, 0x80, 0x80, 0x80, 0xF0,
                             0xE0, 0x90, 0x90, 0x90, 0xE0,
                             0xF0, 0x80, 0xF0, 0x80, 0xF0,
                             0xF0, 0x80, 0xF0, 0x80, 0x80,
                             };

    for(int i = 0; i < 80; i++){
        memory[i] = sprites[i];
    }
}

// Load Rom into memory
int loadRom(const char *filename){
    // Open the ROM file
    FILE* rom_file = fopen(filename, "rb");
    if (rom_file == NULL) {
        printf("Failed to open ROM file: %s\n", filename);
        return 1;
    }

    // Read the contents of the ROM file into a buffer
    fseek(rom_file, 0, SEEK_END);
    long rom_size = ftell(rom_file);
    rewind(rom_file);
    uint8_t* rom_buffer = (uint8_t*) malloc(rom_size * sizeof(uint8_t));
    fread(rom_buffer, sizeof(uint8_t), rom_size, rom_file);

    // Load the ROM into memory starting at address 0x200
    uint16_t rom_address = ROM_START_ADDRESS;
    for (int i = 0; i < rom_size; i++) {
        memory[rom_address++] = rom_buffer[i];
    }

    // Free the ROM buffer and close the file
    free(rom_buffer);
    fclose(rom_file);

    // success
    return 0;
}

// TODO: ADD REMAINING INSTRUCTIONS
Chip8OpCode instructions_array[16] =  { opfunc_00, op_1nnn, op_2nnn, op_3xkk,
                                        op_4xkk, op_5xy0, op_6xkk, op_7xkk,
                                        opfunc_8, op_9xy0, op_Annn, op_Bnnn,
                                        op_Cxkk, op_Dxyn, cpu_NULL, cpu_NULL,
                                       };

Chip8OpCode oparr_00[2] = { op_00E0, op_00EE };

void cpu_NULL(int16_t instruction){

}

void opfunc_00(int16_t instruction){
    if((instruction & 0xF) == 0){
        oparr_00[0](instruction);
    } else{
       oparr_00[1](instruction);
    }
}

Chip8OpCode op_arr8[9] = { op_8xy0, op_8xy1, op_8xy2, op_8xy3,
                           op_8xy4, op_8xy5, op_8xy6, op_8xy7,
                           op_8xyE,
                         };

void opfunc_8(int16_t instruction){
    /* if((instruction & 0xF) < 8){ */
    /*     op_arr8[(instruction & 0xF)](instruction); */
    /* } else{ */
    /*     op_arr8[9](instruction); */
    /* } */

    if ((instruction & 0xF) < 8) {
        op_arr8[instruction & 0xF](instruction);
    } else if ((instruction & 0xF) == 0xE) {
        op_arr8[8](instruction);
}

}
void interpret_instruction(void){
    uint16_t instruction = (memory[pc] << 8) | memory[pc + 1];

    pc += 2;

   instructions_array[(instruction & 0xF000)>>12](instruction);
}



void cycle(void){
    while(1){
        for(int i = 0; i < cpuSpeed; i++){
        if(!paused)
            interpret_instruction();

    }
        if(!paused)
            updateTimer();

        printScreen();
    }
}


void updateTimer(void){
    if(delayTimer > 0)
        delayTimer -= 1;

    if(soundTimer > 0)
        soundTimer -= 1;
}
