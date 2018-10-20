#include "core/mmu.h"

#include "utils/logger.h"

#include <fstream>
#include <iostream>

MMU::MMU(GPU& gpu_in, Input& input_in, const std::string& bios_file, const std::string& rom_file) : 
    gpu(gpu_in), 
    input(input_in),
    in_bios{true},
    bios{},
    rom{},
    eram{},
    wram{},
    hram{} {

    // TODO helper function for reading files
    std::ifstream ifs(bios_file);

    if (!ifs) {
        std::cout << "Could not open bios file" << std::endl;
    } else {
        int byte;
        for (int i = 0; (byte = ifs.get()) != EOF; i++) {
            bios[i] = static_cast<uint8_t>(byte);
        }
    }

    ifs.close();

    std::ifstream ifs2(rom_file);
    if (!ifs2) {
        std::cout << "Could not open rom file" << std::endl;
        exit(1);
    }

    int byte;
    for (int i = 0; (byte = ifs2.get()) != EOF; i++) {
        rom[i] = static_cast<uint8_t>(byte);
    }

    ifs2.close();
}

uint8_t MMU::read_byte(uint16_t addr) {
    switch (get_area(addr)) {
    case AREA_BIOS: return bios[addr];
    case AREA_ROM0: return rom[addr];
    case AREA_ROM1: return rom[addr];
    case AREA_VRAM: 
        LOG("Unimplemented read_byte:AREA_WRAM: %x\n", static_cast<unsigned>(addr));
        return 0;
    case AREA_ERAM: return eram[addr & 0x1FFF];
    case AREA_WRAM: return wram[addr & 0x1FFF];
    case AREA_OAM:
        return gpu.read_byte_oam(addr - 0xFE00);
    case AREA_UNUSED: 
        LOG("Unimplemented read_byte:AREA_UNUSED: %x\n", static_cast<unsigned>(addr));
        return 0;
    case AREA_IO:
        switch (addr) {
        case JOYPAD_REG: return input.get_buttons_pressed();
        case GPU_REG_LCD_CONTROL: return gpu.get_lcd_control();
        case GPU_REG_SCROLL_Y: return gpu.get_scroll_y();
        case GPU_REG_SCROLL_X: return gpu.get_scroll_x();
        case GPU_REG_CURR_SCANLINE: return gpu.get_curr_scanline();
        case GPU_REG_OBJ_PALETTE_0: return gpu.get_obj_palette(0);
        case GPU_REG_OBJ_PALETTE_1: return gpu.get_obj_palette(1);
        default: 
            LOG("Unimplemented read_byte:AREA_IO: %x\n", static_cast<unsigned>(addr));
            return 0;
        }
    case AREA_HRAM: return hram[addr & 0x7F];
    case AREA_IE_REG: 
        LOG("Unimplemented read_byte:AREA_IE_REG: %x\n", static_cast<unsigned>(addr));
        return 0;
    default: 
        LOG("Unimplemented read_byte:INVALID: %x\n", static_cast<unsigned>(addr));
        return 0;
    }
}

uint16_t MMU::read_word(uint16_t addr) {
    switch (get_area(addr)) {
    case AREA_BIOS: return bios[addr] + (bios[addr + 1] << 8);
    case AREA_ROM0: return rom[addr] + (rom[addr + 1] << 8);
    case AREA_ROM1: return rom[addr] + (rom[addr + 1] << 8);
    case AREA_VRAM: 
        LOG("Unimplemented read_word:AREA_WRAM: %x\n", static_cast<unsigned>(addr));
        return 0;
    case AREA_ERAM: return eram[addr & 0x2000] + ((eram[addr + 1] & 0x2000) << 8);
    case AREA_WRAM: return wram[addr & 0x2000] + ((wram[addr + 1] & 0x2000) << 8);
    case AREA_OAM: 
        return (gpu.read_byte_oam(addr - 0xFE00 + 1) << 8) + gpu.read_byte_oam(addr - 0xFE00);
        //LOG("Unimplemented read_word:AREA_OAM: %x\n", static_cast<unsigned>(addr));
        //return 0;
    case AREA_UNUSED: 
        LOG("Unimplemented read_word:AREA_UNUSED: %x\n", static_cast<unsigned>(addr));
        return 0;
    case AREA_IO: 
        LOG("Unimplemented read_word:AREA_IO: %x\n", static_cast<unsigned>(addr));
        return 0;
    case AREA_HRAM: return hram[addr & 0x7F] + (hram[(addr & 0x7F) + 1] << 8);
    case AREA_IE_REG: 
        LOG("Unimplemented read_word:AREA_IE_REG: %x\n", static_cast<unsigned>(addr));
        return 0;
    default: 
        LOG("Unimplemented read_word:INVALID: %x\n", static_cast<unsigned>(addr));
        return 0;
    }
}

