#include "../core/cpu.h"
#include "../core/mmu.h"
#include "../core/gpu.h"
#include "../core/input.h"
#include "../video/sdl_renderer.h"
#include "../input/sdl_keyboard.h"
#include "../debug/logger.h"
#include "argument_handler.h"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_set>

#include <SDL2/SDL.h>

const double MILLIS_PER_FRAME = 1000 / 60;

int main(int argc, char* argv[]) {
    using namespace std::chrono;

    ArgumentHandler args(argc - 1, argv + 1);

    SDLRenderer renderer;
    SDL_Event event;

    GPU gpu(renderer);
    Input input;
    MMU mmu(gpu, input, args.get_bios(), args.get_rom());
    CPU cpu(mmu);

    LOG_INIT(cpu, mmu, gpu, args.get_breakpoints());

    SDLKeyboard joypad(input);

    high_resolution_clock clock;
    auto start_time = clock.now();

    int frames = 0;
    bool run = true;
    while (run) {
        int frame_cycles = 0;
        auto frame_start_time = clock.now();
        while (frame_cycles <= GPU::CYCLES_PER_FRAME) {
            int cycles = cpu.execute();

            if (cycles < 0) {
                while (!SDL_PollEvent(&event)) {
                    std::this_thread::sleep_for(milliseconds(1000));
                }

                cpu.resume();
                continue;
            }

            gpu.step(cycles);
            frame_cycles += cycles;
        }

        while (SDL_PollEvent(&event)) {
            joypad.update_button_presses();
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

