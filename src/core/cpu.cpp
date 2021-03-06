#include "cpu.h"

namespace geemuboi::core {


CPU::CPU(IMmu& mmu_in, Registers& regs_in) : mmu(mmu_in), 
    regs(regs_in),
    instructions{
        std::bind(&CPU::nop, this),
        std::bind(&CPU::ld_bc_d16, this),
        std::bind(&CPU::ld_mbc_a, this),
        std::bind(&CPU::inc_bc, this),
        std::bind(&CPU::inc_b, this),
        std::bind(&CPU::dec_b, this),
        std::bind(&CPU::ld_b_d8, this),
        std::bind(&CPU::rlca, this),
        std::bind(&CPU::ld_ma16_sp, this),
        std::bind(&CPU::add_hl_bc, this),
        std::bind(&CPU::ld_a_mbc, this),
        std::bind(&CPU::dec_bc, this),
        std::bind(&CPU::inc_c, this),
        std::bind(&CPU::dec_c, this),
        std::bind(&CPU::ld_c_d8, this),
        std::bind(&CPU::rrca, this),
        std::bind(&CPU::stop, this),
        std::bind(&CPU::ld_de_d16, this),
        std::bind(&CPU::ld_mde_a, this),
        std::bind(&CPU::inc_de, this),
        std::bind(&CPU::inc_d, this),
        std::bind(&CPU::dec_d, this),
        std::bind(&CPU::ld_d_d8, this),
        std::bind(&CPU::rla, this),
        std::bind(&CPU::jr_r8, this),
        std::bind(&CPU::add_hl_de, this),
        std::bind(&CPU::ld_a_mde, this),
        std::bind(&CPU::dec_de, this),
        std::bind(&CPU::inc_e, this),
        std::bind(&CPU::dec_e, this),
        std::bind(&CPU::ld_e_d8, this),
        std::bind(&CPU::rra, this),
        std::bind(&CPU::jr_nz_r8, this),
        std::bind(&CPU::ld_hl_d16, this),
        std::bind(&CPU::ldi_mhl_a, this),
        std::bind(&CPU::inc_hl, this),
        std::bind(&CPU::inc_h, this),
        std::bind(&CPU::dec_h, this),
        std::bind(&CPU::ld_h_d8, this),
        std::bind(&CPU::daa, this),
        std::bind(&CPU::jr_z_r8, this),
        std::bind(&CPU::add_hl_hl, this),
        std::bind(&CPU::ldi_a_mhl, this),
        std::bind(&CPU::dec_hl, this),
        std::bind(&CPU::inc_l, this),
        std::bind(&CPU::dec_l, this),
        std::bind(&CPU::ld_l_d8, this),
        std::bind(&CPU::cpl, this),
        std::bind(&CPU::jr_nc_r8, this),
        std::bind(&CPU::ld_sp_d16, this),
        std::bind(&CPU::ldd_mhl_a, this),
        std::bind(&CPU::inc_sp, this),
        std::bind(&CPU::inc_mhl, this),
        std::bind(&CPU::dec_mhl, this),
        std::bind(&CPU::ld_mhl_d8, this),
        std::bind(&CPU::scf, this),
        std::bind(&CPU::jr_c_r8, this),
        std::bind(&CPU::add_hl_sp, this),
        std::bind(&CPU::ldd_a_mhl, this),
        std::bind(&CPU::dec_sp, this),
        std::bind(&CPU::inc_a, this),
        std::bind(&CPU::dec_a, this),
        std::bind(&CPU::ld_a_d8, this),
        std::bind(&CPU::ccf, this),
        std::bind(&CPU::ld_b_b, this),
        std::bind(&CPU::ld_b_c, this),
        std::bind(&CPU::ld_b_d, this),
        std::bind(&CPU::ld_b_e, this),
        std::bind(&CPU::ld_b_h, this),
        std::bind(&CPU::ld_b_l, this),
        std::bind(&CPU::ld_b_mhl, this),
        std::bind(&CPU::ld_b_a, this),
        std::bind(&CPU::ld_c_b, this),
        std::bind(&CPU::ld_c_c, this),
        std::bind(&CPU::ld_c_d, this),
        std::bind(&CPU::ld_c_e, this),
        std::bind(&CPU::ld_c_h, this),
        std::bind(&CPU::ld_c_l, this),
        std::bind(&CPU::ld_c_mhl, this),
        std::bind(&CPU::ld_c_a, this),
        std::bind(&CPU::ld_d_b, this),
        std::bind(&CPU::ld_d_c, this),
        std::bind(&CPU::ld_d_d, this),
        std::bind(&CPU::ld_d_e, this),
        std::bind(&CPU::ld_d_h, this),
        std::bind(&CPU::ld_d_l, this),
        std::bind(&CPU::ld_d_mhl, this),
        std::bind(&CPU::ld_d_a, this),
        std::bind(&CPU::ld_e_b, this),
        std::bind(&CPU::ld_e_c, this),
        std::bind(&CPU::ld_e_d, this),
        std::bind(&CPU::ld_e_e, this),
        std::bind(&CPU::ld_e_h, this),
        std::bind(&CPU::ld_e_l, this),
        std::bind(&CPU::ld_e_mhl, this),
        std::bind(&CPU::ld_e_a, this),
        std::bind(&CPU::ld_h_b, this),
        std::bind(&CPU::ld_h_c, this),
        std::bind(&CPU::ld_h_d, this),
        std::bind(&CPU::ld_h_e, this),
        std::bind(&CPU::ld_h_h, this),
        std::bind(&CPU::ld_h_l, this),
        std::bind(&CPU::ld_h_mhl, this),
        std::bind(&CPU::ld_h_a, this),
        std::bind(&CPU::ld_l_b, this),
        std::bind(&CPU::ld_l_c, this),
        std::bind(&CPU::ld_l_d, this),
        std::bind(&CPU::ld_l_e, this),
        std::bind(&CPU::ld_l_h, this),
        std::bind(&CPU::ld_l_l, this),
        std::bind(&CPU::ld_l_mhl, this),
        std::bind(&CPU::ld_l_a, this),
        std::bind(&CPU::ld_mhl_b, this),
        std::bind(&CPU::ld_mhl_c, this),
        std::bind(&CPU::ld_mhl_d, this),
        std::bind(&CPU::ld_mhl_e, this),
        std::bind(&CPU::ld_mhl_h, this),
        std::bind(&CPU::ld_mhl_l, this),
        std::bind(&CPU::halt, this),
        std::bind(&CPU::ld_mhl_a, this),
        std::bind(&CPU::ld_a_b, this),
        std::bind(&CPU::ld_a_c, this),
        std::bind(&CPU::ld_a_d, this),
        std::bind(&CPU::ld_a_e, this),
        std::bind(&CPU::ld_a_h, this),
        std::bind(&CPU::ld_a_l, this),
        std::bind(&CPU::ld_a_mhl, this),
        std::bind(&CPU::ld_a_a, this),
        std::bind(&CPU::add_a_b, this),
        std::bind(&CPU::add_a_c, this),
        std::bind(&CPU::add_a_d, this),
        std::bind(&CPU::add_a_e, this),
        std::bind(&CPU::add_a_h, this),
        std::bind(&CPU::add_a_l, this),
        std::bind(&CPU::add_a_mhl, this),
        std::bind(&CPU::add_a_a, this),
        std::bind(&CPU::adc_a_b, this),
        std::bind(&CPU::adc_a_c, this),
        std::bind(&CPU::adc_a_d, this),
        std::bind(&CPU::adc_a_e, this),
        std::bind(&CPU::adc_a_h, this),
        std::bind(&CPU::adc_a_l, this),
        std::bind(&CPU::adc_a_mhl, this),
        std::bind(&CPU::adc_a_a, this),
        std::bind(&CPU::sub_b, this),
        std::bind(&CPU::sub_c, this),
        std::bind(&CPU::sub_d, this),
        std::bind(&CPU::sub_e, this),
        std::bind(&CPU::sub_h, this),
        std::bind(&CPU::sub_l, this),
        std::bind(&CPU::sub_mhl, this),
        std::bind(&CPU::sub_a, this),
        std::bind(&CPU::sbc_a_b, this),
        std::bind(&CPU::sbc_a_c, this),
        std::bind(&CPU::sbc_a_d, this),
        std::bind(&CPU::sbc_a_e, this),
        std::bind(&CPU::sbc_a_h, this),
        std::bind(&CPU::sbc_a_l, this),
        std::bind(&CPU::sbc_a_mhl, this),
        std::bind(&CPU::sbc_a_a, this),
        std::bind(&CPU::and_b, this),
        std::bind(&CPU::and_c, this),
        std::bind(&CPU::and_d, this),
        std::bind(&CPU::and_e, this),
        std::bind(&CPU::and_h, this),
        std::bind(&CPU::and_l, this),
        std::bind(&CPU::and_mhl, this),
        std::bind(&CPU::and_a, this),
        std::bind(&CPU::xor_b, this),
        std::bind(&CPU::xor_c, this),
        std::bind(&CPU::xor_d, this),
        std::bind(&CPU::xor_e, this),
        std::bind(&CPU::xor_h, this),
        std::bind(&CPU::xor_l, this),
        std::bind(&CPU::xor_mhl, this),
        std::bind(&CPU::xor_a, this),
        std::bind(&CPU::or_b, this),
        std::bind(&CPU::or_c, this),
        std::bind(&CPU::or_d, this),
        std::bind(&CPU::or_e, this),
        std::bind(&CPU::or_h, this),
        std::bind(&CPU::or_l, this),
        std::bind(&CPU::or_mhl, this),
        std::bind(&CPU::or_a, this),
        std::bind(&CPU::cp_b, this),
        std::bind(&CPU::cp_c, this),
        std::bind(&CPU::cp_d, this),
        std::bind(&CPU::cp_e, this),
        std::bind(&CPU::cp_h, this),
        std::bind(&CPU::cp_l, this),
        std::bind(&CPU::cp_mhl, this),
        std::bind(&CPU::cp_a, this),
        std::bind(&CPU::ret_nz, this),
        std::bind(&CPU::pop_bc, this),
        std::bind(&CPU::jp_nz_a16, this),
        std::bind(&CPU::jp_a16, this),
        std::bind(&CPU::call_nz_a16, this),
        std::bind(&CPU::push_bc, this),
        std::bind(&CPU::add_a_d8, this),
        std::bind(&CPU::rst_00h, this),
        std::bind(&CPU::ret_z, this),
        std::bind(&CPU::ret, this),
        std::bind(&CPU::jp_z_a16, this),
        std::bind(&CPU::prefix_cb, this),
        std::bind(&CPU::call_z_a16, this),
        std::bind(&CPU::call_a16, this),
        std::bind(&CPU::adc_a_d8, this),
        std::bind(&CPU::rst_08h, this),
        std::bind(&CPU::ret_nc, this),
        std::bind(&CPU::pop_de, this),
        std::bind(&CPU::jp_nc_a16, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::call_nc_a16, this),
        std::bind(&CPU::push_de, this),
        std::bind(&CPU::sub_d8, this),
        std::bind(&CPU::rst_10h, this),
        std::bind(&CPU::ret_c, this),
        std::bind(&CPU::reti, this),
        std::bind(&CPU::jp_c_a16, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::call_c_a16, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::sbc_a_d8, this),
        std::bind(&CPU::rst_18h, this),
        std::bind(&CPU::ldh_ma8_a, this),
        std::bind(&CPU::pop_hl, this),
        std::bind(&CPU::ld_mc_a, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::push_hl, this),
        std::bind(&CPU::and_d8, this),
        std::bind(&CPU::rst_20h, this),
        std::bind(&CPU::add_sp_r8, this),
        std::bind(&CPU::jp_mhl, this),
        std::bind(&CPU::ld_ma16_a, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::xor_d8, this),
        std::bind(&CPU::rst_28h, this),
        std::bind(&CPU::ldh_a_ma8, this),
        std::bind(&CPU::pop_af, this),
        std::bind(&CPU::ld_a_mc, this),
        std::bind(&CPU::di, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::push_af, this),
        std::bind(&CPU::or_d8, this),
        std::bind(&CPU::rst_30h, this),
        std::bind(&CPU::ldhl_sp_r8, this),
        std::bind(&CPU::ld_sp_hl, this),
        std::bind(&CPU::ld_a_ma16, this),
        std::bind(&CPU::ei, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::unimplemented, this),
        std::bind(&CPU::cp_d8, this),
        std::bind(&CPU::rst_38h, this),
        std::bind(&CPU::rlc_b, this),
        std::bind(&CPU::rlc_c, this),
        std::bind(&CPU::rlc_d, this),
        std::bind(&CPU::rlc_e, this),
        std::bind(&CPU::rlc_h, this),
        std::bind(&CPU::rlc_l, this),
        std::bind(&CPU::rlc_mhl, this),
        std::bind(&CPU::rlc_a, this),
        std::bind(&CPU::rrc_b, this),
        std::bind(&CPU::rrc_c, this),
        std::bind(&CPU::rrc_d, this),
        std::bind(&CPU::rrc_e, this),
        std::bind(&CPU::rrc_h, this),
        std::bind(&CPU::rrc_l, this),
        std::bind(&CPU::rrc_mhl, this),
        std::bind(&CPU::rrc_a, this),
        std::bind(&CPU::rl_b, this),
        std::bind(&CPU::rl_c, this),
        std::bind(&CPU::rl_d, this),
        std::bind(&CPU::rl_e, this),
        std::bind(&CPU::rl_h, this),
        std::bind(&CPU::rl_l, this),
        std::bind(&CPU::rl_mhl, this),
        std::bind(&CPU::rl_a, this),
        std::bind(&CPU::rr_b, this),
        std::bind(&CPU::rr_c, this),
        std::bind(&CPU::rr_d, this),
        std::bind(&CPU::rr_e, this),
        std::bind(&CPU::rr_h, this),
        std::bind(&CPU::rr_l, this),
        std::bind(&CPU::rr_mhl, this),
        std::bind(&CPU::rr_a, this),
        std::bind(&CPU::sla_b, this),
        std::bind(&CPU::sla_c, this),
        std::bind(&CPU::sla_d, this),
        std::bind(&CPU::sla_e, this),
        std::bind(&CPU::sla_h, this),
        std::bind(&CPU::sla_l, this),
        std::bind(&CPU::sla_mhl, this),
        std::bind(&CPU::sla_a, this),
        std::bind(&CPU::sra_b, this),
        std::bind(&CPU::sra_c, this),
        std::bind(&CPU::sra_d, this),
        std::bind(&CPU::sra_e, this),
        std::bind(&CPU::sra_h, this),
        std::bind(&CPU::sra_l, this),
        std::bind(&CPU::sra_mhl, this),
        std::bind(&CPU::sra_a, this),
        std::bind(&CPU::swap_b, this),
        std::bind(&CPU::swap_c, this),
        std::bind(&CPU::swap_d, this),
        std::bind(&CPU::swap_e, this),
        std::bind(&CPU::swap_h, this),
        std::bind(&CPU::swap_l, this),
        std::bind(&CPU::swap_mhl, this),
        std::bind(&CPU::swap_a, this),
        std::bind(&CPU::srl_b, this),
        std::bind(&CPU::srl_c, this),
        std::bind(&CPU::srl_d, this),
        std::bind(&CPU::srl_e, this),
        std::bind(&CPU::srl_h, this),
        std::bind(&CPU::srl_l, this),
        std::bind(&CPU::srl_mhl, this),
        std::bind(&CPU::srl_a, this),
        std::bind(&CPU::bit_0_b, this),
        std::bind(&CPU::bit_0_c, this),
        std::bind(&CPU::bit_0_d, this),
        std::bind(&CPU::bit_0_e, this),
        std::bind(&CPU::bit_0_h, this),
        std::bind(&CPU::bit_0_l, this),
        std::bind(&CPU::bit_0_mhl, this),
        std::bind(&CPU::bit_0_a, this),
        std::bind(&CPU::bit_1_b, this),
        std::bind(&CPU::bit_1_c, this),
        std::bind(&CPU::bit_1_d, this),
        std::bind(&CPU::bit_1_e, this),
        std::bind(&CPU::bit_1_h, this),
        std::bind(&CPU::bit_1_l, this),
        std::bind(&CPU::bit_1_mhl, this),
        std::bind(&CPU::bit_1_a, this),
        std::bind(&CPU::bit_2_b, this),
        std::bind(&CPU::bit_2_c, this),
        std::bind(&CPU::bit_2_d, this),
        std::bind(&CPU::bit_2_e, this),
        std::bind(&CPU::bit_2_h, this),
        std::bind(&CPU::bit_2_l, this),
        std::bind(&CPU::bit_2_mhl, this),
        std::bind(&CPU::bit_2_a, this),
        std::bind(&CPU::bit_3_b, this),
        std::bind(&CPU::bit_3_c, this),
        std::bind(&CPU::bit_3_d, this),
        std::bind(&CPU::bit_3_e, this),
        std::bind(&CPU::bit_3_h, this),
        std::bind(&CPU::bit_3_l, this),
        std::bind(&CPU::bit_3_mhl, this),
        std::bind(&CPU::bit_3_a, this),
        std::bind(&CPU::bit_4_b, this),
        std::bind(&CPU::bit_4_c, this),
        std::bind(&CPU::bit_4_d, this),
        std::bind(&CPU::bit_4_e, this),
        std::bind(&CPU::bit_4_h, this),
        std::bind(&CPU::bit_4_l, this),
        std::bind(&CPU::bit_4_mhl, this),
        std::bind(&CPU::bit_4_a, this),
        std::bind(&CPU::bit_5_b, this),
        std::bind(&CPU::bit_5_c, this),
        std::bind(&CPU::bit_5_d, this),
        std::bind(&CPU::bit_5_e, this),
        std::bind(&CPU::bit_5_h, this),
        std::bind(&CPU::bit_5_l, this),
        std::bind(&CPU::bit_5_mhl, this),
        std::bind(&CPU::bit_5_a, this),
        std::bind(&CPU::bit_6_b, this),
        std::bind(&CPU::bit_6_c, this),
        std::bind(&CPU::bit_6_d, this),
        std::bind(&CPU::bit_6_e, this),
        std::bind(&CPU::bit_6_h, this),
        std::bind(&CPU::bit_6_l, this),
        std::bind(&CPU::bit_6_mhl, this),
        std::bind(&CPU::bit_6_a, this),
        std::bind(&CPU::bit_7_b, this),
        std::bind(&CPU::bit_7_c, this),
        std::bind(&CPU::bit_7_d, this),
        std::bind(&CPU::bit_7_e, this),
        std::bind(&CPU::bit_7_h, this),
        std::bind(&CPU::bit_7_l, this),
        std::bind(&CPU::bit_7_mhl, this),
        std::bind(&CPU::bit_7_a, this),
        std::bind(&CPU::res_0_b, this),
        std::bind(&CPU::res_0_c, this),
        std::bind(&CPU::res_0_d, this),
        std::bind(&CPU::res_0_e, this),
        std::bind(&CPU::res_0_h, this),
        std::bind(&CPU::res_0_l, this),
        std::bind(&CPU::res_0_mhl, this),
        std::bind(&CPU::res_0_a, this),
        std::bind(&CPU::res_1_b, this),
        std::bind(&CPU::res_1_c, this),
        std::bind(&CPU::res_1_d, this),
        std::bind(&CPU::res_1_e, this),
        std::bind(&CPU::res_1_h, this),
        std::bind(&CPU::res_1_l, this),
        std::bind(&CPU::res_1_mhl, this),
        std::bind(&CPU::res_1_a, this),
        std::bind(&CPU::res_2_b, this),
        std::bind(&CPU::res_2_c, this),
        std::bind(&CPU::res_2_d, this),
        std::bind(&CPU::res_2_e, this),
        std::bind(&CPU::res_2_h, this),
        std::bind(&CPU::res_2_l, this),
        std::bind(&CPU::res_2_mhl, this),
        std::bind(&CPU::res_2_a, this),
        std::bind(&CPU::res_3_b, this),
        std::bind(&CPU::res_3_c, this),
        std::bind(&CPU::res_3_d, this),
        std::bind(&CPU::res_3_e, this),
        std::bind(&CPU::res_3_h, this),
        std::bind(&CPU::res_3_l, this),
        std::bind(&CPU::res_3_mhl, this),
        std::bind(&CPU::res_3_a, this),
        std::bind(&CPU::res_4_b, this),
        std::bind(&CPU::res_4_c, this),
        std::bind(&CPU::res_4_d, this),
        std::bind(&CPU::res_4_e, this),
        std::bind(&CPU::res_4_h, this),
        std::bind(&CPU::res_4_l, this),
        std::bind(&CPU::res_4_mhl, this),
        std::bind(&CPU::res_4_a, this),
        std::bind(&CPU::res_5_b, this),
        std::bind(&CPU::res_5_c, this),
        std::bind(&CPU::res_5_d, this),
        std::bind(&CPU::res_5_e, this),
        std::bind(&CPU::res_5_h, this),
        std::bind(&CPU::res_5_l, this),
        std::bind(&CPU::res_5_mhl, this),
        std::bind(&CPU::res_5_a, this),
        std::bind(&CPU::res_6_b, this),
        std::bind(&CPU::res_6_c, this),
        std::bind(&CPU::res_6_d, this),
        std::bind(&CPU::res_6_e, this),
        std::bind(&CPU::res_6_h, this),
        std::bind(&CPU::res_6_l, this),
        std::bind(&CPU::res_6_mhl, this),
        std::bind(&CPU::res_6_a, this),
        std::bind(&CPU::res_7_b, this),
        std::bind(&CPU::res_7_c, this),
        std::bind(&CPU::res_7_d, this),
        std::bind(&CPU::res_7_e, this),
        std::bind(&CPU::res_7_h, this),
        std::bind(&CPU::res_7_l, this),
        std::bind(&CPU::res_7_mhl, this),
        std::bind(&CPU::res_7_a, this),
        std::bind(&CPU::set_0_b, this),
        std::bind(&CPU::set_0_c, this),
        std::bind(&CPU::set_0_d, this),
        std::bind(&CPU::set_0_e, this),
        std::bind(&CPU::set_0_h, this),
        std::bind(&CPU::set_0_l, this),
        std::bind(&CPU::set_0_mhl, this),
        std::bind(&CPU::set_0_a, this),
        std::bind(&CPU::set_1_b, this),
        std::bind(&CPU::set_1_c, this),
        std::bind(&CPU::set_1_d, this),
        std::bind(&CPU::set_1_e, this),
        std::bind(&CPU::set_1_h, this),
        std::bind(&CPU::set_1_l, this),
        std::bind(&CPU::set_1_mhl, this),
        std::bind(&CPU::set_1_a, this),
        std::bind(&CPU::set_2_b, this),
        std::bind(&CPU::set_2_c, this),
        std::bind(&CPU::set_2_d, this),
        std::bind(&CPU::set_2_e, this),
        std::bind(&CPU::set_2_h, this),
        std::bind(&CPU::set_2_l, this),
        std::bind(&CPU::set_2_mhl, this),
        std::bind(&CPU::set_2_a, this),
        std::bind(&CPU::set_3_b, this),
        std::bind(&CPU::set_3_c, this),
        std::bind(&CPU::set_3_d, this),
        std::bind(&CPU::set_3_e, this),
        std::bind(&CPU::set_3_h, this),
        std::bind(&CPU::set_3_l, this),
        std::bind(&CPU::set_3_mhl, this),
        std::bind(&CPU::set_3_a, this),
        std::bind(&CPU::set_4_b, this),
        std::bind(&CPU::set_4_c, this),
        std::bind(&CPU::set_4_d, this),
        std::bind(&CPU::set_4_e, this),
        std::bind(&CPU::set_4_h, this),
        std::bind(&CPU::set_4_l, this),
        std::bind(&CPU::set_4_mhl, this),
        std::bind(&CPU::set_4_a, this),
        std::bind(&CPU::set_5_b, this),
        std::bind(&CPU::set_5_c, this),
        std::bind(&CPU::set_5_d, this),
        std::bind(&CPU::set_5_e, this),
        std::bind(&CPU::set_5_h, this),
        std::bind(&CPU::set_5_l, this),
        std::bind(&CPU::set_5_mhl, this),
        std::bind(&CPU::set_5_a, this),
        std::bind(&CPU::set_6_b, this),
        std::bind(&CPU::set_6_c, this),
        std::bind(&CPU::set_6_d, this),
        std::bind(&CPU::set_6_e, this),
        std::bind(&CPU::set_6_h, this),
        std::bind(&CPU::set_6_l, this),
        std::bind(&CPU::set_6_mhl, this),
        std::bind(&CPU::set_6_a, this),
        std::bind(&CPU::set_7_b, this),
        std::bind(&CPU::set_7_c, this),
        std::bind(&CPU::set_7_d, this),
        std::bind(&CPU::set_7_e, this),
        std::bind(&CPU::set_7_h, this),
        std::bind(&CPU::set_7_l, this),
        std::bind(&CPU::set_7_mhl, this),
        std::bind(&CPU::set_7_a, this)
    },
    cycles{} {}


int CPU::execute() {
    unsigned instruction_cycles = instructions[mmu.read_byte(regs.pc++)]();
    cycles += instruction_cycles;

    return instruction_cycles;
}


unsigned CPU::get_cycles_executed() {
    return cycles;
}


// 0x00
int CPU::nop() {
    return 1;
}

int CPU::ld_bc_d16() {
    ld_r16_r16(regs.b, regs.c, mmu.read_word(regs.pc));
    regs.pc += 2;
    return 3;
}

int CPU::ld_mbc_a() {
    uint16_t addr = (regs.b << 8) + regs.c;
    ld_mr_r8(addr, regs.a);
    return 2;
}

int CPU::inc_bc() {
    inc_r16(regs.b, regs.c);
    return 2;
}

int CPU::inc_b() {
    inc_r8(regs.b);
    return 1;
}

int CPU::dec_b() {
    dec_r8(regs.b);
    return 1;
}

int CPU::ld_b_d8() {
    ld_r8_r8(regs.b, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rlca() {
    regs.f = 0;
    if (regs.a & 0x80) {
        regs.f |= 0x10;
    }

    regs.a = (regs.a << 1) + (regs.f >> 4);
    return 1;
}

int CPU::ld_ma16_sp() {
    uint16_t addr = mmu.read_word(regs.pc);
    regs.pc += 2;
    mmu.write_word(addr, regs.sp);
    return 5;
}

int CPU::add_hl_bc() {
    add_hl_r16(regs.b, regs.c);
    return 2;
}

int CPU::ld_a_mbc() {
    uint16_t addr = (regs.b << 8) + regs.c;
    ld_r8_r8(regs.a, mmu.read_byte(addr));
    return 2;
}

int CPU::dec_bc() {
    dec_r16(regs.b, regs.c);
    return 2;
}

int CPU::inc_c() {
    inc_r8(regs.c);
    return 1;
}

int CPU::dec_c() {
    dec_r8(regs.c);
    return 1;
}

int CPU::ld_c_d8() {
    ld_r8_r8(regs.c, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rrca() {
    regs.f = 0;
    if (regs.a & 0x1) {
        regs.f |= 0x10;
    }

    regs.a = (regs.a >> 1) + (regs.f << 3);
    return 1;
}

// 0x1
int CPU::stop() {
    // unknown?
    regs.pc++;
    return 1;
}

int CPU::ld_de_d16() {
    ld_r16_r16(regs.d, regs.e, mmu.read_word(regs.pc));
    regs.pc += 2;
    return 3;
}

int CPU::ld_mde_a() {
    uint16_t addr = (regs.d << 8) + regs.e;
    ld_mr_r8(addr, regs.a);
    return 2;
}

int CPU::inc_de() {
    inc_r16(regs.d, regs.e);
    return 2;
}

int CPU::inc_d() {
    inc_r8(regs.d);
    return 1;
}

int CPU::dec_d() {
    dec_r8(regs.d);
    return 1;
}

int CPU::ld_d_d8() {
    ld_r8_r8(regs.d, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rla() {
    uint8_t carry = (regs.f & 0x10) >> 4;
    regs.f = 0;
    if (regs.a & 0x80) {
        regs.f |= 0x10;
    }

    regs.a = (regs.a << 1) + carry;
    return 1;
}

int CPU::jr_r8() {
    // CHECK
    int offset = static_cast<int8_t>(mmu.read_byte(regs.pc)) + 2;
    regs.pc += offset - 1;
    return 3;
}

int CPU::add_hl_de() {
    add_hl_r16(regs.d, regs.e);
    return 2;
}

int CPU::ld_a_mde() {
    uint16_t addr = (regs.d << 8) + regs.e;
    ld_r8_r8(regs.a, mmu.read_byte(addr));
    return 2;
}

int CPU::dec_de() {
    dec_r16(regs.d, regs.e);
    return 2;
}

int CPU::inc_e() {
    inc_r8(regs.e);
    return 1;
}

int CPU::dec_e() {
    dec_r8(regs.e);
    return 1;
}

int CPU::ld_e_d8() {
    ld_r8_r8(regs.e, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rra() {
    uint8_t carry = (regs.f & 0x10) << 3;
    regs.f = 0;
    if (regs.a & 0x1) {
        regs.f |= 0x10;
    }

    regs.a = (regs.a >> 1) + carry;
    return 1;
}

// 0x2
int CPU::jr_nz_r8() {
    if (!(regs.f & ICpu::Z_FLAG)) {
        int offset = static_cast<int8_t>(mmu.read_byte(regs.pc)) + 2;
        regs.pc += offset - 1;
        return 3;
    } else {
        regs.pc += 1;
        return 2;
    }
}

int CPU::ld_hl_d16() {
    ld_r16_r16(regs.h, regs.l, mmu.read_word(regs.pc));
    regs.pc += 2;
    return 3;
}

int CPU::ldi_mhl_a() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_mr_r8(addr, regs.a);
    inc_hl();
    return 2;
}

int CPU::inc_hl() {
    inc_r16(regs.h, regs.l);
    return 2;
}

int CPU::inc_h() {
    inc_r8(regs.h);
    return 1;
}

int CPU::dec_h() {
    dec_r8(regs.h);
    return 1;
}

int CPU::ld_h_d8() {
    ld_r8_r8(regs.h, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::daa() {
    // CHECK
    uint8_t a_tmp = regs.a;

    if ((regs.f & 0x20) || ((regs.a & 0xF) > 9)) {
        regs.a += 6; 
    }

    regs.f &= 0xEF;
    
    if ((regs.f & 0x20) || (a_tmp > 0x99)) { 
        regs.a += 0x60; 
        regs.f |= 0x10; 
    } 

    return 1;
}

int CPU::jr_z_r8() {
    if (regs.f & 0x80) {
        int offset = static_cast<int8_t>(mmu.read_byte(regs.pc)) + 2;
        regs.pc += offset - 1;
        return 3;
    } else {
        regs.pc += 1;
        return 2;
    }
}

int CPU::add_hl_hl() {
    add_hl_r16(regs.h, regs.l);
    return 2;
}

int CPU::ldi_a_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_r8_r8(regs.a, mmu.read_byte(addr));
    inc_hl();
    return 2;
}

int CPU::dec_hl() {
    dec_r16(regs.h, regs.l);
    return 2;
}
int CPU::inc_l() {
    inc_r8(regs.l);
    return 1;
}

int CPU::dec_l() {
    dec_r8(regs.l);
    return 1;
}

int CPU::ld_l_d8() {
    ld_r8_r8(regs.l, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::cpl() {
    regs.a ^= 0xFF;
    regs.f |= 0x60;
    return 1;
}

// 0x3
int CPU::jr_nc_r8() {
    if (!(regs.f & 0x10)) {
        int offset = static_cast<int8_t>(mmu.read_byte(regs.pc)) + 2;
        regs.pc += offset - 1;
        return 3;
    } else {
        regs.pc += 1;
        return 2;
    }
}

int CPU::ld_sp_d16() {
    regs.sp = mmu.read_word(regs.pc);
    regs.pc += 2;
    return 3;
}

int CPU::ldd_mhl_a() {
    ld_mhl_a();
    dec_hl();
    return 2;
}

int CPU::inc_sp() {
    ++regs.sp;
    return 2;
}

int CPU::inc_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);

    // CHECK
    inc_r8(val);

    mmu.write_byte(addr, val);
    return 3;
}

int CPU::dec_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);

    // CHECK
    dec_r8(val);

    mmu.write_byte(addr, val);
    return 3;
}

int CPU::ld_mhl_d8() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(regs.pc++);
    mmu.write_byte(addr, val);
    return 3;
}

int CPU::scf() {
    regs.f &= 0x80;
    regs.f |= 0x10;
    return 1;
}

int CPU::jr_c_r8() {
    if (regs.f & 0x10) {
        int8_t offset = static_cast<int8_t>(mmu.read_byte(regs.pc)) + 2;
        regs.pc += offset - 1;
        return 3;
    } else {
        regs.pc += 1;
        return 2;
    }
}

int CPU::add_hl_sp() {
    // CHECK
    uint8_t high = static_cast<uint8_t>(regs.sp >> 8);
    uint8_t low = static_cast<uint8_t>(regs.sp);
    add_hl_r16(high, low);
    return 2;
}

int CPU::ldd_a_mhl() {
    ld_a_mhl();
    dec_hl();
    return 2;
}

int CPU::dec_sp() {
    --regs.sp;
    return 2;
}

int CPU::inc_a() {
    inc_r8(regs.a);
    return 1;
}

int CPU::dec_a() {
    dec_r8(regs.a);
    return 1;
}

int CPU::ld_a_d8() {
    ld_r8_r8(regs.a, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::ccf() {
    regs.f &= 0x90;
    regs.f ^= 0x10;
    return 1;
}

// 0x4
int CPU::ld_b_b() {
    ld_r8_r8(regs.b, regs.b);
    return 1;
}

int CPU::ld_b_c() {
    ld_r8_r8(regs.b, regs.c);
    return 1;
}

int CPU::ld_b_d() {
    ld_r8_r8(regs.b, regs.d);
    return 1;
}

int CPU::ld_b_e() {
    ld_r8_r8(regs.b, regs.e);
    return 1;
}

int CPU::ld_b_h() {
    ld_r8_r8(regs.b, regs.h);
    return 1;
}

int CPU::ld_b_l() {
    ld_r8_r8(regs.b, regs.l);
    return 1;
}

int CPU::ld_b_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_r8_r8(regs.b, mmu.read_byte(addr));
    return 2;
}

int CPU::ld_b_a() {
    ld_r8_r8(regs.b, regs.a);
    return 1;
}

int CPU::ld_c_b() {
    ld_r8_r8(regs.c, regs.b);
    return 1;
}

int CPU::ld_c_c() {
    ld_r8_r8(regs.c, regs.c);
    return 1;
}

int CPU::ld_c_d() {
    ld_r8_r8(regs.c, regs.d);
    return 1;
}

int CPU::ld_c_e() {
    ld_r8_r8(regs.c, regs.e);
    return 1;
}

int CPU::ld_c_h() {
    ld_r8_r8(regs.c, regs.h);
    return 1;
}

int CPU::ld_c_l() {
    ld_r8_r8(regs.c, regs.l);
    return 1;
}

int CPU::ld_c_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_r8_r8(regs.c, mmu.read_byte(addr));
    return 2;
}

int CPU::ld_c_a() {
    ld_r8_r8(regs.c, regs.a);
    return 1;
}

// 0x5
int CPU::ld_d_b() {
    ld_r8_r8(regs.d, regs.b);
    return 1;
}

int CPU::ld_d_c() {
    ld_r8_r8(regs.d, regs.c);
    return 1;
}

int CPU::ld_d_d() {
    ld_r8_r8(regs.d, regs.d);
    return 1;
}

int CPU::ld_d_e() {
    ld_r8_r8(regs.d, regs.e);
    return 1;
}

int CPU::ld_d_h() {
    ld_r8_r8(regs.d, regs.h);
    return 1;
}

int CPU::ld_d_l() {
    ld_r8_r8(regs.d, regs.l);
    return 1;
}

int CPU::ld_d_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_r8_r8(regs.d, mmu.read_byte(addr));
    return 2;
}

int CPU::ld_d_a() {
    ld_r8_r8(regs.d, regs.a);
    return 1;
}

int CPU::ld_e_b() {
    ld_r8_r8(regs.e, regs.b);
    return 1;
}

int CPU::ld_e_c() {
    ld_r8_r8(regs.e, regs.c);
    return 1;
}

int CPU::ld_e_d() {
    ld_r8_r8(regs.e, regs.d);
    return 1;
}

int CPU::ld_e_e() {
    ld_r8_r8(regs.e, regs.e);
    return 1;
}

int CPU::ld_e_h() {
    ld_r8_r8(regs.e, regs.h);
    return 1;
}

int CPU::ld_e_l() {
    ld_r8_r8(regs.e, regs.l);
    return 1;
}

int CPU::ld_e_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_r8_r8(regs.e, mmu.read_byte(addr));
    return 2;
}

int CPU::ld_e_a() {
    ld_r8_r8(regs.e, regs.a);
    return 1;
}

// 0x6
int CPU::ld_h_b() {
    ld_r8_r8(regs.h, regs.b);
    return 1;
}

int CPU::ld_h_c() {
    ld_r8_r8(regs.h, regs.c);
    return 1;
}

int CPU::ld_h_d() {
    ld_r8_r8(regs.h, regs.d);
    return 1;
}

int CPU::ld_h_e() {
    ld_r8_r8(regs.h, regs.e);
    return 1;
}

int CPU::ld_h_h() {
    ld_r8_r8(regs.h, regs.h);
    return 1;
}

int CPU::ld_h_l() {
    ld_r8_r8(regs.h, regs.l);
    return 1;
}

int CPU::ld_h_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_r8_r8(regs.h, mmu.read_byte(addr));
    return 2;
}

int CPU::ld_h_a() {
    ld_r8_r8(regs.h, regs.a);
    return 1;
}

int CPU::ld_l_b() {
    ld_r8_r8(regs.l, regs.b);
    return 1;
}

int CPU::ld_l_c() {
    ld_r8_r8(regs.l, regs.c);
    return 1;
}

int CPU::ld_l_d() {
    ld_r8_r8(regs.l, regs.d);
    return 1;
}

int CPU::ld_l_e() {
    ld_r8_r8(regs.l, regs.e);
    return 1;
}

int CPU::ld_l_h() {
    ld_r8_r8(regs.l, regs.h);
    return 1;
}

int CPU::ld_l_l() {
    ld_r8_r8(regs.l, regs.l);
    return 1;
}

int CPU::ld_l_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_r8_r8(regs.l, mmu.read_byte(addr));
    return 2;
}

int CPU::ld_l_a() {
    ld_r8_r8(regs.l, regs.a);
    return 1;
}

// 0x7
int CPU::ld_mhl_b() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_mr_r8(addr, regs.b);
    return 2;
}

int CPU::ld_mhl_c() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_mr_r8(addr, regs.c);
    return 2;
}

