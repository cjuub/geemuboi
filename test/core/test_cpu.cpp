#include "gtest/gtest.h"

#include "core/gpu.h"
#include "core/icpu.h"
#include "core/cpu_factory.h"

#include <memory>
#include <unordered_set>

#include "core/mock_mmu.h"
#include "view/mock_renderer.h"

namespace geemuboi::test::core {

using namespace geemuboi::core;

using ::testing::_;
using ::testing::Return;


class CpuTest : public ::testing::Test {
protected:
    CpuTest() : mmu{}, regs{}, cpu{create_cpu(mmu, regs)} {}

    void execute_instruction(uint8_t instr) {
        EXPECT_CALL(mmu, read_byte(regs.pc)).WillOnce(Return(instr));
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

    uint16_t make_addr(uint8_t high, uint8_t low) {
        return (high << 8) + low;
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

TEST_F(CpuTest, ld_mbc_a) {
    regs.a = 0xFE;
    regs.b = 0x33;
    regs.c = 0x44;

    EXPECT_CALL(mmu, write_byte(make_addr(regs.b, regs.c), regs.a));
    execute_instruction(0x02);

    ICpu::Registers expected_regs{};
    expected_regs.a = 0xFE;
    expected_regs.b = 0x33;
    expected_regs.c = 0x44;
    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 2);
}

TEST_F(CpuTest, inc_bc_low) {
    execute_instruction(0x3);

    ICpu::Registers expected_regs{};
    expected_regs.c = 1;
    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 2);
}

TEST_F(CpuTest, inc_bc_low_high) {
    regs.c = 0xFF;

    execute_instruction(0x3);

    ICpu::Registers expected_regs{};
    expected_regs.b = 1;
    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 2);
}

TEST_F(CpuTest, inc_bc_low_overflow) {
    regs.b = 0xFF;
    regs.c = 0xFF;

    execute_instruction(0x3);

    ICpu::Registers expected_regs{};
    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 2);
}

TEST_F(CpuTest, inc_b) {
    regs.f = ICpu::N_FLAG;

    execute_instruction(0x4);

    ICpu::Registers expected_regs{};
    expected_regs.b = 1;
    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 1);
}

TEST_F(CpuTest, inc_b_zero_and_half_carry) {
    regs.b = 0xFF;
    regs.f = ICpu::N_FLAG;

    execute_instruction(0x4);

    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::Z_FLAG | ICpu::H_FLAG;
    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 1);
}

TEST_F(CpuTest, dec_b) {
    regs.b = 5;
    regs.f = ICpu::C_FLAG;

    execute_instruction(0x5);

    ICpu::Registers expected_regs{};
    expected_regs.b = 4;
    expected_regs.f = ICpu::N_FLAG | ICpu::C_FLAG;

    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 1);
}

TEST_F(CpuTest, dec_b_half_carry) {
    execute_instruction(0x5);

    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;
    expected_regs.f = ICpu::N_FLAG | ICpu::H_FLAG;
    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 1);
}

TEST_F(CpuTest, dec_b_zero) {
    regs.b = 1;

    execute_instruction(0x5);

    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::N_FLAG | ICpu::Z_FLAG;
    expected_regs.pc = 1;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 1);
}

TEST_F(CpuTest, ld_b_d8) {
    regs.f = ICpu::C_FLAG | ICpu::N_FLAG | ICpu::H_FLAG | ICpu::Z_FLAG;

    EXPECT_CALL(mmu, read_byte(regs.pc + 1)).WillOnce(Return(0x33));

    execute_instruction(0x6);

    ICpu::Registers expected_regs{};
    expected_regs.b = 0x33;
    expected_regs.f = ICpu::C_FLAG | ICpu::N_FLAG | ICpu::H_FLAG | ICpu::Z_FLAG;
    expected_regs.pc = 2;
    verify_state_changes(expected_regs);

    EXPECT_EQ(cpu->get_cycles_executed(), 2);
}

