#define SDL_MAIN_HANDLED

#include "chip-8.h"

int main(int argc, char* argv[]) {
    chip_8::cpu emulator("Chip-8 emulator");
    
    if (argc == 2) {
        emulator.load_program(std::string(argv[1]));
    }
    else {
        std::cout << "Invalid argv[1]\n";
        return -1;
    }

    
    emulator.run();

    return 0;
}
