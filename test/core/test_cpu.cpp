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

    void verify_ld_r16_d16(uint8_t instruction, ICpu::Registers& expected_regs) {
        regs.f = ICpu::C_FLAG | ICpu::N_FLAG | ICpu::H_FLAG | ICpu::Z_FLAG;

        EXPECT_CALL(mmu, read_word(regs.pc + 1)).WillOnce(Return(0x3344));
        execute_instruction(instruction);

        expected_regs.f = ICpu::C_FLAG | ICpu::N_FLAG | ICpu::H_FLAG | ICpu::Z_FLAG;
        expected_regs.pc = 3;
        verify_state_changes(expected_regs);

        EXPECT_EQ(cpu->get_cycles_executed(), 3);
    }

    void verify_inc_dec_reg8(uint8_t instruction, ICpu::Registers& expected_regs) {
        regs.f |= ICpu::C_FLAG;

        execute_instruction(instruction);

        expected_regs.pc = 1;
        expected_regs.f |= ICpu::C_FLAG;
        verify_state_changes(expected_regs);

        EXPECT_EQ(cpu->get_cycles_executed(), 1);
    }

    void verify_inc_dec_reg16(uint8_t instruction, ICpu::Registers& expected_regs) {
        regs.f = ICpu::C_FLAG | ICpu::N_FLAG | ICpu::H_FLAG | ICpu::Z_FLAG;

        execute_instruction(instruction);

        expected_regs.pc = 1;
        expected_regs.f = ICpu::C_FLAG | ICpu::N_FLAG | ICpu::H_FLAG | ICpu::Z_FLAG;
        verify_state_changes(expected_regs);

        EXPECT_EQ(cpu->get_cycles_executed(), 2);
    }

    void verify_ld_r8_r8(uint8_t instruction, ICpu::Registers& expected_regs) {
        regs.f = ICpu::C_FLAG | ICpu::N_FLAG | ICpu::H_FLAG | ICpu::Z_FLAG;

        execute_instruction(instruction);

        expected_regs.f = ICpu::C_FLAG | ICpu::N_FLAG | ICpu::H_FLAG | ICpu::Z_FLAG;
        expected_regs.pc = 1;
        verify_state_changes(expected_regs);

        EXPECT_EQ(cpu->get_cycles_executed(), 1);
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
    ICpu::Registers expected_regs{};
    expected_regs.b = 0x33;
    expected_regs.c = 0x44;
    verify_ld_r16_d16(0x01, expected_regs);
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
    ICpu::Registers expected_regs{};
    expected_regs.c = 1;
    verify_inc_dec_reg16(0x03, expected_regs);
}

TEST_F(CpuTest, inc_bc_low_high) {
    regs.c = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.b = 1;
    verify_inc_dec_reg16(0x03, expected_regs);
}

TEST_F(CpuTest, inc_bc_low_overflow) {
    regs.b = 0xFF;
    regs.c = 0xFF;

    ICpu::Registers expected_regs{};
    verify_inc_dec_reg16(0x03, expected_regs);
}

TEST_F(CpuTest, inc_b) {
    regs.f = ICpu::N_FLAG;
    ICpu::Registers expected_regs{};
    expected_regs.b = 1;
    verify_inc_dec_reg8(0x04, expected_regs);
}

TEST_F(CpuTest, inc_b_zero_and_half_carry) {
    regs.f = ICpu::N_FLAG;
    regs.b = 0xFF;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::Z_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x04, expected_regs);
}

TEST_F(CpuTest, dec_b) {
    regs.b = 5;

    ICpu::Registers expected_regs{};
    expected_regs.b = 4;
    expected_regs.f = ICpu::N_FLAG;
    verify_inc_dec_reg8(0x05, expected_regs);
}

TEST_F(CpuTest, dec_b_half_carry) {
    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;
    expected_regs.f = ICpu::N_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x05, expected_regs);
}

