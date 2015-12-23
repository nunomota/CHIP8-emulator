/*
This is a really simple implementation of a Chip8 emulator.

I chose to make it an 'interpreting' and not a 'recompiling' emulator for the sake of simplicity (as a static recompiling 
one has its downfall for self-modifying code, a dynamic one should be chosen instead - which would mean a lot of work) 
but I might change this in the future.

Implementation choices:
   - use base size 'int' over 'short' or 'long' 
      -> reduces amount of code the compiler generates to convert between different integer lengths;
      -> may also reduce the memory access time, as some CPUs work fastest when reading/writing data of the base size;
   - use 'for(;;)' instead of 'while(CPUIsRunning)' or 'while(1)'
      -> 'while(CPUIsRunning)' can terminate the loop at any moment by setting the variable to 0, which is good,
      but checking the variable on each pass can waste quite a lot of CPU time;
      -> 'while(1)' is clearly a bad choice, as some compilers will generate code checking whether 1 is true or not;
   - make memory read/write directly to an array and not through a function
      -> this will avoid the overhead associated with a function call;
      -> it is just possible because we are not using, for example, paged memory;

Resources:
   http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
   https://en.wikipedia.org/wiki/CHIP-8#Opcode_table
   http://fms.komkon.org/EMUL8/HOWTO.html
*/

#include <emulator.h>

int main() {

    chip8 chip;

    initChip(&chip);
    requestRom(&chip);
    runChip(&chip);
    return 0;
}

void initChip(chip8* chip) {
    memset(chip->memory, 0, CHIP_MEMORY*sizeof(unsigned char));
    memset(chip->v_reg, 0, V_REGISTERS*sizeof(unsigned char));
    chip->I = 0;
    chip->pc = 0;
    chip->opcode = 0;
    memset(chip->stack, 0, STACK_SIZE*sizeof(unsigned int));
    chip->stack_ptr = 0;
    chip->delay_timer = 0;
    chip->sound_timer = 0;
    memset(chip->keys, 0, KEYS*sizeof(unsigned char));
    memset(chip->display, 0, DISPLAY_SIZE*sizeof(unsigned char));
    chip->display_width = DISPLAY_WIDTH;
    chip->display_height = DISPLAY_HEIGHT;
}

void requestRom(chip8* chip) {

    char inputBuffer[MAX_INPUT_SIZE];

    printf("ROM's path: ");
    if (fgets(inputBuffer, MAX_INPUT_SIZE, stdin) != null) {
        loadRomToChip(chip, inputBuffer);
    } else {
        error("Could not read user input");
    }
}

void loadRomToChip(chip8* chip, char* romPath) {

    FILE* file;

    file = fopen(romPath, rb);
    if (!file) {
        error("Could not load specified file");
    } else {
        fread(chip->memory, 1, CHIP_MEMORY, file);
    }
    fclose(file);
}