// TEST_F(CpuTest, rlca) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_ma16_sp) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_hl_bc) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_mbc) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_bc) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_c_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rrca) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, stop) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_de_d16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mde_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_de) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_d_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rla) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jr_r8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_hl_de) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_mde) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_de) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_e_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rra) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jr_nz_r8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_hl_d16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ldi_mhl_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_hl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_h_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, daa) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jr_z_r8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_hl_hl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ldi_a_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_hl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_l_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cpl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jr_nc_r8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_sp_d16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ldd_mhl_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_sp) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mhl_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, scf) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jr_c_r8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_hl_sp) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ldd_a_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_sp) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, inc_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, dec_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ccf) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_b_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_b_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_b_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_b_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_b_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_b_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_b_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_b_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_c_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_c_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_c_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_c_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_c_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_c_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_c_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_c_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_d_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_d_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_d_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_d_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_d_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_d_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_d_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_d_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_e_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_e_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_e_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_e_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_e_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_e_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_e_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_e_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_h_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_h_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_h_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_h_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_h_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_h_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_h_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_h_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_l_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_l_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_l_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_l_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_l_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_l_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_l_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_l_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mhl_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mhl_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mhl_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mhl_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mhl_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mhl_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, halt) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mhl_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_a_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_a_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_a_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_a_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_a_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_a_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_a_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_a_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, adc_a_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, adc_a_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, adc_a_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, adc_a_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, adc_a_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, adc_a_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, adc_a_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, adc_a_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sub_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sub_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sub_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sub_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sub_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sub_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sub_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sub_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sbc_a_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sbc_a_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sbc_a_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sbc_a_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sbc_a_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sbc_a_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sbc_a_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sbc_a_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, and_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, and_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, and_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, and_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, and_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, and_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, and_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, and_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, xor_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, xor_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, xor_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, xor_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, xor_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, xor_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, xor_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, xor_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, or_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, or_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, or_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, or_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, or_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, or_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, or_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, or_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cp_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cp_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cp_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cp_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cp_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cp_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cp_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cp_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ret_nz) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, pop_bc) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jp_nz_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jp_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, call_nz_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, push_bc) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_a_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rst_00h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ret_z) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ret) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jp_z_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, prefix_cb) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, call_z_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, call_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, adc_a_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rst_08h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ret_nc) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, pop_de) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jp_nc_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, call_nc_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, push_de) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sub_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rst_10h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ret_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, reti) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jp_c_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, call_c_a16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sbc_a_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rst_18h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ldh_ma8_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, pop_hl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_mc_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, push_hl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, and_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rst_20h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, add_sp_r8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, jp_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_ma16_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, xor_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rst_28h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ldh_a_ma8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, pop_af) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_mc) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, di) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, push_af) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, or_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rst_30h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ldhl_sp_r8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_sp_hl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ld_a_ma16) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, ei) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, unimplemented) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, cp_d8) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rst_38) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rlc_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rlc_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rlc_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rlc_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rlc_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rlc_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rlc_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rlc_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rrc_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rrc_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rrc_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rrc_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rrc_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rrc_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rrc_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rrc_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rl_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rl_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rl_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rl_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rl_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rl_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rl_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rl_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rr_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rr_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rr_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rr_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rr_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rr_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rr_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, rr_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sla_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sla_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sla_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sla_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sla_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sla_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sla_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sla_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sra_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sra_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sra_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sra_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sra_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sra_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sra_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, sra_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, swap_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, swap_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, swap_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, swap_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, swap_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, swap_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, swap_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, swap_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, srl_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, srl_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, srl_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, srl_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, srl_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, srl_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, srl_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, srl_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_0_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_0_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_0_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_0_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_0_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_0_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_0_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_0_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_1_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_1_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_1_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_1_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_1_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_1_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_1_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_1_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_2_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_2_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_2_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_2_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_2_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_2_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_2_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_2_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_3_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_3_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_3_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_3_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_3_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_3_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_3_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_3_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_4_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_4_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_4_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_4_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_4_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_4_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_4_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_4_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_5_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_5_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_5_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_5_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_5_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_5_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_5_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_5_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_6_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_6_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_6_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_6_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_6_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_6_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_6_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_6_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_7_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_7_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_7_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_7_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_7_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_7_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_7_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, bit_7_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_0_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_0_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_0_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_0_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_0_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_0_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_0_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_0_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_1_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_1_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_1_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_1_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_1_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_1_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_1_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_1_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_2_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_2_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_2_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_2_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_2_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_2_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_2_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_2_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_3_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_3_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_3_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_3_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_3_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_3_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_3_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_3_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_4_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_4_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_4_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_4_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_4_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_4_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_4_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_4_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_5_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_5_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_5_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_5_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_5_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_5_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_5_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_5_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_6_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_6_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_6_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_6_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_6_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_6_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_6_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_6_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_7_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_7_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_7_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_7_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_7_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_7_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_7_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, res_7_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_0_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_0_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_0_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_0_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_0_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_0_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_0_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_0_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_1_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_1_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_1_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_1_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_1_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_1_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_1_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_1_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_2_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_2_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_2_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_2_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_2_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_2_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_2_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_2_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_3_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_3_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_3_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_3_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_3_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_3_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_3_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_3_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_4_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_4_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_4_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_4_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_4_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_4_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_4_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_4_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_5_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_5_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_5_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_5_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_5_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_5_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_5_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_5_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_6_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_6_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_6_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_6_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_6_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_6_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_6_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_6_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_7_b) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_7_c) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_7_d) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_7_e) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_7_h) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_7_l) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_7_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

// TEST_F(CpuTest, set_7_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }


}