int CPU::ld_mhl_d() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_mr_r8(addr, regs.d);
    return 2;
}

int CPU::ld_mhl_e() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_mr_r8(addr, regs.e);
    return 2;
}

int CPU::ld_mhl_h() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_mr_r8(addr, regs.h);
    return 2;
}

int CPU::ld_mhl_l() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_mr_r8(addr, regs.l);
    return 2;
}

int CPU::halt() {
    throw NotImplementedInstructionException();
    return 1;
}

int CPU::ld_mhl_a() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_mr_r8(addr, regs.a);
    return 2;
}
int CPU::ld_a_b() {
    ld_r8_r8(regs.a, regs.b);
    return 1;
}

int CPU::ld_a_c() {
    ld_r8_r8(regs.a, regs.c);
    return 1;
}

int CPU::ld_a_d() {
    ld_r8_r8(regs.a, regs.d);
    return 1;
}

int CPU::ld_a_e() {
    ld_r8_r8(regs.a, regs.e);
    return 1;
}

int CPU::ld_a_h() {
    ld_r8_r8(regs.a, regs.h);
    return 1;
}

int CPU::ld_a_l() {
    ld_r8_r8(regs.a, regs.l);
    return 1;
}

int CPU::ld_a_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    ld_r8_r8(regs.a, mmu.read_byte(addr));
    return 2;
}

