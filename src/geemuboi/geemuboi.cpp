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

const double MILLIS_PER_FRAME = 1000 / 60;

int main(int argc, char* argv[]) {
    using namespace std::chrono;

    if (argc != 3) {
        std::cout << "Invalid arguments." << std::endl;
        exit(0);
    }

    std::string bios = argv[1];
    std::string rom = argv[2];

    SDLRenderer renderer;
    SDL_Event event;

    GPU gpu(renderer);
    MMU mmu(gpu, bios, rom);
    CPU cpu(mmu);

    high_resolution_clock clock;
    auto start_time = clock.now();

    int frames = 0;
    bool run = true;
    while (run) {
        int frame_cycles = 0;
        auto frame_start_time = clock.now();
        while (frame_cycles <= GPU::CYCLES_PER_FRAME) {
            int cycles = cpu.execute();
            gpu.step(cycles);
            frame_cycles += cycles;
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                run = false;
            }
        }

        ++frames;

        if (duration_cast<milliseconds>(clock.now() - start_time).count() >= 1000) {
            start_time = clock.now();
            renderer.update_fps_indicator(frames);
            frames = 0;
        }

        auto frame_time = duration_cast<milliseconds>(clock.now() - frame_start_time);
        if (frame_time.count() < MILLIS_PER_FRAME) {
            int time_to_sleep = MILLIS_PER_FRAME - frame_time.count();
            std::this_thread::sleep_for(milliseconds(time_to_sleep));
        }
    } 

    SDL_Quit();
}

