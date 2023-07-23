#include "chip8.h"
#include "screen.h"

// TODO: ADD REMAINING INSTRUCTIONS
Chip8OpCode instructions_array[16] =  { opfunc_00, op_1nnn, op_2nnn, op_3xkk,
                                        op_4xkk, op_5xy0, op_6xkk, op_7xkk,
                                        opfunc_8, op_9xy0, op_Annn, op_Bnnn,
                                        op_Cxkk, op_Dxyn, cpu_NULL, cpu_NULL,
                                       };

Chip8OpCode oparr_00[2] = { op_00E0, op_00EE };

Chip8OpCode op_arr8[9] = { op_8xy0, op_8xy1, op_8xy2,
                           op_8xy3, op_8xy4, op_8xy5,
                           op_8xy6, op_8xy7, op_8xyE,
                         };

void initCHIP8(struct CHIP8State *cpu){
    // Initialize CPU state, e.g., setting everything to zero
    memset(cpu->memory, 0, sizeof(uint8_t) * MEMORY_SIZE);
    memset(cpu->V, 0, sizeof(uint8_t) * REGISTER_SIZE);
    cpu->I = 0;
    cpu->pc = 0x200;
    cpu->sp = 0;
    memset(cpu->stack, 0, sizeof(uint16_t) * STACK_SIZE);
    cpu->delayTimer = 0;
    cpu->soundTimer = 0;
    cpu->paused = 0;
    cpu->cpuSpeed = 10;
}

// Load sprites into memory
void loadSprites(struct CHIP8State *cpu){
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
        cpu->memory[i] = sprites[i];
    }
}

// Load Rom into memory
int loadRom(struct CHIP8State *cpu, const char *filename){
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
        cpu->memory[rom_address++] = rom_buffer[i];
    }

    // Free the ROM buffer and close the file
    free(rom_buffer);
    fclose(rom_file);

    // success
    return 0;
}

void interpret_instruction(struct CHIP8State *cpu){
    uint16_t op = (cpu->memory[cpu->pc] << 8) | cpu->memory[cpu->pc + 1];

    cpu->pc += 2;

   instructions_array[(op & 0xF000)>>12](cpu, op);
}


// TODO: needs a lot of work
void cycle(struct CHIP8State *cpu){
    while(1){
        for(int i = 0; i < cpu->cpuSpeed; i++){
        if(!cpu->paused)
            interpret_instruction(cpu);

    }
        if(!cpu->paused)
            updateTimer(cpu);

        printScreen();
    }
}


void updateTimer(struct CHIP8State *cpu){
    if(cpu->delayTimer > 0)
        cpu->delayTimer -= 1;

    if(cpu->soundTimer > 0)
        cpu->soundTimer -= 1;
}

uint8_t wrapping_add(uint8_t a, uint8_t b) {
    return a + b;
}
// TODO: ADD NOTHING 0000 - NOP

// CLS - Clear Display
void op_00E0(struct CHIP8State *cpu, uint16_t op){
    clearScreen();
}

/* RET - Return from Subroutine
 * The interpreter sets the program counter to the address at the top of the stack,
 * then subtracts 1 from the stack pointer. */
void op_00EE(struct CHIP8State *cpu, uint16_t op){
   cpu->pc = cpu->stack[cpu->sp];
   cpu->stack[cpu->sp] = 0;
   cpu->sp--;
}

/* JP addr - Jump to location nnn.
 * Jump to location nnn.
 * The interpreter sets the program counter to nnn. */
void op_1nnn(struct CHIP8State *cpu, uint16_t op){
   cpu->pc = op & 0x0FFF;
}

/*
 * CALL addr -  Call subroutine at nnn.
 * The interpreter increments the stack pointer,
 * then puts the current PC on the top of the stack. The PC is then set to nnn.
 */
void op_2nnn(struct CHIP8State *cpu, uint16_t op){
    cpu->sp++;
    cpu->stack[cpu->sp] = cpu->pc;
    cpu->pc = op & 0x0FFF;
}

/* SE Vx, byte - Skip next instruction if Vx = kk.
 * The interpreter compares register Vx to kk,
 * and if they are equal, increments the program counter by 2.
 */
void op_3xkk(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t kk = (op & 0x00FF);

    if(cpu->V[x] == kk){
        cpu->pc += 2;
    }
}

/*
 * SNE Vx, byte - Skip next instruction if Vx != kk.
 * The interpreter compares register Vx to kk,
 * and if they are not equal, increments the program counter by 2.
*/
void op_4xkk(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t kk = (op & 0x00FF);

    if(cpu->V[x] != kk){
        cpu->pc += 2;
    }
}

