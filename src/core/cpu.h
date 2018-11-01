#pragma once

#include "core/icpu.h"
#include "core/immu.h"

#include <cstdint>
#include <vector>
#include <functional>

namespace geemuboi::core {


class CPU : public ICpu {
public:
    CPU(IMmu& mmu_in);
    CPU(IMmu& mmu_in, Registers& regs_in);

    int execute();
    unsigned get_cycles_executed();
private:
    // Generalized CPU functionality
    void dec_r8(uint8_t& r);
    void inc_r8(uint8_t& r);
    void dec_r16(uint8_t& high, uint8_t& low);
    void inc_r16(uint8_t& high, uint8_t& low);
    void add_hl_r16(uint8_t high, uint8_t low);
    void ld_r16_r16(uint8_t& high, uint8_t& low, uint16_t r);
    void ld_mr_r8(uint16_t addr, uint8_t r);
    void ld_r8_r8(uint8_t& r1, uint8_t r2);
    void add_r8_r8(uint8_t& r1, uint8_t r2);
    void adc_r8_r8(uint8_t& r1, uint8_t r2);
    void sub_r8(uint8_t r);
    void sbc_r8_r8(uint8_t& r1, uint8_t r2);
    void and_r8(uint8_t r);
    void xor_r8(uint8_t r);
    void pop_r16(uint8_t& high, uint8_t& low);
    void push_r16(uint8_t high, uint8_t low);
    void or_r8(uint8_t r);
    void cp_r8(uint8_t r);
    void rst(uint8_t val);
    void rlc_r8(uint8_t& r);
    void rrc_r8(uint8_t& r);
    void rl_r8(uint8_t& r);
    void rr_r8(uint8_t& r);
    void sla_r8(uint8_t& r);
    void sra_r8(uint8_t& r);
    void swap_r8(uint8_t& r);
    void srl_r8(uint8_t& r);
    void bit_b_r8(uint8_t b, uint8_t r);
    void res_b_r8(uint8_t b, uint8_t& r);   
    void set_b_r8(uint8_t b, uint8_t& r);

