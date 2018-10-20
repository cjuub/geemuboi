#include "core/cpu.h"
#include "core/mmu.h"
#include "core/gpu.h"
#include "core/input.h"
#include "view/sdl_renderer.h"
#include "input/sdl_keyboard.h"
#include "utils/logger.h"

#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_set>

#include <SDL2/SDL.h>
#include <args.hxx>

const double MILLIS_PER_FRAME = 1000 / 60;

int main(int argc, char* argv[]) {
    using namespace std::chrono;

    args::ArgumentParser parser("This is a test program.", "This goes after the options.");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Positional<std::string> bios(parser, "BIOS", "The GameBoy BIOS ROM.");
    args::Positional<std::string> rom(parser, "ROM", "A GameBoy ROM.");
    args::ValueFlagList<std::string> breakpoints(parser, "breakpoint", "A breakpoint address.", {"b"});

    try {
        parser.ParseCLI(argc, argv);
    } catch (args::Help&) {
        std::cout << parser;
        return 0;
    } catch (args::ParseError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    } catch (const args::ValidationError& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    if (!bios) {
        std::cout << "Missing BIOS argument" << std::endl;
        return 1;
    }

    if (!rom) {
        std::cout << "Missing ROM argument" << std::endl;
        return 1;
    }

    std::unordered_set<uint16_t> bps;
    if (breakpoints) {
        for (const auto& breakpoint : args::get(breakpoints)) {
            uint16_t bp = static_cast<uint16_t>(std::stoul(breakpoint, nullptr, 16));
            bps.insert(bp);
        }
    }

    SDLRenderer renderer;
    SDL_Event event;

    GPU gpu(renderer);
    Input input;
    MMU mmu(gpu, input, args::get(bios), args::get(rom)); 
    CPU cpu(mmu);

    LOG_INIT(cpu, mmu, gpu, bps);

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
