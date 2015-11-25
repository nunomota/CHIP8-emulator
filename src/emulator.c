#include <emulator.h>

int main() {

    chip8 chip;

    initChip(&chip);
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

void runChip(chip8* chip) {

    //fetch opcode
    chip->opcode = (chip->memory[chip->pc] << 8) | chip->memory[chip->pc+1];

    //decode opcode
    switch(chip->opcode & 0xF000) {
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
        default:
            printf("OP code %04X is not supported", chip->opcode);
        break;
    }

}