    // Actual instruction set
    // 0x00
    int nop();
    int ld_bc_d16();
    int ld_mbc_a();
    int inc_bc();
    int inc_b();
    int dec_b();
    int ld_b_d8();
    int rlca();
    int ld_ma16_sp();
    int add_hl_bc();
    int ld_a_mbc();
    int dec_bc();
    int inc_c();
    int dec_c();
    int ld_c_d8();
    int rrca();
    // 0x10
    int stop();
    int ld_de_d16();
    int ld_mde_a();
    int inc_de();
    int inc_d();
    int dec_d();
    int ld_d_d8();
    int rla();
    int jr_r8();
    int add_hl_de();
    int ld_a_mde();
    int dec_de();
    int inc_e();
    int dec_e();
    int ld_e_d8();
    int rra();
    // 0x20
    int jr_nz_r8();
    int ld_hl_d16();
    int ldi_mhl_a();
    int inc_hl();
    int inc_h();
    int dec_h();
    int ld_h_d8();
    int daa();
    int jr_z_r8();
    int add_hl_hl();
    int ldi_a_mhl();
    int dec_hl();
    int inc_l();
    int dec_l();
    int ld_l_d8();
    int cpl();
    // 0x30
    int jr_nc_r8();
    int ld_sp_d16();
    int ldd_mhl_a();
    int inc_sp();
    int inc_mhl();
    int dec_mhl();
    int ld_mhl_d8();
    int scf();
    int jr_c_r8();
    int add_hl_sp();
    int ldd_a_mhl();
    int dec_sp();
    int inc_a();
    int dec_a();
    int ld_a_d8();
    int ccf();
    // 0x40
    int ld_b_b();
    int ld_b_c();
    int ld_b_d();
    int ld_b_e();
    int ld_b_h();
    int ld_b_l();
    int ld_b_mhl();
    int ld_b_a();
    int ld_c_b();
    int ld_c_c();
    int ld_c_d();
    int ld_c_e();
    int ld_c_h();
    int ld_c_l();
    int ld_c_mhl();
    int ld_c_a();
    // 0x50
    int ld_d_b();
    int ld_d_c();
    int ld_d_d();
    int ld_d_e();
    int ld_d_h();
    int ld_d_l();
    int ld_d_mhl();
    int ld_d_a();
    int ld_e_b();
    int ld_e_c();
    int ld_e_d();
    int ld_e_e();
    int ld_e_h();
    int ld_e_l();
    int ld_e_mhl();
    int ld_e_a();
    // 0x60
    int ld_h_b();
    int ld_h_c();
    int ld_h_d();
    int ld_h_e();
    int ld_h_h();
    int ld_h_l();
    int ld_h_mhl();
    int ld_h_a();
    int ld_l_b();
    int ld_l_c();
    int ld_l_d();
    int ld_l_e();
    int ld_l_h();
    int ld_l_l();
    int ld_l_mhl();
    int ld_l_a();
    // 0x70
    int ld_mhl_b();
    int ld_mhl_c();
    int ld_mhl_d();
    int ld_mhl_e();
    int ld_mhl_h();
    int ld_mhl_l();
    int halt();
    int ld_mhl_a();
    int ld_a_b();
    int ld_a_c();
    int ld_a_d();
    int ld_a_e();
    int ld_a_h();
    int ld_a_l();
    int ld_a_mhl();
    int ld_a_a();
    // 0x80
    int add_a_b();
    int add_a_c();
    int add_a_d();
    int add_a_e();
    int add_a_h();
    int add_a_l();
    int add_a_mhl();
    int add_a_a();
    int adc_a_b();
    int adc_a_c();
    int adc_a_d();
    int adc_a_e();
    int adc_a_h();
    int adc_a_l();
    int adc_a_mhl();
    int adc_a_a();
    // 0x90
    int sub_b();
    int sub_c();
    int sub_d();
    int sub_e();
    int sub_h();
    int sub_l();
    int sub_mhl();
    int sub_a();
    int sbc_a_b();
    int sbc_a_c();
    int sbc_a_d();
    int sbc_a_e();
    int sbc_a_h();
    int sbc_a_l();
    int sbc_a_mhl();
    int sbc_a_a();
    // 0xA0
    int and_b();
    int and_c();
    int and_d();
    int and_e();
    int and_h();
    int and_l();
    int and_mhl();
    int and_a();
    int xor_b();
    int xor_c();
    int xor_d();
    int xor_e();
    int xor_h();
    int xor_l();
    int xor_mhl();
    int xor_a();
    // 0xB0
    int or_b();
    int or_c();
    int or_d();
    int or_e();
    int or_h();
    int or_l();
    int or_mhl();
    int or_a();
    int cp_b();
    int cp_c();
    int cp_d();
    int cp_e();
    int cp_h();
    int cp_l();
    int cp_mhl();
    int cp_a();
    // 0xC0
    int ret_nz();
    int pop_bc();
    int jp_nz_a16();
    int jp_a16();
    int call_nz_a16();
    int push_bc();
    int add_a_d8();
    int rst_00h();
    int ret_z();
    int ret();
    int jp_z_a16();
    int prefix_cb();
    int call_z_a16();
    int call_a16();
    int adc_a_d8();
    int rst_08h();
    // 0xD0
    int ret_nc();
    int pop_de();
    int jp_nc_a16();
    int call_nc_a16();
    int push_de();
    int sub_d8();
    int rst_10h();
    int ret_c();
    int reti();
    int jp_c_a16();
    int call_c_a16();
    int sbc_a_d8();
    int rst_18h();
    // 0xE0
    int ldh_ma8_a();
    int pop_hl();
    int ld_mc_a();
    int push_hl();
    int and_d8();
    int rst_20h();
    int add_sp_r8();
    int jp_mhl();
    int ld_ma16_a();
    int xor_d8();
    int rst_28h();
    // 0xF0
    int ldh_a_ma8();
    int pop_af();
    int ld_a_mc();
    int di();
    int push_af();
    int or_d8();
    int rst_30h();
    int ldhl_sp_r8();
    int ld_sp_hl();
    int ld_a_ma16();
    int ei();
    int cp_d8();
    int rst_38h();
    // 0xCB00
    int rlc_b();
    int rlc_c();
    int rlc_d();
    int rlc_e();
    int rlc_h();
    int rlc_l();
    int rlc_mhl();
    int rlc_a();
    int rrc_b();
    int rrc_c();
    int rrc_d();
    int rrc_e();
    int rrc_h();
    int rrc_l();
    int rrc_mhl();
    int rrc_a();
    // 0xCB10
    int rl_b();
    int rl_c();
    int rl_d();
    int rl_e();
    int rl_h();
    int rl_l();
    int rl_mhl();
    int rl_a();
    int rr_b();
    int rr_c();
    int rr_d();
    int rr_e();
    int rr_h();
    int rr_l();
    int rr_mhl();
    int rr_a();
    // 0xCB20
    int sla_b();
    int sla_c();
    int sla_d();
    int sla_e();
    int sla_h();
    int sla_l();
    int sla_mhl();
    int sla_a();
    int sra_b();
    int sra_c();
    int sra_d();
    int sra_e();
    int sra_h();
    int sra_l();
    int sra_mhl();
    int sra_a();
    // 0xCB30
    int swap_b();
    int swap_c();
    int swap_d();
    int swap_e();
    int swap_h();
    int swap_l();
    int swap_mhl();
    int swap_a();
    int srl_b();
    int srl_c();
    int srl_d();
    int srl_e();
    int srl_h();
    int srl_l();
    int srl_mhl();
    int srl_a();
    // 0xCB40
    int bit_0_b();
    int bit_0_c();
    int bit_0_d();
    int bit_0_e();
    int bit_0_h();
    int bit_0_l();
    int bit_0_mhl();
    int bit_0_a();
    int bit_1_b();
    int bit_1_c();
    int bit_1_d();
    int bit_1_e();
    int bit_1_h();
    int bit_1_l();
    int bit_1_mhl();
    int bit_1_a();
    // 0xCB50
    int bit_2_b();
    int bit_2_c();
    int bit_2_d();
    int bit_2_e();
    int bit_2_h();
    int bit_2_l();
    int bit_2_mhl();
    int bit_2_a();
    int bit_3_b();
    int bit_3_c();
    int bit_3_d();
    int bit_3_e();
    int bit_3_h();
    int bit_3_l();
    int bit_3_mhl();
    int bit_3_a();
    // 0xCB60
    int bit_4_b();
    int bit_4_c();
    int bit_4_d();
    int bit_4_e();
    int bit_4_h();
    int bit_4_l();
    int bit_4_mhl();
    int bit_4_a();
    int bit_5_b();
    int bit_5_c();
    int bit_5_d();
    int bit_5_e();
    int bit_5_h();
    int bit_5_l();
    int bit_5_mhl();
    int bit_5_a();
    // 0xCB70
    int bit_6_b();
    int bit_6_c();
    int bit_6_d();
    int bit_6_e();
    int bit_6_h();
    int bit_6_l();
    int bit_6_mhl();
    int bit_6_a();
    int bit_7_b();
    int bit_7_c();
    int bit_7_d();
    int bit_7_e();
    int bit_7_h();
    int bit_7_l();
    int bit_7_mhl();
    int bit_7_a();
    // 0xCB80
    int res_0_b();
    int res_0_c();
    int res_0_d();
    int res_0_e();
    int res_0_h();
    int res_0_l();
    int res_0_mhl();
    int res_0_a();
    int res_1_b();
    int res_1_c();
    int res_1_d();
    int res_1_e();
    int res_1_h();
    int res_1_l();
    int res_1_mhl();
    int res_1_a();
    // 0xCB90
    int res_2_b();
    int res_2_c();
    int res_2_d();
    int res_2_e();
    int res_2_h();
    int res_2_l();
    int res_2_mhl();
    int res_2_a();
    int res_3_b();
    int res_3_c();
    int res_3_d();
    int res_3_e();
    int res_3_h();
    int res_3_l();
    int res_3_mhl();
    int res_3_a();
    // 0xCBA0
    int res_4_b();
    int res_4_c();
    int res_4_d();
    int res_4_e();
    int res_4_h();
    int res_4_l();
    int res_4_mhl();
    int res_4_a();
    int res_5_b();
    int res_5_c();
    int res_5_d();
    int res_5_e();
    int res_5_h();
    int res_5_l();
    int res_5_mhl();
    int res_5_a();
    // 0xCBB0
    int res_6_b();
    int res_6_c();
    int res_6_d();
    int res_6_e();
    int res_6_h();
    int res_6_l();
    int res_6_mhl();
    int res_6_a();
    int res_7_b();
    int res_7_c();
    int res_7_d();
    int res_7_e();
    int res_7_h();
    int res_7_l();
    int res_7_mhl();
    int res_7_a();
    // 0xCBC0
    int set_0_b();
    int set_0_c();
    int set_0_d();
    int set_0_e();
    int set_0_h();
    int set_0_l();
    int set_0_mhl();
    int set_0_a();
    int set_1_b();
    int set_1_c();
    int set_1_d();
    int set_1_e();
    int set_1_h();
    int set_1_l();
    int set_1_mhl();
    int set_1_a();
    // 0xCBD0
    int set_2_b();
    int set_2_c();
    int set_2_d();
    int set_2_e();
    int set_2_h();
    int set_2_l();
    int set_2_mhl();
    int set_2_a();
    int set_3_b();
    int set_3_c();
    int set_3_d();
    int set_3_e();
    int set_3_h();
    int set_3_l();
    int set_3_mhl();
    int set_3_a();
    // 0xCBE0
    int set_4_b();
    int set_4_c();
    int set_4_d();
    int set_4_e();
    int set_4_h();
    int set_4_l();
    int set_4_mhl();
    int set_4_a();
    int set_5_b();
    int set_5_c();
    int set_5_d();
    int set_5_e();
    int set_5_h();
    int set_5_l();
    int set_5_mhl();
    int set_5_a();
    // 0xCBF0
    int set_6_b();
    int set_6_c();
    int set_6_d();
    int set_6_e();
    int set_6_h();
    int set_6_l();
    int set_6_mhl();
    int set_6_a();
    int set_7_b();
    int set_7_c();
    int set_7_d();
    int set_7_e();
    int set_7_h();
    int set_7_l();
    int set_7_mhl();
    int set_7_a();
 
