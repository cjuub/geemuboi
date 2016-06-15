#include "../core/cpu.h"
#include "../core/mmu.h"
#include "../core/gpu.h"
#include "../video/sdl_renderer.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <thread>

#include <SDL2/SDL.h>

const int MILLIS_PER_FRAME = 1000 / 60;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Invalid arguments." << std::endl;
        exit(0);
    }

    std::string bios = argv[1];
    std::string rom = argv[2];

    SDLRenderer rend;
    SDL_Event event;

    GPU gpu(rend);
    MMU mmu(gpu, bios, rom);
    CPU cpu(mmu);

    time_t start_time = time(0);
    unsigned frames = 0;
    bool run = true;
    while (run) {
        int frame_cycles = 0;
        time_t frame_start_time = time(0);
        while (frame_cycles <= GPU::CYCLES_PER_FRAME) {
            int cycles = cpu.execute();
            gpu.step(cycles);
            frame_cycles += cycles;
        }

        ++frames;

        double frame_time = difftime(time(0), frame_start_time) * 1000;
        if (frame_time < MILLIS_PER_FRAME) {
            int time_to_sleep = static_cast<int>(MILLIS_PER_FRAME - frame_time);
            std::this_thread::sleep_for(std::chrono::milliseconds(time_to_sleep));
        }

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