void runChip(chip8* chip) {

    //fetch opcode
    chip->opcode = (chip->memory[chip->pc] << 8) | chip->memory[chip->pc+1];

    //decode opcode
    switch(chip->opcode & 0xF000) {
        int auxiliary_var, i;
        int x, y, vx, vy;
        unsigned int height, pixel_value;
        case 0x0000:
            switch(chip->opcode & 0x00FF) {
                case 0x00E0:    //00E0, clears the screen
                    memset(chip->display, 0, DISPLAY_SIZE*sizeof(unsigned char));
                    chip->pc += 2;
                break;
                case 0x00EE:    //00EE, returns from a subroutine
                    chip->pc = chip->stack[(--chip->stack_ptr)&0xF] + 2;
                break;
                default:        //0NNN, calls RCA 1802 program at address NNN
                    //TODO implement this opcode
                    chip->pc += 2;
                break;
            }
        case 0x1000:        //1NNN, jumps to address NNN
            chip->pc = chip->opcode & 0x0FFF;
        break;
        case 0x2000:        //2NNN, calls subroutine at NNN
            chip->stack[(chip->stack_ptr++)&0xF] = chip->pc;
            chip->pc = chip->opcode & 0x0FFF;
        break;
        case 0x3000:        //3XNN, skips the next instruction if VX equals NN
            if(chip->v_reg[(chip->opcode & 0x0F00) >> 8] == (chip->opcode & 0x00FF)) {
                chip->pc += 4;
            } else {
                chip->pc += 2;
            }
        break;
        case 0x4000:        //4XNN, skips the next instruction if VX doesn't equal NN
            if(chip->v_reg[(chip->opcode & 0x0F00) >> 8] != (chip->opcode & 0x00FF)) {
                chip->pc += 4;
            } else {
                chip->pc += 2;
            }
        break;
        case 0x5000:        //5XY0, skips the next instruction if VX equals VY
            if(chip->v_reg[(chip->opcode & 0x0F00) >> 8] == chip->v_reg[(chip->opcode & 0x00F0) >> 4]) {
                chip->pc += 4;
            } else {
                chip->pc += 2;
            }
        break;
        case 0x6000:        //6XNN, sets VX to NN
            chip->v_reg[(chip->opcode & 0x0F00) >> 8] = (chip->opcode & 0x00FF);
            chip->pc += 2;
        break;
        case 0x7000:        //7XNN, adds NN to VX
            chip->v_reg[(chip->opcode & 0x0F00) >> 8] += (chip->opcode & 0x00FF);
            chip->pc += 2;
        break;
        case 0x8000:
            switch(chip->opcode & 0x000F) {
                case 0x0000:        //8XY0, sets VX to the value of VY
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = chip->v_reg[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                break;
                case 0x0001:        //8XY1, sets VX to VX or VY
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = chip->v_reg[(chip->opcode & 0x0F00) >> 8] | chip->v_reg[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                break;
                case 0x0002:        //8XY2, sets VX to VX and VY
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = chip->v_reg[(chip->opcode & 0x0F00) >> 8] & chip->v_reg[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                break;
                case 0x0003:        //8XY3, sets VX to VX xor VY
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = chip->v_reg[(chip->opcode & 0x0F00) >> 8] ^ chip->v_reg[(chip->opcode & 0x00F0) >> 4];
                    chip->pc += 2;
                break;
                case 0x0004:        //8XY4, adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't
                    auxiliary_var = (int)chip->v_reg[(chip->opcode & 0x0F00) >> 8 ] + (int)chip->v_reg[(chip->opcode & 0x00F0) >> 4];
                    if(auxiliary_var < 256) {
                        chip->v_reg[0xF] &= 0;
                    } else {
                        chip->v_reg[0xF] = 1;
                    }
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = auxiliary_var & 0xF;
                    chip->pc += 2;
                break;
                case 0x0005:        //8XY5, VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't
                    auxiliary_var = (int)chip->v_reg[(chip->opcode & 0x0F00) >> 8 ] - (int)chip->v_reg[(chip->opcode & 0x00F0) >> 4];
                    if(auxiliary_var >= 0) {
                        chip->v_reg[0xF] = 1;
                    } else {
                        chip->v_reg[0xF] &= 0;
                    }
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = auxiliary_var & 0xF;
                    chip->pc += 2;
                break;
                case 0x0006:        //8XY6, shifts VX right by one. VF is set to the value of the least significant bit of VX before the shift
                    chip->v_reg[0xF] = chip->v_reg[(chip->opcode & 0x0F00) >> 8] & 7;
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = chip->v_reg[(chip->opcode & 0x0F00) >> 8] >> 1;
                    chip->pc += 2;
                break;
                case 0x0007:        //8XY7, sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't
                    auxiliary_var = (int)chip->v_reg[(chip->opcode & 0x00F0) >> 4] - (int)chip->v_reg[(chip->opcode & 0x0F00) >> 8];
                    if(auxiliary_var >= 0) {
                        chip->v_reg[0xF] = 1;
                    } else {
                        chip->v_reg[0xF] &= 0;
                    }
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = auxiliary_var & 0xF;
                    chip->pc += 2;
                break;
                case 0x000E:        //8XYE, shifts VX left by one. VF is set to the value of the most significant bit of VX before the shift
                    chip->v_reg[0xF] = chip->v_reg[(chip->opcode & 0x0F00) >> 8] >> 7;
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = chip->v_reg[(chip->opcode & 0x0F00) >> 8] << 1;
                    chip->pc += 2;
                break;
                default:
                    unsupportedOpCode(chip->opcode);
                break;
            }
        break;
        case 0x9000:        //9XY0, skips the next instruction if VX doesn't equal VY
            if(chip->v_reg[(chip->opcode & 0x0F00) >> 8] != chip->v_reg[(chip->opcode & 0x00F0) >> 4]) {
                chip->pc += 4;
            } else {
                chip->pc += 2;
            }
        break;
        case 0xA000:        //ANNN, sets I to the address NNN
            chip->I = chip->opcode & 0x0FFF;
            chip->pc += 2;
        break;
        case 0xB000:        //BNNN, jumps to the address NNN plus V0
            chip->pc = (chip->opcode & 0x0FFF) + chip->v_reg[0];
        break;
        case 0xC000:        //CXNN, sets VX to the result of a bitwise and operation on a random number and NN
            chip->v_reg[(chip->opcode & 0x0F00) >> 8] = rand() & (chip->opcode & 0x00FF);
            chip->pc += 2;
        break;
        case 0xD000:        //DXYN, sprites stored in memory at location in index register (I), 8bits wide. Wraps around the screen. If when drawn, clears a pixel, register VF is set to 1 otherwise it is zero. All drawing is XOR drawing (i.e. it toggles the screen pixels). Sprites are drawn starting at position VX, VY. N is the number of 8bit rows that need to be drawn. If N is greater than 1, second line continues at position VX, VY+1, and so on...
            vx = chip->v_reg[(chip->opcode & 0x0F00) >> 8];
            vy = chip->v_reg[(chip->opcode & 0x00F0) >> 4];
            height = chip->opcode & 0x000F;  
            chip->v_reg[0xF] &= 0;
           
            for(y = 0; y < height; y++) {
                pixel_value = chip->memory[chip->I + y];
                for(x = 0; x < 8; x++) {
                    if((pixel_value & (0x80 >> x)) != 0) {
                        if(chip->display[x+vx+(y+vy)*64] != 0) {
                            chip->v_reg[0xF] = 1;
                        }
                        chip->display[x+vx+(y+vy)*64] ^= 1;
                    }
                }
            }
            chip->pc += 2;
        break;
        case 0xE000:
            switch(chip->opcode & 0x000F) {
                case 0x000E:        //EX9E, skips the next instruction if the key stored in VX is pressed
                    chip->pc += 2;
                break;
                case 0x0001:        //EXA1, skips the next instruction if the key stored in VX isn't pressed
                    chip->pc += 2;
                break;
                default:
                    unsupportedOpCode(chip->opcode);
                break;
            }
        break;
        case 0xF000:
            switch(chip->opcode & 0x00FF) {
                case 0x0007:        //FX07, sets VX to the value of the delay timer
                    chip->v_reg[(chip->opcode & 0x0F00) >> 8] = chip->delay_timer;
                    chip->pc += 2;
                break;
                case 0x000A:        //FX0A, a key press is awaited, and then stored in VX
                    chip->pc += 2;
                break;
                case 0x0015:        //FX15, sets the delay timer to VX
                    chip->delay_timer = chip->v_reg[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                break;
                case 0x0018:        //FX18, sets the sound timer to VX
                    chip->sound_timer = chip->v_reg[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                break;
                case 0x001E:        //FX1E, adds VX to I
                    chip->I += chip->v_reg[(chip->opcode & 0x0F00) >> 8];
                    chip->pc += 2;
                break;
                case 0x0029:        //FX29, sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
                    chip->I = chip->v_reg[(chip->opcode & 0x0F00) >> 8] * 5;
                    chip->pc += 2;
                break;
                case 0x0033:        //FX33, stores the Binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
                    chip->memory[chip->I] = chip->v_reg[(chip->opcode & 0x0F00) >> 8] / 100;
                    chip->memory[chip->I+1] = (chip->v_reg[(chip->opcode & 0x0F00) >> 8] / 10) % 10;
                    chip->memory[chip->I+2] = chip->v_reg[(chip->opcode & 0x0F00) >> 8] % 10;
                    chip->pc += 2;
                break;
                case 0x0055:        //FX55, stores V0 to VX in memory starting at address I
                    for(i = 0; i <= ((chip->opcode & 0x0F00) >> 8); i++) {
                        chip->memory[chip->I+i] = chip->v_reg[i];
                    }
                    chip->pc += 2;
                break;
                case 0x0065:        //FX65, fills V0 to VX with values from memory starting at address I
                    for(i = 0; i <= ((chip->opcode & 0x0F00) >> 8); i++) {
                        chip->v_reg[i] = chip->memory[chip->I + i];
                    }
                    chip->pc += 2;
                break;
                default:
                    unsupportedOpCode(chip->opcode);
                break;
            }
        break;
        default:
            unsupportedOpCode(chip->opcode);
        break;
    }

}

void unsupportedOpCode(unsigned int opcode) {
    printf("OP code %04X is not supported", opcode);
}

void error(char* errorMessage) {
    printf(errorMessage);
    exit(0);
}