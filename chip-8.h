#pragma once

#include <iostream>
#include <vector>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <fstream>

#include <Windows.h>
#include <SDL.h>

#define clr() system("cls")
#define LOG_NIB(nibble) printf("0x%X", nibble)
#define LOG_BYTE(byte) printf("0x%02X", byte)


#define GET_NIB(instruction, index) (((instruction) >> ((3 - (index)) * 4)) & 0xF)

//get last 3 nibs, usually address pointer
#define GET_NIB3(instruction) ((GET_NIB(instruction, 1) << 8) | \
                               (GET_NIB(instruction, 2) << 4) | \
                               (GET_NIB(instruction, 3)))

#define N_SYS_MEMORY  4096
#define N_STACK		  16
#define N_V_REGISTER  16
#define PROG_START    0x200

#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32
#define SCALE_FACTOR  16

#define N_KEYS		  16

#ifndef CYCLE_RATE
#define CYCLE_RATE 16 //ms

#endif

uint8_t font_set[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


namespace chip_8 {
	class memory {
	public:
		
	private:
		//not that it makes this any more safe lol
		uint8_t* sys_memory;

	public:

		//where does .data begin and .text end
		uint32_t data_pos = 0;

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
		bool keys[N_KEYS] = { false };
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

		// If something is using 40% of the cpu this is probably it :3
		void io_block_update() {
			SDL_Event event;

			bool keyPressed = false;
			while (!keyPressed) {
				SDL_WaitEvent(&event);

				if (event.type == SDL_KEYDOWN) {
					SDL_Keysym keysym = event.key.keysym;
					switch (keysym.sym) {
					case SDLK_1:
						keys[0x1] = true;
						keyPressed = true;
						break;
					case SDLK_2:
						keys[0x2] = true;
						keyPressed = true;
						break;
					case SDLK_3:
						keys[0x3] = true;
						keyPressed = true;
						break;
					case SDLK_4:
						keys[0xC] = true;
						keyPressed = true;
						break;
					case SDLK_q:
						keys[0x4] = true;
						keyPressed = true;
						break;
					case SDLK_w:
						keys[0x5] = true;
						keyPressed = true;
						break;
					case SDLK_e:
						keys[0x6] = true;
						keyPressed = true;
						break;
					case SDLK_r:
						keys[0xD] = true;
						keyPressed = true;
						break;
					case SDLK_a:
						keys[0x7] = true;
						keyPressed = true;
						break;
					case SDLK_s:
						keys[0x8] = true;
						keyPressed = true;
						break;
					case SDLK_d:
						keys[0x9] = true;
						keyPressed = true;
						break;
					case SDLK_f:
						keys[0xE] = true;
						keyPressed = true;
						break;
					case SDLK_z:
						keys[0xA] = true;
						keyPressed = true;
						break;
					case SDLK_x:
						keys[0x0] = true;
						keyPressed = true;
						break;
					case SDLK_c:
						keys[0xB] = true;
						keyPressed = true;
						break;
					case SDLK_v:
						keys[0xF] = true;
						keyPressed = true;
						break;
					case SDLK_ESCAPE:
						quit = true;
						keyPressed = true;
						break;
					default:
						break;
					}
				}
			}
		}


		bool is_key_down(SDL_Scancode scancode) {
			const uint8_t* state = SDL_GetKeyboardState(NULL);
			return state[scancode] != 0; // Return true if the key is pressed
		}
	};

	//modified version of screen class used in other projects
	class screen {
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;

		bool pixel_state[SCREEN_WIDTH][SCREEN_HEIGHT]{ {false} };  // Array to track pixel set state

	public:
		// Constructor: Initialize SDL, create window, and renderer
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

		// Start rendering (clear screen)
		void render_start() {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
		}

		// Stop rendering (present the updated screen)
		void render_stop() {
			pixel_edit(255, 255, 255);
			for (uint8_t i = 0; i < SCREEN_WIDTH; i++) {
				for (uint8_t j = 0; j < SCREEN_HEIGHT; j++) {
					if (pixel_state[i][j]) {
						this->draw_pixel(i, j);

					}
				}
			}

			SDL_RenderPresent(renderer);
		}

		// Edit a pixel color using r, g, b values and a default alpha of 255
		inline void pixel_edit(uint8_t r, uint8_t g, uint8_t b) {
			SDL_SetRenderDrawColor(renderer, r, g, b, 255);
		}

		// Draw a pixel on the scaled window (treating it as a 64x32 grid)
		void draw_pixel(uint8_t x, uint8_t y) {
			if (x < SCREEN_WIDTH && y < SCREEN_HEIGHT) {
				// Scale the coordinates by the scale factor (16x)
				SDL_Rect pixelRect = {
					static_cast<int>(x * SCALE_FACTOR),  // Scaled x coordinate
					static_cast<int>(y * SCALE_FACTOR),  // Scaled y coordinate
					SCALE_FACTOR,  // Width of the pixel block
					SCALE_FACTOR   // Height of the pixel block
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

		// Draw the sprite at the given (X, Y) position
		bool draw_sprite(uint8_t x, uint8_t y, std::vector<uint8_t>& sprite, uint8_t n) {
			bool pixelOverwritten = false;  // Flag to track if any pixel was overwritten

			for (uint8_t row = 0; row < n; ++row) {
				uint8_t spriteRow = sprite[row];
				for (uint8_t col = 0; col < 8; ++col) {
					if ((spriteRow & (0x80 >> col)) != 0) {  // If the pixel is set in the sprite
						uint8_t pixelX = (x + col) % SCREEN_WIDTH;
						uint8_t pixelY = (y + row) % SCREEN_HEIGHT;

						// Check if the pixel is being overwritten
						if (set_pixel(pixelX, pixelY)) {
							// Pixel was flipped (set to 0 after being 1), so set pixelOverwritten flag
							pixelOverwritten = true;
						}

						// Draw the pixel as set (typically white)
						draw_pixel(pixelX, pixelY);
					}
				}
			}

			return pixelOverwritten;  // Return whether any pixel was overwritten
		}

		// Clear the screen (reset to black and reset pixelState)
		void clear() {
			SDL_RenderClear(renderer);

			// Reset the pixelState array
			for (int i = 0; i < SCREEN_WIDTH; ++i) {
				for (int j = 0; j < SCREEN_HEIGHT; ++j) {
					pixel_state[i][j] = false;
				}
			}
		}

		// Destructor: Clean up SDL resources
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
		uint8_t ST = 0; // sound timer

		memory sys_memory;
		screen display;
		keyboard_input input;

		bool running = true;

		cpu(const char *name): display(name), sys_memory() {
			std::srand(std::time(nullptr));

			load_fonts();
		}

		void load(const std::vector<uint8_t> program) {
			for (size_t i = 0; i < program.size(); i++) {
				*sys_memory[PROG_START + i] = program[i];
			}
		}

		void load_text(const std::vector<uint16_t>& program) {
			if (PROG_START + program.size() * 2 > N_SYS_MEMORY) {
				std::cerr << "Error: Not enough space to load instructions into memory!" << std::endl;
				return;
			}

			sys_memory.data_pos = PROG_START + (program.size() * 2) + 1;
			for (size_t i = 0; i < program.size(); ++i) {
				uint16_t instruction = program[i];

				uint8_t high_byte= (instruction >> 8) & 0xFF;
				uint8_t low_byte= instruction & 0xFF;

				*sys_memory[PROG_START + i * 2] = high_byte;
				*sys_memory[PROG_START + i * 2 + 1] = low_byte;
			}
		}

		void load_data(const std::vector<uint8_t>& data, uint32_t data_p) {
			if (data_p != 0) {
				sys_memory.data_pos = data_p;
			}

			for (uint32_t i = 0; i < data.size(); i++) {
				*sys_memory[sys_memory.data_pos + i] = data[i];
			}
		}

		void load_program(const std::string& filename) {
			std::ifstream file(filename, std::ios::binary);

			if (!file) {
				throw std::runtime_error("Failed to open the file: " + filename);
			}
			
			file.seekg(0, std::ios::end);
			size_t file_size = file.tellg();
			file.seekg(0, std::ios::beg);

			if (file_size > (4096 - 0x200)) {
				throw std::runtime_error("Program size exceeds memory capacity");
			}

			file.read(reinterpret_cast<char*>(sys_memory[0x200]), file_size);

			if (!file) {
				throw std::runtime_error("Error reading the file into memory");
			}

			file.close();
		}

		void load_fonts() {
			//first 80 bytes usually reserved for internal interpreter
			for (int i = 0; i < 80; ++i) {
				*sys_memory[0x050 + i] = font_set[i];
			}
		}


		void run() {
			while (running && !input.quit) {
				clr();
				display.render_start();
				input.update();
				

				cycle();

				if (DT > 0) DT--;

				log_status();

				display.render_stop();
				SDL_Delay(CYCLE_RATE);

			}

			
			
			/*
			display.render_start();

			display.set_pixel(32, 16);
			display.set_pixel(31, 16);

			display.render_stop();
			
			SDL_Delay(10000);
			*/
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
				uint16_t jmp = GET_NIB3(instruction);
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

			// "6XNN" set register X to NN
			case 0x6: {
				uint8_t Vx = GET_NIB(instruction, 1);
				if (Vx > 16) { // invalid register, halt

				}

				V[Vx] = GET_NIB(instruction, 2) | GET_NIB(instruction, 3);

				break;
			}

			// "7XNN" add NN to register X
			case 0x7: {

				uint8_t Vx = GET_NIB(instruction, 1);
				if (Vx > 16) { // invalid register, halt

				}
				V[Vx] += (GET_NIB(instruction, 2) << 4) | GET_NIB(instruction, 3);

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

						std::cout << "draw(" << static_cast<int>(Vx) << ", " <<
							static_cast<int>(Vy) << ")\n";

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
							if (input.keys[V[GET_NIB(instruction, 1)]])
								PC += 2;

							break;
						
						case 0x1:
							if (!input.keys[V[GET_NIB(instruction, 1)]])
								PC += 2;
							
							break;
					}

					break;

				case 0xF:
					switch ((GET_NIB(instruction, 2) << 4) | GET_NIB(instruction, 3)) {
						case 0x07:
							V[GET_NIB(instruction, 1)] = DT;

							break;

						case 0x15:
							DT = V[GET_NIB(instruction, 1)];
							break;
						
						case 0x18:
							ST = V[GET_NIB(instruction, 1)];
							break;

						// "FX0A" | block IO until keyboard is pressed, stored in Vx
						case 0x0A:
							input.io_block_update();
						
							for (uint32_t i = 0; i < N_KEYS; i++) {
								if (input.keys[i]) {
									V[GET_NIB(instruction, 1)] = i;
								}
							}

							break;

						case 0x1E:
							I += V[GET_NIB(instruction, 1)];

							break;

						// Set index register to the location for sprite[Vx]
						case 0x29:
							I = 0x050 + (V[GET_NIB(instruction, 1)] * 5);
							break;

						case 0x33: {
							const uint16_t reg_val = V[GET_NIB(instruction, 1)];
							
							*sys_memory[I]	   = reg_val / 100;
							*sys_memory[I + 1] = (reg_val / 10) % 10;
							*sys_memory[I + 2] = reg_val % 10;

							break;
						}

						case 0x55: {
							for (uint32_t i = 0; i < GET_NIB(instruction, 1); i++) {
								*sys_memory[I + i] = V[i];
							}
							
							break;
						}

						case 0x65: {
							for (uint32_t i = 0; i < GET_NIB(instruction, 1); i++) {
								V[i] = *sys_memory[I + i];
							}

							break;
						}
					}

					break;
			}
		}

	};
}
