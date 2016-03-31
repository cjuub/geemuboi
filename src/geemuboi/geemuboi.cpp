#include "../core/cpu.h"
#include "../core/mmu.h"
#include "../core/gpu.h"

#include <iostream>
#include <fstream>
#include <string>

#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "Invalid arguments." << std::endl;
		exit(0);
	}

	std::string bios = argv[1];
	std::string rom = argv[2];

	GPU gpu;
	MMU mmu(gpu, bios, rom);
	CPU cpu(mmu);

	SDL_Event event;
	bool run = true;
	while (run) {
		int cycles = cpu.execute();
		gpu.step(cycles);

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				run = false;
			}
		}
	}

	// SDL_DestroyWindow( window );
	SDL_Quit();
}