TEST_F(CpuTest, dec_b_zero) {
    regs.b = 1;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::N_FLAG | ICpu::Z_FLAG;
    verify_inc_dec_reg8(0x05, expected_regs);
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

TEST_F(CpuTest, dec_bc) {
    regs.c = 2;
    ICpu::Registers expected_regs{};
    expected_regs.c = 0x1;
    verify_inc_dec_reg16(0x0B, expected_regs);
}

TEST_F(CpuTest, dec_bc_underflow) {
    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;
    expected_regs.c = 0xFF;
    verify_inc_dec_reg16(0x0B, expected_regs);
}

TEST_F(CpuTest, dec_bc_half_underflow) {
    regs.b = 0x01;
    ICpu::Registers expected_regs{};
    expected_regs.c = 0xFF;
    verify_inc_dec_reg16(0x0B, expected_regs);
}

TEST_F(CpuTest, inc_c) {
    regs.f = ICpu::N_FLAG;
    ICpu::Registers expected_regs{};
    expected_regs.c = 1;
    verify_inc_dec_reg8(0x0C, expected_regs);
}

TEST_F(CpuTest, inc_c_zero_and_half_carry) {
    regs.f = ICpu::N_FLAG;
    regs.c = 0xFF;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::Z_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x0C, expected_regs);
}

TEST_F(CpuTest, dec_c) {
    regs.c = 5;

    ICpu::Registers expected_regs{};
    expected_regs.c = 4;
    expected_regs.f = ICpu::N_FLAG;
    verify_inc_dec_reg8(0x0D, expected_regs);
}

TEST_F(CpuTest, dec_c_half_carry) {
    ICpu::Registers expected_regs{};
    expected_regs.c = 0xFF;
    expected_regs.f = ICpu::N_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x0D, expected_regs);
}

TEST_F(CpuTest, dec_c_zero) {
    regs.c = 1;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::N_FLAG | ICpu::Z_FLAG;
    verify_inc_dec_reg8(0x0D, expected_regs);
}

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

TEST_F(CpuTest, ld_de_d16) {
    ICpu::Registers expected_regs{};
    expected_regs.d = 0x33;
    expected_regs.e = 0x44;
    verify_ld_r16_d16(0x11, expected_regs);
}

// TEST_F(CpuTest, ld_mde_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

TEST_F(CpuTest, inc_de_low) {
    ICpu::Registers expected_regs{};
    expected_regs.e = 1;
    verify_inc_dec_reg16(0x13, expected_regs);
}

TEST_F(CpuTest, inc_de_low_high) {
    regs.e = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.d = 1;
    verify_inc_dec_reg16(0x13, expected_regs);
}

TEST_F(CpuTest, inc_de_low_overflow) {
    regs.d = 0xFF;
    regs.e = 0xFF;

    ICpu::Registers expected_regs{};
    verify_inc_dec_reg16(0x13, expected_regs);
}

TEST_F(CpuTest, inc_d) {
    regs.f = ICpu::N_FLAG;
    ICpu::Registers expected_regs{};
    expected_regs.d = 1;
    verify_inc_dec_reg8(0x14, expected_regs);
}

TEST_F(CpuTest, inc_d_zero_and_half_carry) {
    regs.f = ICpu::N_FLAG;
    regs.d = 0xFF;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::Z_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x14, expected_regs);
}

TEST_F(CpuTest, dec_d) {
    regs.d = 5;

    ICpu::Registers expected_regs{};
    expected_regs.d = 4;
    expected_regs.f = ICpu::N_FLAG;
    verify_inc_dec_reg8(0x15, expected_regs);
}

TEST_F(CpuTest, dec_d_half_carry) {
    ICpu::Registers expected_regs{};
    expected_regs.d = 0xFF;
    expected_regs.f = ICpu::N_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x15, expected_regs);
}

TEST_F(CpuTest, dec_d_zero) {
    regs.d = 1;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::N_FLAG | ICpu::Z_FLAG;
    verify_inc_dec_reg8(0x15, expected_regs);
}

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

TEST_F(CpuTest, dec_de) {
    regs.e = 2;
    ICpu::Registers expected_regs{};
    expected_regs.e = 0x1;
    verify_inc_dec_reg16(0x1B, expected_regs);
}

TEST_F(CpuTest, dec_de_underflow) {
    ICpu::Registers expected_regs{};
    expected_regs.d = 0xFF;
    expected_regs.e = 0xFF;
    verify_inc_dec_reg16(0x1B, expected_regs);
}

TEST_F(CpuTest, dec_de_half_underflow) {
    regs.d = 0x01;
    ICpu::Registers expected_regs{};
    expected_regs.e = 0xFF;
    verify_inc_dec_reg16(0x1B, expected_regs);
}

TEST_F(CpuTest, inc_e) {
    regs.f = ICpu::N_FLAG;
    ICpu::Registers expected_regs{};
    expected_regs.e = 1;
    verify_inc_dec_reg8(0x1C, expected_regs);
}