    int unimplemented();

    IMmu& mmu;
    Registers& regs;
    const std::vector<std::function<int()>> instructions;
   
    unsigned cycles;
};

inline void CPU::dec_r8(uint8_t& r) {
    regs.f &= ICpu::C_FLAG;
    regs.f |= ICpu::N_FLAG;

    if (!(r & 0xF)) {
        regs.f |= ICpu::H_FLAG;
    }

    if (!--r) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::inc_r8(uint8_t& r) {
    regs.f &= ICpu::C_FLAG;
    if ((r & 0xF) == 0xF) {
        regs.f |= ICpu::H_FLAG;
    }

    if (!++r) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::dec_r16(uint8_t& high, uint8_t& low) {
    if (--low == 0xFF) {
        --high;
    }
}

inline void CPU::inc_r16(uint8_t& high, uint8_t& low) {
    if (!++low) {
        ++high;
    }
}

inline void CPU::add_hl_r16(uint8_t high, uint8_t low) {
    regs.f &= ICpu::Z_FLAG;
    uint32_t sum = regs.l + low;
    if (sum >= 0x100) {
        ++high;
    }

    regs.l += low;
    if ((((regs.h & 0xF) + (high & 0xF)) & 0x10) == 0x10) {
        regs.f |= ICpu::H_FLAG;
    }

    sum = regs.h + high;
    if (sum >= 0x100) {
        regs.f |= ICpu::C_FLAG;
    }

    regs.h += high; 
}

inline void CPU::ld_r16_r16(uint8_t& high, uint8_t& low, uint16_t r) {
    high = r >> 8;
    low = r;
}

inline void CPU::ld_mr_r8(uint16_t addr, uint8_t r) {
    mmu.write_byte(addr, r);
}

inline void CPU::ld_r8_r8(uint8_t& r1, uint8_t r2) {
    r1 = r2;
}

inline void CPU::add_r8_r8(uint8_t& r1, uint8_t r2) {
    regs.f = 0;
    if ((r1 & 0xF) + (r2 & 0xF) >= 0x10) {
        regs.f |= ICpu::H_FLAG;
    }

    if (r1 + r2 >= 0x100) {
        regs.f |= ICpu::C_FLAG;
    }

    if (!(r1 += r2)) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::adc_r8_r8(uint8_t& r1, uint8_t r2) {
    uint8_t carry = (regs.f & ICpu::C_FLAG) >> 4;
    regs.f = 0;
    if ((r1 & 0xF) + (r2 & 0xF) + carry >= 0x10) {
        regs.f |= ICpu::H_FLAG;
    }

    if (r1 + r2 + carry >= 0x100) {
        regs.f |= ICpu::C_FLAG;
    }

    if (!(r1 += r2 + carry)) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::sub_r8(uint8_t r) {
    regs.f = 0;
    regs.f |= ICpu::N_FLAG;

    if ((regs.a & 0xF) < (r & 0xF)) {
        regs.f |= ICpu::H_FLAG;
    }

    if (regs.a < r) {
        regs.f |= ICpu::C_FLAG;
    }

    if (!(regs.a -= r)) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::sbc_r8_r8(uint8_t& r1, uint8_t r2) {
    uint8_t carry = (regs.f & ICpu::C_FLAG) >> 4;
    regs.f = 0;
    regs.f |= ICpu::N_FLAG;

    if ((r1 & 0xF) < (r2 & 0xF) - carry) {
        regs.f |= ICpu::H_FLAG;
    }

    if (r1 < r2 - carry) {
        regs.f |= ICpu::C_FLAG;
    }

    if (!(r1 -= r2 - carry)) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::and_r8(uint8_t r) {
    regs.f = 0;
    if (!(regs.a &= r)) {
        regs.f |= ICpu::Z_FLAG | ICpu::H_FLAG;
    } else {
        regs.f |= ICpu::H_FLAG;
    }
}

inline void CPU::xor_r8(uint8_t r) {
    regs.f = 0;
    if (!(regs.a ^= r)) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::pop_r16(uint8_t& high, uint8_t& low) {
    low = mmu.read_byte(regs.sp++);
    high = mmu.read_byte(regs.sp++);
}

inline void CPU::push_r16(uint8_t high, uint8_t low) {
    mmu.write_byte(--regs.sp, high);
    mmu.write_byte(--regs.sp, low);
}

inline void CPU::or_r8(uint8_t r) {
    regs.f = 0;
    if (!(regs.a |= r)) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::cp_r8(uint8_t r) {
    regs.f = 0;
    regs.f |= ICpu::N_FLAG;

    if ((regs.a & 0xF) < (r & 0xF)) {
        regs.f |= ICpu::H_FLAG;
    }

    if (regs.a < r) {
        regs.f |= ICpu::C_FLAG;
    }

    if (!(regs.a - r)) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::rst(uint8_t val) {
    mmu.write_word(regs.sp - 2, regs.pc);
    regs.sp -= 2;
    regs.pc = val;
}

inline void CPU::rlc_r8(uint8_t& r) {
    regs.f = 0;
    if (r & 0x80) {
        regs.f |= ICpu::C_FLAG;
    }

    r = (r << 1) + (regs.f >> 4);
    if (!r) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::rrc_r8(uint8_t& r) {
    regs.f = 0;
    if (r & 0x1) {
        regs.f |= ICpu::C_FLAG;
    }

    r = (r >> 1) + (regs.f << 3);
    if (!r) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::rl_r8(uint8_t& r) {
    uint8_t carry = (regs.f & ICpu::C_FLAG) >> 4;
    regs.f = 0;
    if (r & 0x80) {
        regs.f |= ICpu::C_FLAG;
    }

    r = (r << 1) + carry;
    if (!r) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::rr_r8(uint8_t& r) {
    uint8_t carry = (regs.f & ICpu::C_FLAG) << 3;
    regs.f = 0;
    if (r & 0x1) {
        regs.f |= ICpu::C_FLAG;
    }

    r = (r >> 1) + carry;
    if (!r) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::sla_r8(uint8_t& r) {
    regs.f = 0;
    regs.f |= (r & 0x80) << 4;
    if (!(r <<= 1)) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::sra_r8(uint8_t& r) {
    regs.f = 0;
    regs.f |= (r & 0x1) << 4;
    uint8_t msb = r & 0x80;
    r >>= 1;
    r += msb;
    if (!r) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::swap_r8(uint8_t& r) {
    regs.f = 0;
    uint8_t high = r >> 4;
    r = (r << 4) + high;
    if (!r) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::srl_r8(uint8_t& r) {
    regs.f = 0;
    regs.f |= (r & 0x1) << 4;
    if (!(r >>= 1)) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::bit_b_r8(uint8_t b, uint8_t r) {
    regs.f &= ICpu::C_FLAG;
    regs.f |= ICpu::H_FLAG;
    if (!(r & (1 << b))) {
        regs.f |= ICpu::Z_FLAG;
    }
}

inline void CPU::res_b_r8(uint8_t b, uint8_t& r) {
    r &= ~(1 << b);
}

inline void CPU::set_b_r8(uint8_t b, uint8_t& r) {
    r |= 1 << b;
}


}