/* SE Vx, Vy - Skip next instruction if Vx = Vy.
 * The interpreter compares register Vx to register Vy,
 * and if they are equal, increments the program counter by 2.
 */
void op_5xy0(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    if(cpu->V[x] == cpu->V[y]){
        cpu->pc += 2;
    }
}

/*
 * LD Vx, byte - Set Vx = kk.
 * The interpreter puts the value kk into register Vx.
 */
void op_6xkk(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t kk = (op & 0x00FF);

    cpu->V[x] = kk;
}


/*
 * ADD Vx, byte - Set Vx = Vx + kk.
 * Adds the value kk to the value of register Vx,
 * then stores the result in Vx.
 */
void op_7xkk(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t kk = (op & 0x00FF);

    cpu->V[x] = wrapping_add(cpu->V[x], kk);
}

/*
 * LD Vx, Vy - Set Vx = Vy.
 * Stores the value of register Vy in register Vx.
 */
void op_8xy0(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    cpu->V[x] = cpu->V[y];
}

/*
 * OR Vx, Vy - Set Vx = Vx OR Vy.
 * Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx.
 */
void op_8xy1(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    cpu->V[x] |= cpu->V[y];
}


/*
 * AND Vx, Vy - Set Vx = Vx AND Vy.
 * Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
 */
void op_8xy2(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    cpu->V[x] &= cpu->V[y];
}

/*
 * XOR Vx, Vy - Set Vx = Vx XOR Vy.
 * Performs a bitwise exclusive OR on the values of Vx and Vy,
 * then stores the result in Vx.*/
void op_8xy3(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    cpu->V[x] ^= cpu->V[y];
}

/*
 * ADD Vx, Vy - Set Vx = Vx + Vy, set VF = carry.
 * The values of Vx and Vy are added together.
 * If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0.
 * Only the lowest 8 bits of the result are kept, and stored in Vx.
 */
void op_8xy4(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    // Perform addition with overflow check
    uint16_t result = cpu->V[x] + cpu->V[y];

    // Check for overflow and update the carry flag (VF)
    cpu->V[0xF] = (result > 0xFF) ? 1 : 0;

    // Store the lowest 8 bits of the result in Vx
    cpu->V[x] = result & 0xFF;
}

/*
 * SUB Vx, Vy - Set Vx = Vx - Vy, set VF = NOT borrow.
 * If Vx > Vy, then VF is set to 1, otherwise 0.
 * Then Vy is subtracted from Vx, and the results stored in Vx.
 */
void op_8xy5(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    if(cpu->V[x] > cpu->V[y]){
        cpu->V[0xF] = 1;
    } else{
        cpu->V[0xF] = 0;
    }

    cpu->V[x] = cpu->V[x] -  cpu->V[y];
}

/*
 * SHR Vx {, Vy} - Set Vx = Vx SHR 1.
 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0.
 * Then Vx is divided by 2.
 */
void op_8xy6(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t lsb = cpu->V[x] & 1;
    if( lsb == 1 ) {
        cpu->V[0xF] = 1;
    } else {
        cpu->V[0xF] = 0;
    }

    cpu->V[x] >>= 1;
}

/*
 * SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow.
 * If Vy > Vx, then VF is set to 1, otherwise 0.
 * Then Vx is subtracted from Vy, and the results stored in Vx.
 */
void op_8xy7(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    if(cpu->V[y] > cpu->V[x]){
        cpu->V[0xF] = 1;
    } else{
        cpu->V[0xF] = 0;
    }

    cpu->V[x] = cpu->V[y] - cpu->V[x];
}

/*
 * SHL Vx {, Vy} - Set Vx = Vx SHL 1.
 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0.
 * Then Vx is multiplied by 2.
 */
void op_8xyE(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;

    // Get most significant bit of Vx
    uint8_t msb = (cpu->V[x] >> 7) & 1;

    // Shift Vx left by 1 bit
    cpu->V[x] <<= 1;

    // Set VF to MSB of Vx
    cpu->V[0xF] = msb;
}

/*
 * SNE Vx, Vy
 * Skip next instruction if Vx != Vy.
 * The values of Vx and Vy are compared, and if they are not equal,
 * the program counter is increased by 2.*/
void op_9xy0(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    if(cpu->V[x] != cpu->V[y]){
       cpu->pc += 2;
    }
}

/*
 * LD I, addr - Set I = nnn.
 * The value of register I is set to nnn.
 */
void op_Annn(struct CHIP8State *cpu, uint16_t op){
    cpu->I = op & 0x0FFF;
}