TEST_F(CpuTest, inc_e_zero_and_half_carry) {
    regs.f = ICpu::N_FLAG;
    regs.e = 0xFF;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::Z_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x1C, expected_regs);
}

TEST_F(CpuTest, dec_e) {
    regs.e = 5;

    ICpu::Registers expected_regs{};
    expected_regs.e = 4;
    expected_regs.f = ICpu::N_FLAG;
    verify_inc_dec_reg8(0x1D, expected_regs);
}

TEST_F(CpuTest, dec_e_half_carry) {
    ICpu::Registers expected_regs{};
    expected_regs.e = 0xFF;
    expected_regs.f = ICpu::N_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x1D, expected_regs);
}

TEST_F(CpuTest, dec_e_zero) {
    regs.e = 1;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::N_FLAG | ICpu::Z_FLAG;
    verify_inc_dec_reg8(0x1D, expected_regs);
}

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

TEST_F(CpuTest, ld_hl_d16) {
    ICpu::Registers expected_regs{};
    expected_regs.h = 0x33;
    expected_regs.l = 0x44;
    verify_ld_r16_d16(0x21, expected_regs);
}

// TEST_F(CpuTest, ldi_mhl_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

TEST_F(CpuTest, inc_hl_low) {
    ICpu::Registers expected_regs{};
    expected_regs.l = 1;
    verify_inc_dec_reg16(0x23, expected_regs);
}

TEST_F(CpuTest, inc_hl_low_high) {
    regs.l = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.h = 1;
    verify_inc_dec_reg16(0x23, expected_regs);
}

TEST_F(CpuTest, inc_hl_low_overflow) {
    regs.h = 0xFF;
    regs.l = 0xFF;

    ICpu::Registers expected_regs{};
    verify_inc_dec_reg16(0x23, expected_regs);
}

TEST_F(CpuTest, inc_h) {
    regs.f = ICpu::N_FLAG;
    ICpu::Registers expected_regs{};
    expected_regs.h = 1;
    verify_inc_dec_reg8(0x24, expected_regs);
}

TEST_F(CpuTest, inc_h_zero_and_half_carry) {
    regs.f = ICpu::N_FLAG;
    regs.h = 0xFF;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::Z_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x24, expected_regs);
}

TEST_F(CpuTest, dec_h) {
    regs.h = 5;

    ICpu::Registers expected_regs{};
    expected_regs.h = 4;
    expected_regs.f = ICpu::N_FLAG;
    verify_inc_dec_reg8(0x25, expected_regs);
}

TEST_F(CpuTest, dec_h_half_carry) {
    ICpu::Registers expected_regs{};
    expected_regs.h = 0xFF;
    expected_regs.f = ICpu::N_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x25, expected_regs);
}

TEST_F(CpuTest, dec_h_zero) {
    regs.h = 1;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::N_FLAG | ICpu::Z_FLAG;
    verify_inc_dec_reg8(0x25, expected_regs);
}

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

TEST_F(CpuTest, dec_hl) {
    regs.l = 2;
    ICpu::Registers expected_regs{};
    expected_regs.l = 0x1;
    verify_inc_dec_reg16(0x2B, expected_regs);
}

TEST_F(CpuTest, dec_hl_underflow) {
    ICpu::Registers expected_regs{};
    expected_regs.h = 0xFF;
    expected_regs.l = 0xFF;
    verify_inc_dec_reg16(0x2B, expected_regs);
}

TEST_F(CpuTest, dec_hl_half_underflow) {
    regs.h = 0x01;
    ICpu::Registers expected_regs{};
    expected_regs.l = 0xFF;
    verify_inc_dec_reg16(0x2B, expected_regs);
}

TEST_F(CpuTest, inc_l) {
    regs.f = ICpu::N_FLAG;
    ICpu::Registers expected_regs{};
    expected_regs.l = 1;
    verify_inc_dec_reg8(0x2C, expected_regs);
}

TEST_F(CpuTest, inc_l_zero_and_half_carry) {
    regs.f = ICpu::N_FLAG;
    regs.l = 0xFF;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::Z_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x2C, expected_regs);
}

TEST_F(CpuTest, dec_l) {
    regs.l = 5;

    ICpu::Registers expected_regs{};
    expected_regs.l = 4;
    expected_regs.f = ICpu::N_FLAG;
    verify_inc_dec_reg8(0x2D, expected_regs);
}

