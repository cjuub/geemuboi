#pragma once

#include "gpu.h"
#include "input.h"

#include <cstdint>
#include <string>

class IMmu {
public:
    virtual ~IMmu() {}
    virtual uint8_t read_byte(uint16_t addr) = 0;
    virtual uint16_t read_word(uint16_t addr) = 0;
    virtual void write_byte(uint16_t addr, uint8_t val) = 0;
    virtual void write_word(uint16_t addr, uint16_t val) = 0;
};
