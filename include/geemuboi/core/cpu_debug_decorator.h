#pragma once

#include "core/icpu.h"
#include "core/immu.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

class CpuDebugDecorator : public ICpu {
public:
    CpuDebugDecorator(
        std::unique_ptr<ICpu> cpu_in, 
        IMmu& mmu_in,
        const ICpu::Registers& regs_in,
        const std::unordered_set<uint16_t>& breakpoints_in);

    virtual int execute();
    virtual unsigned get_cycles_executed();
private:
    void print_breakpoint() const;
    void print_cpu_context() const;
    void print_stack(int before, int after) const;
    void print_current_instruction(int before, int after) const;

    std::unique_ptr<ICpu> cpu;
    IMmu& mmu;
    const ICpu::Registers& regs;
    const std::unordered_set<uint16_t>& breakpoints;
    std::vector<std::string> instruction_names;
};
