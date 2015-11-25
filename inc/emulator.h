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
    unsigned int I = 0;                     //16 bits allocated, but only 12 will effectively be used
    unsigned int pc = 0;                    //program counter
    unsigned int opcode = 0;
    unsigned int stack[STACK_SIZE];
    unsigned int stack_ptr = 0;
    unsigned int delay_timer = 0;
    unsigned int sound_timer = 0;
    unsigned char keys[KEYS];
    unsigned char display[DISPLAY_SIZE];
    unsigned int display_width = DISPLAY_WIDTH;
    unsigned int display_height = DISPLAY_HEIGHT;
} chip8;