TEST_F(CpuTest, dec_l_half_carry) {
    ICpu::Registers expected_regs{};
    expected_regs.l = 0xFF;
    expected_regs.f = ICpu::N_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x2D, expected_regs);
}

TEST_F(CpuTest, dec_l_zero) {
    regs.l = 1;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::N_FLAG | ICpu::Z_FLAG;
    verify_inc_dec_reg8(0x2D, expected_regs);
}

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

TEST_F(CpuTest, ld_sp_d16) {
    ICpu::Registers expected_regs{};
    expected_regs.sp = 0x3344;
    verify_ld_r16_d16(0x31, expected_regs);
}

// TEST_F(CpuTest, ldd_mhl_a) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

TEST_F(CpuTest, inc_sp) {
    ICpu::Registers expected_regs{};
    expected_regs.sp = 1;
    verify_inc_dec_reg16(0x33, expected_regs);
}

TEST_F(CpuTest, inc_sp_overflow) {
    regs.sp = 0xFFFF;
    ICpu::Registers expected_regs{};
    verify_inc_dec_reg16(0x33, expected_regs);
}

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

TEST_F(CpuTest, dec_sp) {
    regs.sp = 2;
    ICpu::Registers expected_regs{};
    expected_regs.sp = 1;
    verify_inc_dec_reg16(0x3B, expected_regs);
}

TEST_F(CpuTest, dec_sp_underflow) {
    ICpu::Registers expected_regs{};
    expected_regs.sp = 0xFFFF;
    verify_inc_dec_reg16(0x3B, expected_regs);
}

TEST_F(CpuTest, inc_a) {
    regs.f = ICpu::N_FLAG;
    ICpu::Registers expected_regs{};
    expected_regs.a = 1;
    verify_inc_dec_reg8(0x3C, expected_regs);
}

TEST_F(CpuTest, inc_a_zero_and_half_carry) {
    regs.f = ICpu::N_FLAG;
    regs.a = 0xFF;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::Z_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x3C, expected_regs);
}

TEST_F(CpuTest, dec_a) {
    regs.a = 5;

    ICpu::Registers expected_regs{};
    expected_regs.a = 4;
    expected_regs.f = ICpu::N_FLAG;
    verify_inc_dec_reg8(0x3D, expected_regs);
}

TEST_F(CpuTest, dec_a_half_carry) {
    ICpu::Registers expected_regs{};
    expected_regs.a = 0xFF;
    expected_regs.f = ICpu::N_FLAG | ICpu::H_FLAG;
    verify_inc_dec_reg8(0x3D, expected_regs);
}

TEST_F(CpuTest, dec_a_zero) {
    regs.a = 1;
    ICpu::Registers expected_regs{};
    expected_regs.f = ICpu::N_FLAG | ICpu::Z_FLAG;
    verify_inc_dec_reg8(0x3D, expected_regs);
}

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


// TEST_F(CpuTest, ld_r8_r8) {
    // r1_list = [];
// }

TEST_F(CpuTest, ld_b_b) {
    regs.b = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;

    verify_ld_r8_r8(0x40, expected_regs);
}

TEST_F(CpuTest, ld_b_c) {
    regs.c = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;
    expected_regs.c = 0xFF;

    verify_ld_r8_r8(0x41, expected_regs);
}

TEST_F(CpuTest, ld_b_d) {
    regs.d = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;
    expected_regs.d = 0xFF;

    verify_ld_r8_r8(0x42, expected_regs);
}

TEST_F(CpuTest, ld_b_e) {
    regs.e = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;
    expected_regs.e = 0xFF;

    verify_ld_r8_r8(0x43, expected_regs);
}

TEST_F(CpuTest, ld_b_h) {
    regs.h = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;
    expected_regs.h = 0xFF;

    verify_ld_r8_r8(0x44, expected_regs);
}

TEST_F(CpuTest, ld_b_l) {
    regs.l = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;
    expected_regs.l = 0xFF;

    verify_ld_r8_r8(0x45, expected_regs);
}

// TEST_F(CpuTest, ld_b_mhl) {
// }

TEST_F(CpuTest, ld_b_a) {
    regs.a = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.b = 0xFF;
    expected_regs.a = 0xFF;

    verify_ld_r8_r8(0x47, expected_regs);
}

