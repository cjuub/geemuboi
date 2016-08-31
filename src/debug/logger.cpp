#include "logger.h"

#include <iostream>

using namespace std;

Logger::Logger(const CPU& cpu_in, const MMU& mmu_in, const GPU& gpu_in) : 
    cpu(cpu_in),
    mmu(mmu_in),
    gpu(gpu_in)
{}

Logger& Logger::init(const CPU& cpu_in, const MMU& mmu_in, const GPU& gpu_in) {
    static Logger logger(cpu_in, mmu_in, gpu_in);
    return logger;
}

Logger& Logger::get_instance() {
    CPU* cpu = nullptr;
    MMU* mmu = nullptr;
    GPU* gpu = nullptr;

    return Logger::init(*cpu, *mmu, *gpu);
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

