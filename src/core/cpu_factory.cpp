#include "core/cpu_factory.h"

#include "core/icpu.h"
#include "core/cpu.h"

#include <memory>

std::unique_ptr<ICpu> create_cpu(IMmu& mmu, ICpu::Registers& regs) {
    return std::make_unique<CPU>(mmu, regs); 
}