TEST_F(CpuTest, ld_c_b) {
    regs.b = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.c = 0xFF;
    expected_regs.b = 0xFF;

    verify_ld_r8_r8(0x48, expected_regs);
}

TEST_F(CpuTest, ld_c_c) {
    regs.c = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.c = 0xFF;

    verify_ld_r8_r8(0x49, expected_regs);
}

TEST_F(CpuTest, ld_c_d) {
    regs.d = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.c = 0xFF;
    expected_regs.d = 0xFF;

    verify_ld_r8_r8(0x4A, expected_regs);
}

TEST_F(CpuTest, ld_c_e) {
    regs.e = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.c = 0xFF;
    expected_regs.e = 0xFF;

    verify_ld_r8_r8(0x4B, expected_regs);
}

TEST_F(CpuTest, ld_c_h) {
    regs.h = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.c = 0xFF;
    expected_regs.h = 0xFF;

    verify_ld_r8_r8(0x4C, expected_regs);
}

TEST_F(CpuTest, ld_c_l) {
    regs.l = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.c = 0xFF;
    expected_regs.l = 0xFF;

    verify_ld_r8_r8(0x4D, expected_regs);
}

// TEST_F(CpuTest, ld_c_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

TEST_F(CpuTest, ld_c_a) {
    regs.a = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.c = 0xFF;
    expected_regs.a = 0xFF;

    verify_ld_r8_r8(0x4F, expected_regs);
}

TEST_F(CpuTest, ld_d_b) {
    regs.b = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.d = 0xFF;
    expected_regs.b = 0xFF;

    verify_ld_r8_r8(0x50, expected_regs);
}

TEST_F(CpuTest, ld_d_c) {
    regs.c = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.d = 0xFF;
    expected_regs.c = 0xFF;

    verify_ld_r8_r8(0x51, expected_regs);
}

TEST_F(CpuTest, ld_d_d) {
    regs.d = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.d = 0xFF;

    verify_ld_r8_r8(0x52, expected_regs);
}

TEST_F(CpuTest, ld_d_e) {
    regs.e = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.d = 0xFF;
    expected_regs.e = 0xFF;

    verify_ld_r8_r8(0x53, expected_regs);
}

TEST_F(CpuTest, ld_d_h) {
    regs.h = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.d = 0xFF;
    expected_regs.h = 0xFF;

    verify_ld_r8_r8(0x54, expected_regs);
}

TEST_F(CpuTest, ld_d_l) {
    regs.l = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.d = 0xFF;
    expected_regs.l = 0xFF;

    verify_ld_r8_r8(0x55, expected_regs);
}

// TEST_F(CpuTest, ld_d_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

TEST_F(CpuTest, ld_d_a) {
    regs.a = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.d = 0xFF;
    expected_regs.a = 0xFF;

    verify_ld_r8_r8(0x57, expected_regs);
}

TEST_F(CpuTest, ld_e_b) {
    regs.b = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.e = 0xFF;
    expected_regs.b = 0xFF;

    verify_ld_r8_r8(0x58, expected_regs);
}

TEST_F(CpuTest, ld_e_c) {
    regs.c = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.e = 0xFF;
    expected_regs.c = 0xFF;

    verify_ld_r8_r8(0x59, expected_regs);
}

TEST_F(CpuTest, ld_e_d) {
    regs.d = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.e = 0xFF;
    expected_regs.d = 0xFF;

    verify_ld_r8_r8(0x5A, expected_regs);
}

TEST_F(CpuTest, ld_e_e) {
    regs.e = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.e = 0xFF;

    verify_ld_r8_r8(0x5B, expected_regs);
}

TEST_F(CpuTest, ld_e_h) {
    regs.h = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.e = 0xFF;
    expected_regs.h = 0xFF;

    verify_ld_r8_r8(0x5C, expected_regs);
}

TEST_F(CpuTest, ld_e_l) {
    regs.l = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.e = 0xFF;
    expected_regs.l = 0xFF;

    verify_ld_r8_r8(0x5D, expected_regs);
}

// TEST_F(CpuTest, ld_e_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

TEST_F(CpuTest, ld_e_a) {
    regs.a = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.e = 0xFF;
    expected_regs.a = 0xFF;

    verify_ld_r8_r8(0x5F, expected_regs);
}

TEST_F(CpuTest, ld_h_b) {
    regs.b = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.h = 0xFF;
    expected_regs.b = 0xFF;

    verify_ld_r8_r8(0x60, expected_regs);
}