int CPU::ld_a_a() {
    ld_r8_r8(regs.a, regs.a);
    return 1;
}

// 0x8
int CPU::add_a_b() {
    add_r8_r8(regs.a, regs.b);
    return 1;
}

int CPU::add_a_c() {
    add_r8_r8(regs.a, regs.c);
    return 1;
}

int CPU::add_a_d() {
    add_r8_r8(regs.a, regs.d);
    return 1;
}

int CPU::add_a_e() {
    add_r8_r8(regs.a, regs.e);
    return 1;
}

int CPU::add_a_h() {
    add_r8_r8(regs.a, regs.h);
    return 1;
}

int CPU::add_a_l() {
    add_r8_r8(regs.a, regs.l);
    return 1;
}

int CPU::add_a_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    add_r8_r8(regs.a, mmu.read_byte(addr));
    return 2;
}

int CPU::add_a_a() {
    add_r8_r8(regs.a, regs.a);
    return 1;
}

int CPU::adc_a_b() {
    adc_r8_r8(regs.a, regs.b);
    return 1;
}

int CPU::adc_a_c() {
    adc_r8_r8(regs.a, regs.c);
    return 1;
}

int CPU::adc_a_d() {
    adc_r8_r8(regs.a, regs.d);
    return 1;
}

int CPU::adc_a_e() {
    adc_r8_r8(regs.a, regs.e);
    return 1;
}

