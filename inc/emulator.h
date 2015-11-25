#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define CHIP_MEMORY     4096                //in bytes
#define V_REGISTERS     16                  //total number of 'v' registers
#define STACK_SIZE      16
#define KEYS            16
#define DISPLAY_WIDTH   64
#define DISPLAY_HEIGHT  32
#define DISPLAY_SIZE    64*32

typedef struct {
    unsigned char memory[CHIP_MEMORY];
    unsigned char v_reg[V_REGISTERS];
    unsigned int I;                     //16 bits allocated, but only 12 will effectively be used
    unsigned int pc;                    //program counter
    unsigned int opcode;
    unsigned int stack[STACK_SIZE];
    unsigned int stack_ptr;
    unsigned int delay_timer;
    unsigned int sound_timer;
    unsigned char keys[KEYS];
    unsigned char display[DISPLAY_SIZE];
    unsigned int display_width;
    unsigned int display_height;
} chip8;

void initChip(chip8* chip);
void runChip(chip8* chip);
void unsupportedOpCode(unsigned int opcode);