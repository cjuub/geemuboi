#include "../core/cpu.h"
#include "../core/mmu.h"
#include "../core/gpu.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

#include <SDL2/SDL.h>

const unsigned CYCLES_PER_FRAME = 70224;

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

    time_t start_time = time(0);
    unsigned frames = 0;
    bool run = true;
    while (run) {
        int frame_cycles = 0;
        while (frame_cycles <= CYCLES_PER_FRAME) {
            int cycles = cpu.execute();
            gpu.step(cycles);
            frame_cycles += cycles;
        }

        ++frames;
        if (difftime(time(0), start_time) >= 1) {
            start_time = time(0);
            std::cout << std::dec << frames << std::endl;
            frames = 0;
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
        }
    }

    SDL_Quit();
}