void MMU::write_byte(uint16_t addr, uint8_t val) {
    switch (get_area(addr)) {
    case AREA_BIOS: bios[addr] = val; break;
    case AREA_ROM0: rom[addr] = val; break;
    case AREA_ROM1: rom[addr] = val; break;
    case AREA_VRAM: gpu.write_byte_vram(addr - 0x8000, val); break;
    case AREA_ERAM: eram[addr & 0x1FFF] = val; break;
    case AREA_WRAM: wram[addr & 0x1FFF] = val; break;
    case AREA_OAM: gpu.write_byte_oam(addr - 0xFE00, val); break;
    case AREA_UNUSED: break;
    case AREA_IO: 
        switch (addr) {
        case JOYPAD_REG: input.set_buttons_pressed_switch(val); break;
        case GPU_REG_LCD_CONTROL: gpu.set_lcd_control(val); break;
        case GPU_REG_SCROLL_Y: gpu.set_scroll_y(val); break;
        case GPU_REG_SCROLL_X: gpu.set_scroll_x(val); break;
        case GPU_REG_BG_PALETTE: gpu.set_bg_palette(val); break;
        case GPU_REG_OBJ_PALETTE_0: gpu.set_obj_palette(0, val); break;
        case GPU_REG_OBJ_PALETTE_1: gpu.set_obj_palette(1, val); break;
        }
        break;
    case AREA_HRAM: hram[addr & 0x7F] = val; break;
    case AREA_IE_REG: break;
    }
}

void MMU::write_word(uint16_t addr, uint16_t val) {
    switch (get_area(addr)) {
    case AREA_BIOS: 
        bios[addr] = val; 
        bios[addr + 1] = val >> 8; 
        break;
    case AREA_ROM0:
        rom[addr] = val;
        rom[addr + 1] = val >> 8;
        break;
    case AREA_ROM1:
        rom[addr] = val;
        break;
    case AREA_VRAM: gpu.write_word_vram(addr - 0x8000, val); break;
    case AREA_ERAM: eram[addr & 0x1FFF] = val; break;
    case AREA_WRAM: wram[addr & 0x1FFF] = val; break;
    case AREA_OAM: break;
    case AREA_UNUSED: break;
    case AREA_IO: break;
    case AREA_HRAM:
        hram[addr & 0x7F] = val;
        hram[(addr + 1) & 0x7F] = val >> 8;
        break;
    case AREA_IE_REG: break;
    }
}

int MMU::get_area(uint16_t addr) {
    if (addr < 0x4000) {
        if (in_bios && addr == 0x100) {
            in_bios = false;
            LOG("Exit BIOS\n");
        }
        if (addr < 0x100 && in_bios) {
            return AREA_BIOS;
        }
        return AREA_ROM0;
    } else if (addr >= 0x4000 && addr < 0x8000) {
        return AREA_ROM1;
    } else if (addr >= 0x8000 && addr < 0xA000) {
        return AREA_VRAM;
    } else if (addr >= 0xA000 && addr < 0xC000) {
        return AREA_ERAM;
    } else if (addr >= 0xC000 && addr < 0xE000) {
        return AREA_WRAM;
    } else if (addr >= 0xE000 && addr < 0xFDFF) {
        return AREA_WRAM; // shadow
    } else {
        if (addr < 0xFE00) {
            return AREA_WRAM; // shadow
        } else if (addr >= 0xFE00 && addr < 0xFEA0) {
            return AREA_OAM;
        } else if (addr >= 0xFEA0 && addr < 0xFF00) {
            return AREA_UNUSED;
        } else if (addr >= 0xFF00 && addr < 0xFF80) {
            return AREA_IO;
        } else if (addr >= 0xFF80 && addr < 0xFFFF) {
            return AREA_HRAM;
        } else {
            return AREA_IE_REG;
        }
    }
}