/*
 * JP V0, addr - Jump to location nnn + V0.
 * The program counter is set to nnn plus the value of V0.
 */
void op_Bnnn(struct CHIP8State *cpu, uint16_t op){
    uint16_t nnn = op & 0x0FFF;
    cpu->pc = nnn + cpu->V[0];
}

/*
 * RND Vx, byte - Set Vx = random byte AND kk.
 * The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk.
 * The results are stored in Vx.
 */
void op_Cxkk(struct CHIP8State *cpu, uint16_t op){

    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t kk = (op & 0x00FF);
    uint8_t random_num = rand() % 256; // generates a random number between 0 and 255

    cpu->V[x] = random_num & kk;
}

/*
 * DRW Vx, Vy, nibble - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
 * The interpreter reads n bytes from memory, starting at the address stored in I.
 * These bytes are then displayed as sprites on screen at coordinates (Vx, Vy).
 * Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
 * If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen.
 */
void op_Dxyn(struct CHIP8State *cpu, uint16_t op){
   // TODO: this is probably wrong, needs to be fixed
    uint8_t x = (op & 0x0F00) >> 8;
    uint8_t y = (op & 0x00F0) >> 4;

    int height = op & 0x000F;
    int width = 8;
    int pixel;

    cpu->V[0xF] = 1;

    for(int row = 0; row < height; row++){
        pixel = cpu->memory[cpu->I + row];

        for(int col = 0; col < width; col++){
            if((pixel & 0x080) > 0){
                if(setPixel(cpu->V[x] + col, cpu->V[y] + row)){
                    cpu->V[0xF] = 1;
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
void op_Fx07(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    cpu->V[x] = cpu->delayTimer;
}


// TODO Fx0A

/*
 * LD DT, Vx - Set delay timer = Vx.
 * DT is set equal to the value of Vx.
 */
void op_Fx15(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    cpu->delayTimer = cpu->V[x];
}

/*
 * LD ST, Vx - Set sound timer = Vx.
 * ST is set equal to the value of Vx.
 */
void op_Fx18(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    cpu->soundTimer = cpu->V[x];
}

/*
 * ADD I, Vx - Set I = I + Vx.
 * The values of I and Vx are added, and the results are stored in I.
 */
void op_Fx1E(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;
    cpu->I += cpu->V[x];
}

/*
 * LD F, Vx - Set I = location of sprite for digit Vx.
 * The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx.*/
void op_Fx29(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;

    cpu->I = cpu->V[x] * 5; // Each sprite is 5 bytes long
}

/*
 * LD B, Vx - Store BCD representation of Vx in memory locations I, I+1, and I+2.
 * The interpreter takes the decimal value of Vx,
 * and places the hundreds digit in memory at location in I,
 * the tens digit at location I+1, and the ones digit at location I+2.*/
void op_Fx33(struct CHIP8State *cpu, uint16_t op){
    uint8_t x = (op & 0x0F00) >> 8;

    uint16_t value = cpu->V[x];
    cpu->memory[cpu->I] = value / 100; // store hundreds digit
    cpu->memory[cpu->I+1] = (value / 10) % 10; // store tens digit
    cpu->memory[cpu->I+2] = value % 10; // store ones digit
}

/*
 * LD [I], Vx - Store registers V0 through Vx in memory starting at location I.
 * The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
 */
void op_Fx55(struct CHIP8State *cpu, uint16_t op){

    uint8_t x = (op & 0x0F00) >> 8;

    for (int i = 0; i <= x; i++) {
        cpu->memory[cpu->I + i] = cpu->V[i];
    }
}

/*
 * LD Vx, [I] - Read registers V0 through Vx from memory starting at location I.
 * The interpreter reads values from memory starting at location I into registers V0 through Vx.
 */
void op_Fx65(struct CHIP8State *cpu, uint16_t op){

    uint8_t x = (op & 0x0F00) >> 8;

    for(int i = 0; i <= x; i++) {
        cpu->V[i] = cpu->memory[cpu->I + i];
    }
}


void cpu_NULL(struct CHIP8State *cpu, uint16_t op){

}

void opfunc_00(struct CHIP8State *cpu, uint16_t op){
    if((op & 0xF) == 0){
        oparr_00[0](cpu, op);
    } else{
       oparr_00[1](cpu, op);
    }
}


void opfunc_8(struct CHIP8State *cpu, uint16_t op){
    if ((op & 0xF) < 8) {
        op_arr8[op & 0xF](cpu, op);
    } else if ((op & 0xF) == 0xE) {
        op_arr8[8](cpu, op);
}

}
