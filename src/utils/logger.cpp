#ifndef NDEBUG

#include "utils/logger.h"

#include <iostream>

using namespace std;

Logger::Logger(CPU& cpu_in,
               const IMmu& mmu_in, 
               const GPU& gpu_in,
               unordered_set<uint16_t>& breakpoints_in) : 
    cpu(cpu_in),
    mmu(mmu_in),
    gpu(gpu_in),
    breakpoints(breakpoints_in),
    prev_addr(0xFFFF)
{}

Logger& Logger::init(CPU& cpu_in, 
                     const IMmu& mmu_in, 
                     const GPU& gpu_in,
                     unordered_set<uint16_t>& breakpoints_in) {
    static Logger logger(cpu_in, mmu_in, gpu_in, breakpoints_in);
    return logger;
}

Logger& Logger::get_instance() {
    CPU* cpu = nullptr;
    IMmu* mmu = nullptr;
    GPU* gpu = nullptr;
    unordered_set<uint16_t>* breakpoints = nullptr;

    return Logger::init(*cpu, *mmu, *gpu, *breakpoints);
}

void Logger::log_all() const {
    log_cpu_regs();
    log_curr_stack(-5, 5);
    log_curr_instr(-5, 5);
}

void Logger::log_cpu_regs() const {
    cout << "-------- CPU --------" << endl;
    cout << cpu.print_context();
}

void Logger::log_curr_stack(int before, int after) const {
    cout << "------- STACK -------" << endl;
    cout << cpu.print_stack(before, after);
}

void Logger::log_curr_instr(int before, int after) const {
    cout << "------- INSTR -------" << endl;
    cout << cpu.print_curr_instr(before, after);
}

void Logger::log_breakpoint(uint16_t addr) {
    if (breakpoints.find(addr) != breakpoints.end()) {
        cout << "------- BREAK -------" << endl;
        log_all();
        cout << "Press enter to continue..." << endl;
        cin.get();
    }
}

#endif