int CPU::adc_a_h() {
    adc_r8_r8(regs.a, regs.h);
    return 1;
}

int CPU::adc_a_l() {
    adc_r8_r8(regs.a, regs.l);
    return 1;
}

int CPU::adc_a_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    adc_r8_r8(regs.a, mmu.read_byte(addr));
    return 2;
}

int CPU::adc_a_a() {
    adc_r8_r8(regs.a, regs.a);
    return 1;
}

// 0x9
int CPU::sub_b() {
    sub_r8(regs.b);
    return 1;
}

int CPU::sub_c() {
    sub_r8(regs.c);
    return 1;
}

int CPU::sub_d() {
    sub_r8(regs.d);
    return 1;
}

int CPU::sub_e() {
    sub_r8(regs.e);
    return 1;
}

int CPU::sub_h() {
    sub_r8(regs.h);
    return 1;
}

int CPU::sub_l() {
    sub_r8(regs.l);
    return 1;
}

int CPU::sub_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    sub_r8(mmu.read_byte(addr));
    return 2;
}

int CPU::sub_a() {
    sub_r8(regs.a);
    return 1;
}

int CPU::sbc_a_b() {
    sbc_r8_r8(regs.a, regs.b);
    return 1;
}

int CPU::sbc_a_c() {
    sbc_r8_r8(regs.a, regs.c);
    return 1;
}

