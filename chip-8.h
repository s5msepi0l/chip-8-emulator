#pragma once

#include <iostream>
#include <vector>
#include <cstdint>
#include <ctime>
#include <iomanip>

#include <Windows.h>
#include <SDL.h>

#define clr() system("cls")
#define LOG_NIB(nibble) printf("0x%X", nibble)
#define LOG_BYTE(byte) printf("0x%02X", byte)


#define GET_NIB(instruction, index) (((instruction) >> ((3 - (index)) * 4)) & 0xF)

//get last 3 nibs, usually address pointer
#define GET_NIB3(instruction) GET_NIB(instruction, 1) | \
	GET_NIB(instruction, 2) |							   \
	GET_NIB(instruction, 3)


#define N_SYS_MEMORY 4096
#define N_STACK		 16
#define N_V_REGISTER 16
#define PROG_START  0x200

#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32
#define SCALE_FACTOR  16

namespace chip_8 {
	class memory {
	public:
		
	private:
		//not that it makes this any more safe lol
		uint8_t* sys_memory;

	public:
		memory(uint8_t *init_mem) {
			sys_memory = new uint8_t[N_SYS_MEMORY];
		
			for (int i = 0; i < PROG_START; i++) {
				sys_memory[i] = init_mem[i];
			}
		}

		memory() {
			sys_memory = new uint8_t[N_SYS_MEMORY];
			for (size_t i = 0; i < N_SYS_MEMORY; i++)
				sys_memory[i] = 0;
		}

		~memory() {
			delete sys_memory;
		}

		uint8_t *operator [](int index) {
			
			/* first 512 bytes is considered rom and is usually
			* just system fonts 
			*/
			return &sys_memory[index];

		}
	};

	class keyboard_input {
	public:
		bool keys[16] = { false };
		bool quit = false;

		void update() {
			SDL_PumpEvents();

			keys[0x1] = is_key_down(SDL_SCANCODE_1);
			keys[0x2] = is_key_down(SDL_SCANCODE_2);
			keys[0x3] = is_key_down(SDL_SCANCODE_3);
			keys[0xC] = is_key_down(SDL_SCANCODE_4);
			keys[0x4] = is_key_down(SDL_SCANCODE_Q);
			keys[0x5] = is_key_down(SDL_SCANCODE_W);
			keys[0x6] = is_key_down(SDL_SCANCODE_E);
			keys[0xD] = is_key_down(SDL_SCANCODE_R);
			keys[0x7] = is_key_down(SDL_SCANCODE_A);
			keys[0x8] = is_key_down(SDL_SCANCODE_S);
			keys[0x9] = is_key_down(SDL_SCANCODE_D);
			keys[0xE] = is_key_down(SDL_SCANCODE_F);
			keys[0xA] = is_key_down(SDL_SCANCODE_Z);
			keys[0x0] = is_key_down(SDL_SCANCODE_X);
			keys[0xB] = is_key_down(SDL_SCANCODE_C);
			keys[0xF] = is_key_down(SDL_SCANCODE_V);

			quit	  = is_key_down(SDL_SCANCODE_ESCAPE);
		}

		bool is_key_down(SDL_Scancode scancode) {
			const Uint8* state = SDL_GetKeyboardState(NULL);
			return state[scancode] != 0; // Return true if the key is pressed
		}
	};

	//modified version of screen class used in other projects
	class screen {
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;

		bool pixel_state[SCREEN_WIDTH][SCREEN_HEIGHT]{ {false} };

	public:
		screen(const char* name, uint32_t width = SCREEN_WIDTH * SCALE_FACTOR, uint32_t height = SCREEN_HEIGHT * SCALE_FACTOR) {
			if (SDL_Init(SDL_INIT_VIDEO) != 0) {
				std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
				throw std::runtime_error("SDL Initialization failed");
			}

			window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
			if (!window) {
				std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
				SDL_Quit();
				throw std::runtime_error("Window creation failed");
			}

			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
			if (!renderer) {
				std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
				SDL_DestroyWindow(window);
				SDL_Quit();
				throw std::runtime_error("Renderer creation failed");
			}
		}

		void render_start() {
			SDL_RenderClear(renderer);
		}

		void render_stop() {
			for (uint8_t i = 0; i < SCREEN_WIDTH; i++) {
				for (uint8_t j = 0; j < SCREEN_HEIGHT; j++) {
					if (pixel_state) {
						this->draw_pixel(i, j);
					}
				}
			}

			SDL_RenderPresent(renderer);
		}

		inline void pixel_edit(uint8_t r, uint8_t g, uint8_t b) {
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
		}

