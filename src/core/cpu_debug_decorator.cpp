#include "core/cpu_debug_decorator.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <iomanip>

namespace geemuboi::core  {


CpuDebugDecorator::CpuDebugDecorator(
    std::unique_ptr<ICpu> cpu_in,
    IMmu& mmu_in,
    const ICpu::Registers& regs_in,
    const std::unordered_set<uint16_t>& breakpoints_in) 
        : cpu{std::move(cpu_in)}, 
          mmu{mmu_in}, 
          real_regs{regs_in},
          breakpoints{breakpoints_in}, 
          instruction_names{},
          regs{} {
    std::ifstream ifs("instr.txt");

    std::string str;
    while (ifs >> str) {
        instruction_names.push_back(str);
    }
}


int CpuDebugDecorator::execute() {
    regs = real_regs;

    if (breakpoints.find(regs.pc) != breakpoints.end()) {
        print_breakpoint();
    }

    std::string next_instruction{instruction_names[mmu.read_byte(regs.pc)]};
    
    try {
        return cpu->execute();
    } catch (const NotImplementedInstructionException& e) {
        std::cout << e.what() << " Instruction: " << next_instruction << std::endl;
    } catch (const UndefinedInstructionException& e) {
        std::cout << e.what() << " Instruction: " << next_instruction << std::endl;
    } catch (const NotImplementedMemoryRegionException& e) {
        std::cout << e.what() << " " << e.get_region_name() << " 0x" << std::hex
                  << e.get_address() << " " << e.get_access() << std::endl;
    }

    return 0;
}


unsigned CpuDebugDecorator::get_cycles_executed() {
    return cpu->get_cycles_executed();
}


void CpuDebugDecorator::print_breakpoint() const {
    std::cout << "------- BREAK -------" << std::endl;
    std::cout << "-------- CPU --------\n";
    print_cpu_context();
    std::cout << "------- STACK -------\n";
    print_stack(-5, 5);
    std::cout << "------- INSTR -------\n";
    print_current_instruction(-5, 5);
    std::cout << "Press any key to continue..." << std::endl;
    std::cin.get();
}


void CpuDebugDecorator::print_cpu_context() const {
    std::cout << std::hex << std::setfill('0');


    std::cout << "A: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.a) << " ";
    std::cout << "F: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.f) << " ";
    std::cout << "PC: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.pc) << "\n";

    std::cout << "B: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.b) << " ";
    std::cout << "C: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.c) << " ";
    std::cout << "SP: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.sp) << "\n";

    std::cout << "D: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.d) << " ";
    std::cout << "E: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.e) << "\n";

    std::cout << "H: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.h) << " ";
    std::cout << "L: 0x" << std::hex << std::setw(4) << static_cast<unsigned>(regs.l) << "\n";
}


void CpuDebugDecorator::print_stack(int before, int after) const {
    std::cout << std::hex;
    before = (regs.sp + before < 0) ? -regs.sp : before;
    after = (regs.sp + after > 0xFFFF) ? 0xFFFF - regs.sp : after;
    for (int i = before; i != after; ++i) {
        std::cout << ((i == 0) ? ">" : " ");
        std::cout << "0x" << static_cast<unsigned>(regs.sp + i * 2) << ": ";
        std::cout << "0x" << static_cast<unsigned>(mmu.read_word(regs.sp + i * 2)) << std::endl;
    }
}


void CpuDebugDecorator::print_current_instruction(int before, int after) const {
    std::cout << std::hex;
    before = (regs.pc + before < 0) ? -regs.pc : before;
    after = (regs.pc + after > 0xFFFF) ? 0xFFFF - regs.pc : after;
    for (int i = before; i != after; ++i) {
        std::cout << ((i == 0) ? ">" : " ");
        std::cout << "0x" << static_cast<unsigned>(regs.pc + i) << ": ";
        std::cout << instruction_names[mmu.read_byte(regs.pc + i)] << std::endl;
    }
}


}