int CPU::sbc_a_d() {
    sbc_r8_r8(regs.a, regs.d);
    return 1;
}

int CPU::sbc_a_e() {
    sbc_r8_r8(regs.a, regs.e);
    return 1;
}

int CPU::sbc_a_h() {
    sbc_r8_r8(regs.a, regs.h);
    return 1;
}

int CPU::sbc_a_l() {
    sbc_r8_r8(regs.a, regs.l);
    return 1;
}

int CPU::sbc_a_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    sbc_r8_r8(regs.a, mmu.read_byte(addr));
    return 2;
}

int CPU::sbc_a_a() {
    sbc_r8_r8(regs.a, regs.a);
    return 1;
}

// 0xA
int CPU::and_b() {
    and_r8(regs.b);
    return 1;
}

int CPU::and_c() {
    and_r8(regs.c);
    return 1;
}

int CPU::and_d() {
    and_r8(regs.d);
    return 1;
}

int CPU::and_e() {
    and_r8(regs.e);
    return 1;
}

int CPU::and_h() {
    and_r8(regs.h);
    return 1;
}

int CPU::and_l() {
    and_r8(regs.l);
    return 1;
}

int CPU::and_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    and_r8(mmu.read_byte(addr));
    return 2;
}

int CPU::and_a() {
    and_r8(regs.a);
    return 1;
}

int CPU::xor_b() {
    xor_r8(regs.b);
    return 1;
}

int CPU::xor_c() {
    xor_r8(regs.c);
    return 1;
}

int CPU::xor_d() {
    xor_r8(regs.d);
    return 1;
}

int CPU::xor_e() {
    xor_r8(regs.e);
    return 1;
}

int CPU::xor_h() {
    xor_r8(regs.h);
    return 1;
}

int CPU::xor_l() {
    xor_r8(regs.l);
    return 1;
}

int CPU::xor_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    xor_r8(mmu.read_byte(addr));
    return 2;
}

int CPU::xor_a() {
    xor_r8(regs.a);
    return 1;
}

// 0xB
int CPU::or_b() {
    or_r8(regs.b);
    return 1;
}

int CPU::or_c() {
    or_r8(regs.c);
    return 1;
}

int CPU::or_d() {
    or_r8(regs.d);
    return 1;
}

int CPU::or_e() {
    or_r8(regs.e);
    return 1;
}

int CPU::or_h() {
    or_r8(regs.h);
    return 1;
}

int CPU::or_l() {
    or_r8(regs.l);
    return 1;
}

int CPU::or_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    or_r8(mmu.read_byte(addr));
    return 2;
}

int CPU::or_a() {
    or_r8(regs.a);
    return 1;
}

int CPU::cp_b() {
    cp_r8(regs.b);
    return 1;
}

int CPU::cp_c() {
    cp_r8(regs.c);
    return 1;
}

int CPU::cp_d() {
    cp_r8(regs.d);
    return 1;
}

int CPU::cp_e() {
    cp_r8(regs.e);
    return 1;
}

int CPU::cp_h() {
    cp_r8(regs.h);
    return 1;
}

int CPU::cp_l() {
    cp_r8(regs.l);
    return 1;
}

int CPU::cp_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    cp_r8(mmu.read_byte(addr));
    return 2;
}

int CPU::cp_a() {
    cp_r8(regs.a);
    return 1;
}

// 0xC
int CPU::ret_nz() {
    if (!(regs.f & 0x80)) {
        regs.pc = mmu.read_word(regs.sp);
        regs.sp += 2;
        return 5;
    } else {
        return 2;
    }
}

int CPU::pop_bc() {
    pop_r16(regs.b, regs.c);
    return 3;
}

int CPU::jp_nz_a16() {
    if (!(regs.f & 0x80)) {
        regs.pc = mmu.read_word(regs.pc);
        return 4;
    } else {
        regs.pc += 2;
        return 3;
    }
}

int CPU::jp_a16() {
    regs.pc = mmu.read_word(regs.pc);
    return 4;
}

int CPU::call_nz_a16() {
    if (!(regs.f & 0x80)) {
        mmu.write_word(regs.sp - 2, regs.pc + 2);
        regs.sp -= 2;
        regs.pc = mmu.read_word(regs.pc);
        return 6;
    } else {
        regs.pc += 2;
        return 3;
    }
}

int CPU::push_bc() {
    push_r16(regs.b, regs.c);
    return 4;
}

