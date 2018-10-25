#pragma once

#include "core/icpu.h"
#include "core/immu.h"

#include <memory>

namespace geemuboi::core {


std::unique_ptr<ICpu> create_cpu(IMmu& mmu, ICpu::Registers& regs);


}
