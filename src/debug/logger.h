#ifdef DEBUG

#ifndef LOGGER_H
#define LOGGER_H

#include "../core/cpu.h"

#define LOG_INIT(a, b, c) Logger::init(a, b, c)
#define LOG() Logger::get_instance().log_all()
#define LOG_CPU_REGS() Logger::get_instance().log_cpu_regs()
#define LOG_CURR_STACK(a, b) Logger::get_instance().log_curr_stack(a, b) 
#define LOG_CURR_INSTR(a, b) Logger::get_instance().log_curr_instr(a, b) 

class CPU;

class Logger {
public:
    static Logger& init(const CPU& cpu_in, const MMU& mmu_in, const GPU& gpu_in);
    static Logger& get_instance();
    void log_all() const;
    void log_cpu_regs() const;
    void log_curr_stack(int before, int after) const;
    void log_curr_instr(int before, int after) const;
private:
    Logger(const CPU& cpu_in, const MMU& mmu_in, const GPU& gpu_in);

    const CPU& cpu;
    const MMU& mmu;
    const GPU& gpu;
};

#endif

#else
#define LOG_INIT(a, b, c) (void) 0
#define LOG() (void) 0
#define LOG_CPU_REGS() (void) 0
#define LOG_CURR_STACK(a, b) (void) 0
#define LOG_CURR_INSTR(a, b) (void) 0
#endif