int CPU::add_a_d8() {
    add_r8_r8(regs.a, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rst_00h() {
    rst(0x00);
    return 4;
}

int CPU::ret_z() {
    if (regs.f & 0x80) {
        regs.pc = mmu.read_word(regs.sp);
        regs.sp += 2;
        return 5;
    } else {
        return 2;
    }
}

int CPU::ret() {
    regs.pc = mmu.read_word(regs.sp);
    regs.sp += 2;
    return 4;
}

int CPU::jp_z_a16() {
    if (regs.f & 0x80) {
        regs.pc = mmu.read_word(regs.pc);
        return 4;
    } else {
        regs.pc += 2;
        return 3;
    }
}

int CPU::prefix_cb() {
    return instructions[0x100 + mmu.read_byte(regs.pc++)]();
}

int CPU::call_z_a16() {
    if (regs.f & 0x80) {
        mmu.write_word(regs.sp - 2, regs.pc + 2);
        regs.sp -= 2;
        regs.pc = mmu.read_word(regs.pc);
        return 6;
    } else {
        regs.pc += 2;
        return 3;
    }
}

int CPU::call_a16() {
    mmu.write_word(regs.sp - 2, regs.pc + 2);
    regs.sp -= 2;
    regs.pc = mmu.read_word(regs.pc);
    return 6;
}

int CPU::adc_a_d8() {
    adc_r8_r8(regs.a, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rst_08h() {
    rst(0x08);
    return 4;
}

// 0xD
int CPU::ret_nc() {
    if (!(regs.f & 0x10)) {
        regs.pc = mmu.read_word(regs.sp);
        regs.sp += 2;
        return 5;
    } else {
        return 2;
    }
}

int CPU::pop_de() {
    pop_r16(regs.d, regs.e);
    return 3;
}

int CPU::jp_nc_a16() {
    if (!(regs.f & 0x10)) {
        regs.pc = mmu.read_word(regs.pc);
        return 4;
    } else {
        regs.pc += 2;
        return 3;
    }
}
//
int CPU::call_nc_a16() {
    if (!(regs.f & 0x10)) {
        mmu.write_word(regs.sp - 2, regs.pc + 2);
        regs.sp -= 2;
        regs.pc = mmu.read_word(regs.pc);
        return 6;
    } else {
        regs.pc += 2;
        return 3;
    }
}

int CPU::push_de() {
    push_r16(regs.d, regs.e);
    return 4;
}

int CPU::sub_d8() {
    sub_r8(mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rst_10h() {
    rst(0x10);
    return 4;
}

int CPU::ret_c() {
    if (regs.f & 0x10) {
        regs.pc = mmu.read_word(regs.sp);
        regs.sp += 2;
        return 5;
    } else {
        return 2;
    }
}

int CPU::reti() {
    regs.pc = mmu.read_word(regs.sp);
    regs.sp += 2;
    // TODO enable interrupts
    throw NotImplementedInstructionException();
    return 4;
}

int CPU::jp_c_a16() {
    if (regs.f & 0x10) {
        regs.pc = mmu.read_word(regs.pc);
        return 4;
    } else {
        regs.pc += 2;
        return 3;
    }
}
//
int CPU::call_c_a16() {
    if (regs.f & 0x10) {
        mmu.write_word(regs.sp - 2, regs.pc + 2);
        regs.sp -= 2;
        regs.pc = mmu.read_word(regs.pc);
        return 6;
    } else {
        regs.pc += 2;
        return 3;
    }
}
//
int CPU::sbc_a_d8() {
    sbc_r8_r8(regs.a, mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rst_18h() {
    rst(0x18);
    return 4;
}

// 0xE
int CPU::ldh_ma8_a() {
    uint8_t val = mmu.read_byte(regs.pc++);
    mmu.write_byte(0xFF00 + val, regs.a);
    return 3;
}

int CPU::pop_hl() {
    pop_r16(regs.h, regs.l);
    return 3;
}

int CPU::ld_mc_a() {
    mmu.write_byte(0xFF00 + regs.c, regs.a);
    return 3;
}
//
//
int CPU::push_hl() {
    push_r16(regs.h, regs.l);
    return 4;
}

int CPU::and_d8() {
    and_r8(mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rst_20h() {
    rst(0x20);
    return 4;
}

int CPU::add_sp_r8() {
    regs.f = 0;
    int8_t val = static_cast<int8_t>(mmu.read_byte(regs.pc++));

    if ((regs.sp & 0xFF) + val > 0xFF) {
        regs.f |= ICpu::C_FLAG; 
    }

    if ((regs.sp & 0xF) + (val & 0xF) > 0xF) {
        regs.f |= ICpu::H_FLAG;
    }

    regs.sp += val;
    return 4;
}

int CPU::jp_mhl() {
    regs.pc = (regs.h >> 8) + regs.l;
    return 1;
}

int CPU::ld_ma16_a() {
    uint16_t val = mmu.read_word(regs.pc);
    mmu.write_byte(val, regs.a);
    regs.pc += 2;
    return 4;
}
//
//
//
int CPU::xor_d8() {
    xor_r8(mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rst_28h() {
    rst(0x28);
    return 4;
}

// 0xF
int CPU::ldh_a_ma8() {
    uint8_t val = mmu.read_byte(regs.pc++);
    regs.a = mmu.read_byte(0xFF00 + val);
    return 3;
}

int CPU::pop_af() {
    pop_r16(regs.a, regs.f);
    return 3;
}

int CPU::ld_a_mc() {
    // not used?
    regs.a = mmu.read_byte(0xFF00 + regs.c);
    return 2;
}

int CPU::di() {
    // TODO
    throw NotImplementedInstructionException();
    return 1;
}
//
int CPU::push_af() {
    push_r16(regs.a, regs.f);
    return 4;
}

int CPU::or_d8() {
    or_r8(mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rst_30h() {
    rst(0x30);
    return 4;
}

int CPU::ldhl_sp_r8() {
    regs.f = 0;
    int8_t val = static_cast<int8_t>(mmu.read_byte(regs.pc++));

    if ((regs.sp & 0xFF) + val > 0xFF) {
        regs.f |= ICpu::C_FLAG;
    }

    if ((regs.sp & 0xF) + (val & 0xF) > 0xF) {
        regs.f |= ICpu::H_FLAG;
    }

    regs.h = (regs.sp + val) >> 8;
    regs.l = static_cast<uint8_t>(regs.sp + val);
    return 3;
}
int CPU::ld_sp_hl() {
    regs.sp = (regs.h << 8) + regs.l;
    return 2;
}

int CPU::ld_a_ma16() {
    regs.a = mmu.read_byte(mmu.read_word(regs.pc));
    regs.pc += 2;
    return 4;
}

int CPU::ei() {
    // TODO
    throw NotImplementedInstructionException();
    return 1;
}
//
//
int CPU::cp_d8() {
    cp_r8(mmu.read_byte(regs.pc++));
    return 2;
}

int CPU::rst_38h() {
    rst(0x38);
    return 4;
}
//
int CPU::unimplemented() {
    throw UndefinedInstructionException();
    return 0;
}

int CPU::rlc_b() {
    rlc_r8(regs.b);
    return 2;
}

int CPU::rlc_c() {
    rlc_r8(regs.c);
    return 2;
}

int CPU::rlc_d() {
    rlc_r8(regs.d);
    return 2;
}

int CPU::rlc_e() {
    rlc_r8(regs.e);
    return 2;
}

int CPU::rlc_h() {
    rlc_r8(regs.h);
    return 2;
}

int CPU::rlc_l() {
    rlc_r8(regs.l);
    return 2;
}

int CPU::rlc_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    rlc_r8(val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::rlc_a() {
    rlc_r8(regs.a);
    return 2;
}

int CPU::rrc_b() {
    rrc_r8(regs.b);
    return 2;
}

int CPU::rrc_c() {
    rrc_r8(regs.c);
    return 2;
}

int CPU::rrc_d() {
    rrc_r8(regs.d);
    return 2;
}

int CPU::rrc_e() {
    rrc_r8(regs.e);
    return 2;
}

int CPU::rrc_h() {
    rrc_r8(regs.h);
    return 2;
}

int CPU::rrc_l() {
    rrc_r8(regs.l);
    return 2;
}

int CPU::rrc_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    rrc_r8(val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::rrc_a() {
    rrc_r8(regs.a);
    return 2;
}

int CPU::rl_b() {
    rl_r8(regs.b);
    return 2;
}

int CPU::rl_c() {
    rl_r8(regs.c);
    return 2;
}

int CPU::rl_d() {
    rl_r8(regs.d);
    return 2;
}

int CPU::rl_e() {
    rl_r8(regs.e);
    return 2;
}

int CPU::rl_h() {
    rl_r8(regs.h);
    return 2;
}

int CPU::rl_l() {
    rl_r8(regs.l);
    return 2;
}

int CPU::rl_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    rl_r8(val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::rl_a() {
    rl_r8(regs.a);
    return 2;
}

int CPU::rr_b() {
    rr_r8(regs.b);
    return 2;
}

int CPU::rr_c() {
    rr_r8(regs.c);
    return 2;
}

int CPU::rr_d() {
    rr_r8(regs.d);
    return 2;
}

int CPU::rr_e() {
    rr_r8(regs.e);
    return 2;
}

int CPU::rr_h() {
    rr_r8(regs.h);
    return 2;
}

int CPU::rr_l() {
    rr_r8(regs.l);
    return 2;
}

int CPU::rr_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    rr_r8(val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::rr_a() {
    rr_r8(regs.a);
    return 2;
}

int CPU::sla_b() {
    sla_r8(regs.b);
    return 2;
}

int CPU::sla_c() {
    sla_r8(regs.c);
    return 2;
}

int CPU::sla_d() {
    sla_r8(regs.d);
    return 2;
}

int CPU::sla_e() {
    sla_r8(regs.e);
    return 2;
}

int CPU::sla_h() {
    sla_r8(regs.h);
    return 2;
}

int CPU::sla_l() {
    sla_r8(regs.l);
    return 2;
}

int CPU::sla_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    sla_r8(val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::sla_a() {
    sla_r8(regs.a);
    return 2;
}

int CPU::sra_b() {
    sra_r8(regs.b);
    return 2;
}

int CPU::sra_c() {
    sra_r8(regs.c);
    return 2;
}

int CPU::sra_d() {
    sra_r8(regs.d);
    return 2;
}

int CPU::sra_e() {
    sra_r8(regs.e);
    return 2;
}

int CPU::sra_h() {
    sra_r8(regs.h);
    return 2;
}

int CPU::sra_l() {
    sra_r8(regs.l);
    return 2;
}

int CPU::sra_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    sra_r8(val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::sra_a() {
    sra_r8(regs.a);
    return 2;
}

int CPU::swap_b() {
    swap_r8(regs.b);
    return 2;
}

int CPU::swap_c() {
    swap_r8(regs.c);
    return 2;
}

int CPU::swap_d() {
    swap_r8(regs.d);
    return 2;
}

int CPU::swap_e() {
    swap_r8(regs.e);
    return 2;
}

int CPU::swap_h() {
    swap_r8(regs.h);
    return 2;
}

int CPU::swap_l() {
    swap_r8(regs.l);
    return 2;
}

int CPU::swap_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    swap_r8(val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::swap_a() {
    swap_r8(regs.a);
    return 2;
}

int CPU::srl_b() {
    srl_r8(regs.b);
    return 2;
}

int CPU::srl_c() {
    srl_r8(regs.c);
    return 2;
}

int CPU::srl_d() {
    srl_r8(regs.d);
    return 2;
}

int CPU::srl_e() {
    srl_r8(regs.e);
    return 2;
}

int CPU::srl_h() {
    srl_r8(regs.h);
    return 2;
}

int CPU::srl_l() {
    srl_r8(regs.l);
    return 2;
}

int CPU::srl_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    srl_r8(val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::srl_a() {
    srl_r8(regs.a);
    return 2;
}

int CPU::bit_0_b() {
    bit_b_r8(0, regs.b);
    return 2;
}

int CPU::bit_0_c() {
    bit_b_r8(0, regs.c);
    return 2;
}

int CPU::bit_0_d() {
    bit_b_r8(0, regs.d);
    return 2;
}

int CPU::bit_0_e() {
    bit_b_r8(0, regs.e);
    return 2;
}

int CPU::bit_0_h() {
    bit_b_r8(0, regs.h);
    return 2;
}

int CPU::bit_0_l() {
    bit_b_r8(0, regs.l);
    return 2;
}

int CPU::bit_0_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    bit_b_r8(0, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::bit_0_a() {
    bit_b_r8(0, regs.a);
    return 2;
}

int CPU::bit_1_b() {
    bit_b_r8(1, regs.b);
    return 2;
}

int CPU::bit_1_c() {
    bit_b_r8(1, regs.c);
    return 2;
}

int CPU::bit_1_d() {
    bit_b_r8(1, regs.d);
    return 2;
}

int CPU::bit_1_e() {
    bit_b_r8(1, regs.e);
    return 2;
}

int CPU::bit_1_h() {
    bit_b_r8(1, regs.h);
    return 2;
}

int CPU::bit_1_l() {
    bit_b_r8(1, regs.l);
    return 2;
}

int CPU::bit_1_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    bit_b_r8(1, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::bit_1_a() {
    bit_b_r8(1, regs.a);
    return 2;
}

int CPU::bit_2_b() {
    bit_b_r8(2, regs.b);
    return 2;
}

int CPU::bit_2_c() {
    bit_b_r8(2, regs.c);
    return 2;
}

int CPU::bit_2_d() {
    bit_b_r8(2, regs.d);
    return 2;
}

int CPU::bit_2_e() {
    bit_b_r8(2, regs.e);
    return 2;
}

int CPU::bit_2_h() {
    bit_b_r8(2, regs.h);
    return 2;
}

int CPU::bit_2_l() {
    bit_b_r8(2, regs.l);
    return 2;
}

int CPU::bit_2_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    bit_b_r8(2, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::bit_2_a() {
    bit_b_r8(2, regs.a);
    return 2;
}

int CPU::bit_3_b() {
    bit_b_r8(3, regs.b);
    return 2;
}

int CPU::bit_3_c() {
    bit_b_r8(3, regs.c);
    return 2;
}

int CPU::bit_3_d() {
    bit_b_r8(3, regs.d);
    return 2;
}

int CPU::bit_3_e() {
    bit_b_r8(3, regs.e);
    return 2;
}

int CPU::bit_3_h() {
    bit_b_r8(3, regs.h);
    return 2;
}

int CPU::bit_3_l() {
    bit_b_r8(3, regs.l);
    return 2;
}

int CPU::bit_3_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    bit_b_r8(3, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::bit_3_a() {
    bit_b_r8(3, regs.a);
    return 2;
}

int CPU::bit_4_b() {
    bit_b_r8(4, regs.b);
    return 2;
}

int CPU::bit_4_c() {
    bit_b_r8(4, regs.c);
    return 2;
}

int CPU::bit_4_d() {
    bit_b_r8(4, regs.d);
    return 2;
}

int CPU::bit_4_e() {
    bit_b_r8(4, regs.e);
    return 2;
}

int CPU::bit_4_h() {
    bit_b_r8(4, regs.h);
    return 2;
}

int CPU::bit_4_l() {
    bit_b_r8(4, regs.l);
    return 2;
}

int CPU::bit_4_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    bit_b_r8(4, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::bit_4_a() {
    bit_b_r8(4, regs.a);
    return 2;
}

int CPU::bit_5_b() {
    bit_b_r8(5, regs.b);
    return 2;
}

int CPU::bit_5_c() {
    bit_b_r8(5, regs.c);
    return 2;
}

int CPU::bit_5_d() {
    bit_b_r8(5, regs.d);
    return 2;
}

int CPU::bit_5_e() {
    bit_b_r8(5, regs.e);
    return 2;
}

int CPU::bit_5_h() {
    bit_b_r8(5, regs.h);
    return 2;
}

int CPU::bit_5_l() {
    bit_b_r8(5, regs.l);
    return 2;
}

int CPU::bit_5_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    bit_b_r8(5, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::bit_5_a() {
    bit_b_r8(5, regs.a);
    return 2;
}

int CPU::bit_6_b() {
    bit_b_r8(6, regs.b);
    return 2;
}

int CPU::bit_6_c() {
    bit_b_r8(6, regs.c);
    return 2;
}

int CPU::bit_6_d() {
    bit_b_r8(6, regs.d);
    return 2;
}

int CPU::bit_6_e() {
    bit_b_r8(6, regs.e);
    return 2;
}

int CPU::bit_6_h() {
    bit_b_r8(6, regs.h);
    return 2;
}

int CPU::bit_6_l() {
    bit_b_r8(6, regs.l);
    return 2;
}

int CPU::bit_6_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    bit_b_r8(6, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::bit_6_a() {
    bit_b_r8(6, regs.a);
    return 2;
}

int CPU::bit_7_b() {
    bit_b_r8(7, regs.b);
    return 2;
}

int CPU::bit_7_c() {
    bit_b_r8(7, regs.c);
    return 2;
}

int CPU::bit_7_d() {
    bit_b_r8(7, regs.d);
    return 2;
}

int CPU::bit_7_e() {
    bit_b_r8(7, regs.e);
    return 2;
}

int CPU::bit_7_h() {
    bit_b_r8(7, regs.h);
    return 2;
}

int CPU::bit_7_l() {
    bit_b_r8(7, regs.l);
    return 2;
}

int CPU::bit_7_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    bit_b_r8(7, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::bit_7_a() {
    bit_b_r8(7, regs.a);
    return 2;
}

int CPU::res_0_b() {
    res_b_r8(0, regs.b);
    return 2;
}

int CPU::res_0_c() {
    res_b_r8(0, regs.c);
    return 2;
}

int CPU::res_0_d() {
    res_b_r8(0, regs.d);
    return 2;
}

int CPU::res_0_e() {
    res_b_r8(0, regs.e);
    return 2;
}

int CPU::res_0_h() {
    res_b_r8(0, regs.h);
    return 2;
}

int CPU::res_0_l() {
    res_b_r8(0, regs.l);
    return 2;
}

int CPU::res_0_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    res_b_r8(0, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::res_0_a() {
    res_b_r8(0, regs.a);
    return 2;
}

int CPU::res_1_b() {
    res_b_r8(1, regs.b);
    return 2;
}

int CPU::res_1_c() {
    res_b_r8(1, regs.c);
    return 2;
}

int CPU::res_1_d() {
    res_b_r8(1, regs.d);
    return 2;
}

int CPU::res_1_e() {
    res_b_r8(1, regs.e);
    return 2;
}

int CPU::res_1_h() {
    res_b_r8(1, regs.h);
    return 2;
}

int CPU::res_1_l() {
    res_b_r8(1, regs.l);
    return 2;
}

int CPU::res_1_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    res_b_r8(1, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::res_1_a() {
    res_b_r8(1, regs.a);
    return 2;
}

int CPU::res_2_b() {
    res_b_r8(2, regs.b);
    return 2;
}

int CPU::res_2_c() {
    res_b_r8(2, regs.c);
    return 2;
}

int CPU::res_2_d() {
    res_b_r8(2, regs.d);
    return 2;
}

int CPU::res_2_e() {
    res_b_r8(2, regs.e);
    return 2;
}

int CPU::res_2_h() {
    res_b_r8(2, regs.h);
    return 2;
}

int CPU::res_2_l() {
    res_b_r8(2, regs.l);
    return 2;
}

int CPU::res_2_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    res_b_r8(2, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::res_2_a() {
    res_b_r8(2, regs.a);
    return 2;
}

int CPU::res_3_b() {
    res_b_r8(3, regs.b);
    return 2;
}

int CPU::res_3_c() {
    res_b_r8(3, regs.c);
    return 2;
}

int CPU::res_3_d() {
    res_b_r8(3, regs.d);
    return 2;
}

int CPU::res_3_e() {
    res_b_r8(3, regs.e);
    return 2;
}

int CPU::res_3_h() {
    res_b_r8(3, regs.h);
    return 2;
}

int CPU::res_3_l() {
    res_b_r8(3, regs.l);
    return 2;
}

int CPU::res_3_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    res_b_r8(3, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::res_3_a() {
    res_b_r8(3, regs.a);
    return 2;
}

int CPU::res_4_b() {
    res_b_r8(4, regs.b);
    return 2;
}

int CPU::res_4_c() {
    res_b_r8(4, regs.c);
    return 2;
}

int CPU::res_4_d() {
    res_b_r8(4, regs.d);
    return 2;
}

int CPU::res_4_e() {
    res_b_r8(4, regs.e);
    return 2;
}

int CPU::res_4_h() {
    res_b_r8(4, regs.h);
    return 2;
}

int CPU::res_4_l() {
    res_b_r8(4, regs.l);
    return 2;
}

int CPU::res_4_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    res_b_r8(4, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::res_4_a() {
    res_b_r8(4, regs.a);
    return 2;
}

int CPU::res_5_b() {
    res_b_r8(5, regs.b);
    return 2;
}

int CPU::res_5_c() {
    res_b_r8(5, regs.c);
    return 2;
}

int CPU::res_5_d() {
    res_b_r8(5, regs.d);
    return 2;
}

int CPU::res_5_e() {
    res_b_r8(5, regs.e);
    return 2;
}

int CPU::res_5_h() {
    res_b_r8(5, regs.h);
    return 2;
}

int CPU::res_5_l() {
    res_b_r8(5, regs.l);
    return 2;
}

int CPU::res_5_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    res_b_r8(5, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::res_5_a() {
    res_b_r8(5, regs.a);
    return 2;
}

int CPU::res_6_b() {
    res_b_r8(6, regs.b);
    return 2;
}

int CPU::res_6_c() {
    res_b_r8(6, regs.c);
    return 2;
}

int CPU::res_6_d() {
    res_b_r8(6, regs.d);
    return 2;
}

int CPU::res_6_e() {
    res_b_r8(6, regs.e);
    return 2;
}

int CPU::res_6_h() {
    res_b_r8(6, regs.h);
    return 2;
}

int CPU::res_6_l() {
    res_b_r8(6, regs.l);
    return 2;
}

int CPU::res_6_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    res_b_r8(6, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::res_6_a() {
    res_b_r8(6, regs.a);
    return 2;
}

int CPU::res_7_b() {
    res_b_r8(7, regs.b);
    return 2;
}

int CPU::res_7_c() {
    res_b_r8(7, regs.c);
    return 2;
}

int CPU::res_7_d() {
    res_b_r8(7, regs.d);
    return 2;
}

int CPU::res_7_e() {
    res_b_r8(7, regs.e);
    return 2;
}

int CPU::res_7_h() {
    res_b_r8(7, regs.h);
    return 2;
}

int CPU::res_7_l() {
    res_b_r8(7, regs.l);
    return 2;
}

int CPU::res_7_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    res_b_r8(7, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::res_7_a() {
    res_b_r8(7, regs.a);
    return 2;
}

int CPU::set_0_b() {
    set_b_r8(0, regs.b);
    return 2;
}

int CPU::set_0_c() {
    set_b_r8(0, regs.c);
    return 2;
}

int CPU::set_0_d() {
    set_b_r8(0, regs.d);
    return 2;
}

int CPU::set_0_e() {
    set_b_r8(0, regs.e);
    return 2;
}

int CPU::set_0_h() {
    set_b_r8(0, regs.h);
    return 2;
}

int CPU::set_0_l() {
    set_b_r8(0, regs.l);
    return 2;
}

int CPU::set_0_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    set_b_r8(0, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::set_0_a() {
    set_b_r8(0, regs.a);
    return 2;
}

int CPU::set_1_b() {
    set_b_r8(1, regs.b);
    return 2;
}

int CPU::set_1_c() {
    set_b_r8(1, regs.c);
    return 2;
}

int CPU::set_1_d() {
    set_b_r8(1, regs.d);
    return 2;
}

int CPU::set_1_e() {
    set_b_r8(1, regs.e);
    return 2;
}

int CPU::set_1_h() {
    set_b_r8(1, regs.h);
    return 2;
}

int CPU::set_1_l() {
    set_b_r8(1, regs.l);
    return 2;
}

int CPU::set_1_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    set_b_r8(1, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::set_1_a() {
    set_b_r8(1, regs.a);
    return 2;
}

int CPU::set_2_b() {
    set_b_r8(2, regs.b);
    return 2;
}

int CPU::set_2_c() {
    set_b_r8(2, regs.c);
    return 2;
}

int CPU::set_2_d() {
    set_b_r8(2, regs.d);
    return 2;
}

int CPU::set_2_e() {
    set_b_r8(2, regs.e);
    return 2;
}

int CPU::set_2_h() {
    set_b_r8(2, regs.h);
    return 2;
}

int CPU::set_2_l() {
    set_b_r8(2, regs.l);
    return 2;
}

int CPU::set_2_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    set_b_r8(2, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::set_2_a() {
    set_b_r8(2, regs.a);
    return 2;
}

int CPU::set_3_b() {
    set_b_r8(3, regs.b);
    return 2;
}

int CPU::set_3_c() {
    set_b_r8(3, regs.c);
    return 2;
}

int CPU::set_3_d() {
    set_b_r8(3, regs.d);
    return 2;
}

int CPU::set_3_e() {
    set_b_r8(3, regs.e);
    return 2;
}

int CPU::set_3_h() {
    set_b_r8(3, regs.h);
    return 2;
}

int CPU::set_3_l() {
    set_b_r8(3, regs.l);
    return 2;
}

int CPU::set_3_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    set_b_r8(3, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::set_3_a() {
    set_b_r8(3, regs.a);
    return 2;
}

int CPU::set_4_b() {
    set_b_r8(4, regs.b);
    return 2;
}

int CPU::set_4_c() {
    set_b_r8(4, regs.c);
    return 2;
}

int CPU::set_4_d() {
    set_b_r8(4, regs.d);
    return 2;
}

int CPU::set_4_e() {
    set_b_r8(4, regs.e);
    return 2;
}

int CPU::set_4_h() {
    set_b_r8(4, regs.h);
    return 2;
}

int CPU::set_4_l() {
    set_b_r8(4, regs.l);
    return 2;
}

int CPU::set_4_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    set_b_r8(4, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::set_4_a() {
    set_b_r8(4, regs.a);
    return 2;
}

int CPU::set_5_b() {
    set_b_r8(5, regs.b);
    return 2;
}

int CPU::set_5_c() {
    set_b_r8(5, regs.c);
    return 2;
}

int CPU::set_5_d() {
    set_b_r8(5, regs.d);
    return 2;
}

int CPU::set_5_e() {
    set_b_r8(5, regs.e);
    return 2;
}

int CPU::set_5_h() {
    set_b_r8(5, regs.h);
    return 2;
}

int CPU::set_5_l() {
    set_b_r8(5, regs.l);
    return 2;
}

int CPU::set_5_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    set_b_r8(5, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::set_5_a() {
    set_b_r8(5, regs.a);
    return 2;
}

int CPU::set_6_b() {
    set_b_r8(6, regs.b);
    return 2;
}

int CPU::set_6_c() {
    set_b_r8(6, regs.c);
    return 2;
}

int CPU::set_6_d() {
    set_b_r8(6, regs.d);
    return 2;
}

int CPU::set_6_e() {
    set_b_r8(6, regs.e);
    return 2;
}

int CPU::set_6_h() {
    set_b_r8(6, regs.h);
    return 2;
}

int CPU::set_6_l() {
    set_b_r8(6, regs.l);
    return 2;
}

int CPU::set_6_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    set_b_r8(6, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::set_6_a() {
    set_b_r8(6, regs.a);
    return 2;
}

int CPU::set_7_b() {
    set_b_r8(7, regs.b);
    return 2;
}

int CPU::set_7_c() {
    set_b_r8(7, regs.c);
    return 2;
}

int CPU::set_7_d() {
    set_b_r8(7, regs.d);
    return 2;
}

int CPU::set_7_e() {
    set_b_r8(7, regs.e);
    return 2;
}

int CPU::set_7_h() {
    set_b_r8(7, regs.h);
    return 2;
}

int CPU::set_7_l() {
    set_b_r8(7, regs.l);
    return 2;
}

int CPU::set_7_mhl() {
    uint16_t addr = (regs.h << 8) + regs.l;
    uint8_t val = mmu.read_byte(addr);
    set_b_r8(7, val);
    mmu.write_byte(addr, val);
    return 4;
}

int CPU::set_7_a() {
    set_b_r8(7, regs.a);
    return 2;
}


}
