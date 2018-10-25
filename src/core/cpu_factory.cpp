#include "core/cpu_factory.h"

#include "core/icpu.h"

#include "cpu.h"

namespace geemuboi::core {


std::unique_ptr<ICpu> create_cpu(IMmu& mmu, ICpu::Registers& regs) {
    return std::make_unique<CPU>(mmu, regs);
}


}
