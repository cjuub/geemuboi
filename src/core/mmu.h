#ifndef MMU_H
#define MMU_H

#include "gpu.h"
#include "input.h"

#include <cstdint>
#include <string>

class MMU {
public:
    MMU(GPU& gpu, Input& input_in, const std::string& bios_file, const std::string& rom_file);
    
    uint8_t read_byte(uint16_t addr);
    uint16_t read_word(uint16_t addr);
    void write_byte(uint16_t addr, uint8_t val);
    void write_word(uint16_t addr, uint16_t val);
private:
    int get_area(uint16_t addr);

    enum Area {
        AREA_BIOS,
        AREA_ROM0,
        AREA_ROM1,
        AREA_VRAM,
        AREA_ERAM,
        AREA_WRAM,
        AREA_OAM,
        AREA_UNUSED,
        AREA_IO,
        AREA_HRAM,
        AREA_IE_REG
    };

    enum JoypadRegs {
        JOYPAD_REG = 0xFF00
    };

    enum GPURegs {
        GPU_REG_LCD_CONTROL = 0xFF40,
        GPU_REG_SCROLL_Y = 0xFF42,
        GPU_REG_SCROLL_X = 0xFF43,
        GPU_REG_CURR_SCANLINE = 0xFF44,
        GPU_REG_BG_PALETTE = 0xFF47
    };

    GPU& gpu;
    Input& input;

    bool in_bios;

    uint8_t bios[0x100];
    uint8_t rom[0x4000];
    uint8_t eram[0x2000];
    uint8_t wram[0x2000];
    uint8_t hram[0x7F];
};

#endif

