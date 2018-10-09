#ifndef LOGGER_H
#define LOGGER_H

#ifndef NDEBUG
#include "../core/cpu.h"
#include "../core/mmu.h"
#include "../core/gpu.h"

#include <cstdio>
#include <unordered_set>

#define LOG_INIT(a, b, c, d) Logger::init(a, b, c, d)
#define LOG(...) printf(__VA_ARGS__)
#define LOG_ALL() Logger::get_instance().log_all()
#define LOG_CPU_REGS() Logger::get_instance().log_cpu_regs()
#define LOG_CURR_STACK(a, b) Logger::get_instance().log_curr_stack(a, b) 
#define LOG_CURR_INSTR(a, b) Logger::get_instance().log_curr_instr(a, b) 
#define LOG_BREAKPOINT(a) Logger::get_instance().log_breakpoint(a);

class CPU;

class Logger {
public:
    static Logger& init(CPU& cpu_in, 
                        const MMU& mmu_in,
                        const GPU& gpu_in,
                        std::unordered_set<uint16_t>& breakpoints_in);
    static Logger& get_instance();

    void log_all() const;
    void log_cpu_regs() const;
    void log_curr_stack(int before, int after) const;
    void log_curr_instr(int before, int after) const;
    void log_breakpoint(uint16_t addr);
private:
    Logger(CPU& cpu_in,
           const MMU& mmu_in,
           const GPU& gpu_in,
           std::unordered_set<uint16_t>& breakpoints_in);

    CPU& cpu;
    const MMU& mmu;
    const GPU& gpu;
    std::unordered_set<uint16_t>& breakpoints;
    uint16_t prev_addr;
};

#else
#define LOG_INIT(a, b, c, d) (void) 0
#define LOG(...) (void) 0
#define LOG_ALL() (void) 0
#define LOG_CPU_REGS() (void) 0
#define LOG_CURR_STACK(a, b) (void) 0
#define LOG_CURR_INSTR(a, b) (void) 0
#define LOG_BREAKPOINT(a) (void) 0
#endif

#endif