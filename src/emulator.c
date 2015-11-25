#include <emulator.h>

chip8 chip;

int main() {
    initChip();
    return 0;
}

void initChip() {
    chip.I = 0;
    chip.pc = 0;
    chip.opcode = 0;
    chip.stack_ptr = 0;
    chip.delay_timer = 0;
    chip.sound_timer = 0;
    chip.display_width = DISPLAY_WIDTH;
    chip.display_height = DISPLAY_HEIGHT;
}