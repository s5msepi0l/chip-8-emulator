#define SDL_MAIN_HANDLED

#include "chip-8.h"

int main(int argc, char* argv[]) {
    chip_8::cpu emulator("Chip-8 emulator");
    
    emulator.load(
        {
        0x00, 0x00,
        0x12, 0x00,
        0x00, 0x00,
        0xff, 0xff
        });

    emulator.run();

    return 0;
}
