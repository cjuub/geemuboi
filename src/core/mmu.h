#ifndef MMU_H
#define MMU_H

#include "gpu.h"

#include <cstdint>
#include <string>

class MMU {
public:
    MMU(GPU& gpu, std::string bios_file, std::string rom_file);
    
    uint8_t read_byte(uint16_t addr);
    uint16_t read_word(uint16_t addr);
    void write_byte(uint16_t addr, uint8_t val);
    void write_word(uint16_t addr, uint16_t val);
private:
    int get_area(uint16_t addr);

    enum Area {
        AREA_BIOS = 0,
        AREA_ROM0 = 1,
        AREA_ROM1 = 2,
        AREA_VRAM = 3,
        AREA_ERAM = 4,
        AREA_WRAM = 5,
        AREA_OAM = 6,
        AREA_UNUSED = 7,
        AREA_IO = 8,
        AREA_HRAM = 9,
        AREA_IE_REG = 10
    };

    enum CpuReg {
        GPU_REG_LCD_CONTROL = 0xFF40,
        GPU_REG_SCROLL_Y = 0xFF42,
        GPU_REG_SCROLL_X = 0xFF43,
        GPU_REG_CURR_SCANLINE = 0xFF44,
        GPU_REG_BG_PALETTE = 0xFF47
    };

    GPU& gpu;

    bool in_bios = true;

    uint8_t bios[0x100] = {0};
    uint8_t rom[0x4000] = {0};
    uint8_t eram[0x2000] = {0};
    uint8_t wram[0x2000] = {0};
    uint8_t hram[0x7F] = {0};
};

#endif

