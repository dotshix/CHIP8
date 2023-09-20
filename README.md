#+TITLE: Chip8 Emulator

This is a Chip8 emulator written in C that runs in the terminal using the ncurses library. The emulator is a work in progress and is intended for educational purposes to learn about emulation, function pointers, and low-level programming.

* Features

  - Chip8 emulation with a basic display.
  - Support for a subset of Chip8 opcodes.
  - Uses function pointers for opcode handling.
  - Runs in the terminal with ncurses for a visual interface.

* To-Do List

  This project is still under development, and there are several improvements and fixes that can be made:

  1. **Optimize CPU Usage:** Currently, the emulator might have high CPU usage. Consider implementing a timing mechanism to limit the emulation speed to the original Chip8 clock rate.

  2. **Keyboard Support:** Add support for keyboard input to fully interact with Chip8 games.

  3. **Finish Opcodes:** Implement more Chip8 opcodes to improve compatibility with a wider range of Chip8 games.

* Building and Running

  To build and run the Chip8 emulator, follow these steps:

  1. Clone the repository:

     ```
     git clone https://github.com/dotshix/CHIP8.git
     ```

  2. Change into the project directory:

     ```
     cd CHIP8 
     ```

  3. Build the emulator:

     ```
     make
     ```

  4. Run the emulator:

     ```
     ./chip8
     ```

* Usage

  - Use the emulator to load and play Chip8 ROMs.
  - Keyboard input (if implemented) will be used to control the games.
