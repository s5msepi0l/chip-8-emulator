# CHIP-8 Emulator

Chip 8 implementation i wrote over the weekend, enjoy :)

## Features

- Implements the full CHIP-8 instruction set.
- Supports standard CHIP-8 programs.
- Simple command-line interface.
- Renders graphics using a SDL 2

## prerequisites


```bash
sudo apt install libsdl2-dev -y
```

or <a href="https://github.com/libsdl-org/SDL/releases"> install SDL2 manually</a> and link manually

## Usage

To run the emulator, provide the path to a CHIP-8 program as an argument:

```bash
g++ -lSDL2 -o chip8
./chip8 /path/to/program.ch8
```

## Note
This has only been tested on window 11 w MSCV c++