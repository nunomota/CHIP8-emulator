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

}