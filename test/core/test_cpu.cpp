#include "gtest/gtest.h"

#include "core/gpu.h"
#include "core/icpu.h"
#include "core/cpu_factory.h"

#include <memory>
#include <unordered_set>

#include "core/mock_mmu.h"
#include "view/mock_renderer.h"

#include "core/cpu.h"

using ::testing::_;
using ::testing::Return;


class CpuTest : public ::testing::Test {
protected:
    CpuTest() : mmu{}, regs{}, cpu{create_cpu(mmu, regs)} {}

    void execute_instruction(uint8_t instr) {
        EXPECT_CALL(mmu, read_byte(_)).WillOnce(Return(instr));
        cpu->execute();
    }

    void verify_state_changes(const ICpu::Registers& expected_regs) {
        EXPECT_EQ(regs.a, expected_regs.a);
        EXPECT_EQ(regs.b, expected_regs.b);
        EXPECT_EQ(regs.c, expected_regs.c);
        EXPECT_EQ(regs.d, expected_regs.d);
        EXPECT_EQ(regs.e, expected_regs.e);
        EXPECT_EQ(regs.h, expected_regs.h);
        EXPECT_EQ(regs.l, expected_regs.l);
        EXPECT_EQ(regs.f, expected_regs.f);
        EXPECT_EQ(regs.pc, expected_regs.pc);
        EXPECT_EQ(regs.sp, expected_regs.sp);
    }

    MockMmu mmu;
    ICpu::Registers regs;
    std::unique_ptr<ICpu> cpu;
};


TEST_F(CpuTest, nop) {
    execute_instruction(0x00);

    ICpu::Registers expected_regs{};
    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 1);
}


TEST_F(CpuTest, ld_bc_d16) {
    EXPECT_CALL(mmu, read_word(regs.pc + 1)).WillOnce(Return(0x3344));
    execute_instruction(0x01);

    ICpu::Registers expected_regs{};
    expected_regs.b = 0x33;
    expected_regs.c = 0x44;
    expected_regs.pc = 3;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 3);
}