		void draw_pixel(uint8_t x, uint8_t y) {
			if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
				SDL_Rect pixelRect = {
					static_cast<int>(x * SCALE_FACTOR),
					static_cast<int>(y * SCALE_FACTOR),
					SCALE_FACTOR,  
					SCALE_FACTOR   
				};
				SDL_RenderFillRect(renderer, &pixelRect);
			}
		}

		// Bleh :P, muchos (slow)
		bool set_pixel(uint8_t x, uint8_t y) {
			if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
				if (pixel_state[x][y]) {
					pixel_state[x][y] = false;
					return true;
				}
				pixel_state[x][y] = true;
				return false;
			}

			return false;
		}

		bool draw_sprite(uint8_t x, uint8_t y, std::vector<uint8_t>& sprite, uint8_t n) {
			bool pixel_overwritten = false;

			for (uint8_t row = 0; row < n; ++row) {
				uint8_t sprite_row = sprite[row];
				for (uint8_t col = 0; col < 8; ++col) {
					if ((sprite_row & (0x80 >> col)) != 0) {
						uint8_t pixel_x = (x + col) % SCREEN_WIDTH;
						uint8_t pixel_y = (y + row) % SCREEN_HEIGHT;

						if (set_pixel(pixel_x, pixel_y)) {
							pixel_overwritten = true;
						}

						draw_pixel(pixel_x, pixel_x);
					}
				}
			}

			return pixel_overwritten;
		}

		void clear() {
			SDL_RenderClear(renderer);

			for (int i = 0; i < SCREEN_WIDTH; ++i) {
				for (int j = 0; j < SCREEN_HEIGHT; ++j) {
					pixel_state[i][j] = false;
				}
			}
		}

		~screen() {
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(window);
			SDL_Quit();
		}
	};

	class cpu {
	public:
		// general purpose registers, V0-VF
		uint8_t V[N_V_REGISTER]{ 0 };
		
		uint16_t stack[N_STACK]{ 0 };

		uint16_t PC = PROG_START; //program counter

		uint8_t SP = 0; // stack pointer
		uint16_t I = 0; // index register
		uint8_t DT = 0; // delay timer

		memory sys_memory;
		screen display;
		keyboard_input input;

		bool running = true;

		cpu(const char *name): display(name), sys_memory() {
			std::srand(std::time(nullptr));
		}

		void load(const std::vector<uint8_t> program) {
			for (size_t i = 0; i < program.size(); i++) {
				*sys_memory[PROG_START + i] = program[i];
			}
		}

		void run() {
			while (running) {
				clr();
				display.render_start();

				cycle();
				log_status();

				display.render_stop();
				SDL_Delay(1000 * 5);

			}

			

		}

		void log_status() {
			std::cout << "----------- CHIP-8 Registers -----------\n";

			std::cout << "General Purpose Registers (V0 - VF):\n";
			for (int i = 0; i < N_V_REGISTER; ++i) {
				std::cout << "V" << std::hex << std::uppercase << i
					<< ": " << std::setw(2) << std::setfill('0') << (int)V[i] << std::endl;
			}

			std::cout << "\nStack (Top to Bottom):\n";
			for (int i = 0; i < N_STACK; ++i) {
				std::cout << "Stack[" << std::hex << std::uppercase << i
					<< "]: " << std::setw(4) << std::setfill('0') << stack[i] << std::endl;
			}

			std::cout << "\nSpecial Registers:\n";
			std::cout << "PC (Program Counter): " << std::setw(4) << std::setfill('0') << std::hex << PC << std::endl;
			std::cout << "SP (Stack Pointer): " << std::hex << (int)SP << std::endl;
			std::cout << "I (Index Register): " << std::setw(4) << std::setfill('0') << I << std::endl;
			std::cout << "DT (Delay Timer): " << std::hex << (int)DT << std::endl;

			std::cout << "----------------------------------------\n";
		}


	private:
		void cycle() {
			uint16_t instruction = (*sys_memory[PC] << 8) | *sys_memory[PC + 1];
			PC += 2;

			uint8_t opcode = GET_NIB(instruction, 0);
			std::cout << "current opcode: " << LOG_BYTE(instruction) << std::endl;
			switch (opcode) {

			case 0x0:
				
				switch (instruction) {
				case 0x00E0: // clear screen
					clr();

					break;

					/* return from function
					* set PC to top of stack
					* decrement stack pointer
					*/
				case 0x00EE:
					if (SP > 0) {
						PC = stack[SP - 1];
						SP--;

					}
					else { //stack is empty unable to return, halt execution

					}

					break;
				}

				break;

				// jump to NNN
			case 0x1: { 
				uint8_t jmp = instruction & 0xFFF;
				std::cout << "JMP: " << static_cast<int>( jmp) << std::endl;

				PC = jmp;

				break;
			}
				// call function
			case 0x2:
				if (SP < 16) {
					stack[SP++] = PC;
					PC = GET_NIB3(instruction);

				}
				else { //stack overflow, halt execution

				}

				break;

			case 0x3: {
				uint8_t Vx = GET_NIB(instruction, 1);
				if (
					(GET_NIB(instruction, 2) | GET_NIB(instruction, 3)
						!= V[Vx])) {
					PC += 2; //skip next instruction
				}

				break;
			}

			case 0x4:
				if (
					((GET_NIB(instruction, 2) | GET_NIB(instruction, 3))
						== V[GET_NIB(instruction, 1)])) {
					PC += 2; //skip next instruction
				}

				break;

			case 0x5: {
				uint8_t Vx = GET_NIB(instruction, 1);
				uint8_t Vy = GET_NIB(instruction, 2);
				if (Vx > 16 && Vy > 16) { // invalid register, halt

				}

				if (V[Vx] == V[Vy]) PC += 2;

				break;
			}

			case 0x6: {
				uint8_t Vx = GET_NIB(instruction, 1);
				if (Vx > 16) { // invalid register, halt

				}

				V[Vx] = GET_NIB(instruction, 2) | GET_NIB(instruction, 3);

				break;
			}

			case 0x7: {

				uint8_t Vx = GET_NIB(instruction, 1);
				if (Vx > 16) { // invalid register, halt

				}

				uint16_t carry = V[Vx] + GET_NIB(instruction, 2) | GET_NIB(instruction, 3);

				if (carry > 0xFF) V[0xF] = 1;
				else              V[0xF] = 0;

				V[Vx] = carry & 0xFF;

				break;
			}

			case 0x8: {
				switch (GET_NIB(instruction, 3)) {
				case 0x0: // =
					V[GET_NIB(instruction, 1)] = V[GET_NIB(instruction, 2)];
					break;

				case 0x1: // |=
					V[GET_NIB(instruction, 1)] |= V[GET_NIB(instruction, 2)];
					break;

				case 0x2: // &=
					V[GET_NIB(instruction, 1)] &= V[GET_NIB(instruction, 2)];
					break;

				case 0x3: // ^=
					V[GET_NIB(instruction, 1)] ^= V[GET_NIB(instruction, 2)];
					break;

				case 0x4: //+=
					V[GET_NIB(instruction, 1)] += V[GET_NIB(instruction, 2)];
					break;

				case 0x5: // -=
					V[GET_NIB(instruction, 1)] -= V[GET_NIB(instruction, 2)];
					break;

				case 0x6:// >>=
					V[GET_NIB(instruction, 1)] >>= V[GET_NIB(instruction, 2)];
					break;

				case 0x7:
					V[GET_NIB(instruction, 1)] = \
						V[GET_NIB(instruction, 2)] - V[GET_NIB(instruction, 1)];

					break;

				case 0xE:

					V[GET_NIB(instruction, 1)] <<= V[GET_NIB(instruction, 2)];
					break;
				}
				break;
			}

				case 0x9:
					if (V[GET_NIB(instruction, 1)] != V[GET_NIB(instruction, 2)]) {
						PC += 2;
					}
					break;

				case 0xA:
					I = GET_NIB3(instruction);
					break;

				case 0xB:
					PC = V[0] + GET_NIB3(instruction);
					break;

				case 0xC:
					V[GET_NIB(instruction, 1)] = (std::rand() % 255) & (
						GET_NIB(instruction, 2) | GET_NIB(instruction, 3));
					break;

				case 0xD: {
					uint8_t Vx, Vy, n;
					Vx = GET_NIB(instruction, 1);
					Vy = GET_NIB(instruction, 2);
					n = GET_NIB(instruction, 3);

					uint8_t x = V[Vx];
					uint8_t y = V[Vy];

					for (uint8_t row = 0; row<n; row++) {
						uint8_t sprite_byte = *sys_memory[I + row];
						for (uint8_t col = 0; col < 8; col++) {
							if ((sprite_byte >> (7 - col)) & 1) {
								if (display.set_pixel(x + col, y + row))
									V[0xF] = 1; // set collision flag that pixel was erased (collision)
							}
						}
					}

					break;
				}

				case 0xE:
					switch (GET_NIB(instruction, 3)) {
						case 0xE:
							break;
						
						case 0x1:
							break;
					}
			}
		}

	};
}