TEST_F(CpuTest, ld_h_c) {
    regs.c = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.h = 0xFF;
    expected_regs.c = 0xFF;

    verify_ld_r8_r8(0x61, expected_regs);
}

TEST_F(CpuTest, ld_h_d) {
    regs.d = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.h = 0xFF;
    expected_regs.d = 0xFF;

    verify_ld_r8_r8(0x62, expected_regs);
}

TEST_F(CpuTest, ld_h_e) {
    regs.e = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.h = 0xFF;
    expected_regs.e = 0xFF;

    verify_ld_r8_r8(0x63, expected_regs);
}

TEST_F(CpuTest, ld_h_h) {
    regs.h = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.h = 0xFF;

    verify_ld_r8_r8(0x64, expected_regs);
}

TEST_F(CpuTest, ld_h_l) {
    regs.l = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.h = 0xFF;
    expected_regs.l = 0xFF;

    verify_ld_r8_r8(0x65, expected_regs);
}

// TEST_F(CpuTest, ld_h_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

TEST_F(CpuTest, ld_h_a) {
    regs.a = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.h = 0xFF;
    expected_regs.a = 0xFF;

    verify_ld_r8_r8(0x67, expected_regs);
}

TEST_F(CpuTest, ld_l_b) {
    regs.b = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.l = 0xFF;
    expected_regs.b = 0xFF;

    verify_ld_r8_r8(0x68, expected_regs);
}

TEST_F(CpuTest, ld_l_c) {
    regs.c = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.l = 0xFF;
    expected_regs.c = 0xFF;

    verify_ld_r8_r8(0x69, expected_regs);
}

TEST_F(CpuTest, ld_l_d) {
    regs.d = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.l = 0xFF;
    expected_regs.d = 0xFF;

    verify_ld_r8_r8(0x6A, expected_regs);
}

TEST_F(CpuTest, ld_l_e) {
    regs.e = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.l = 0xFF;
    expected_regs.e = 0xFF;

    verify_ld_r8_r8(0x6B, expected_regs);
}

TEST_F(CpuTest, ld_l_h) {
    regs.h = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.l = 0xFF;
    expected_regs.h = 0xFF;

    verify_ld_r8_r8(0x6C, expected_regs);
}

TEST_F(CpuTest, ld_l_l) {
    regs.l = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.l = 0xFF;

    verify_ld_r8_r8(0x6D, expected_regs);
}

// TEST_F(CpuTest, ld_l_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

TEST_F(CpuTest, ld_l_a) {
    regs.a = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.l = 0xFF;
    expected_regs.a = 0xFF;

    verify_ld_r8_r8(0x6F, expected_regs);
}

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

TEST_F(CpuTest, ld_a_b) {
    regs.b = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.a = 0xFF;
    expected_regs.b = 0xFF;

    verify_ld_r8_r8(0x78, expected_regs);
}

TEST_F(CpuTest, ld_a_c) {
    regs.c = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.a = 0xFF;
    expected_regs.c = 0xFF;

    verify_ld_r8_r8(0x79, expected_regs);
}

TEST_F(CpuTest, ld_a_d) {
    regs.d = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.a = 0xFF;
    expected_regs.d = 0xFF;

    verify_ld_r8_r8(0x7A, expected_regs);
}

TEST_F(CpuTest, ld_a_e) {
    regs.e = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.a = 0xFF;
    expected_regs.e = 0xFF;

    verify_ld_r8_r8(0x7B, expected_regs);
}

TEST_F(CpuTest, ld_a_h) {
    regs.h = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.a = 0xFF;
    expected_regs.h = 0xFF;

    verify_ld_r8_r8(0x7C, expected_regs);
}

TEST_F(CpuTest, ld_a_l) {
    regs.l = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.a = 0xFF;
    expected_regs.l = 0xFF;

    verify_ld_r8_r8(0x7D, expected_regs);
}

// TEST_F(CpuTest, ld_a_mhl) {
//     // execute_instruction(x);

//     ICpu::Registers expected_regs{};
//     expected_regs.pc = 1;
//     verify_state_changes(expected_regs);

//     // EXPECT_EQ(cpu->get_cycles_executed(), x);
// }

TEST_F(CpuTest, ld_a_a) {
    regs.a = 0xFF;

    ICpu::Registers expected_regs{};
    expected_regs.a = 0xFF;

    verify_ld_r8_r8(0x7F, expected_regs);
}

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
