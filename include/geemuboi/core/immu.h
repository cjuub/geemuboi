#pragma once

#include <cstdint>
#include <stdexcept>


class NotImplementedMemoryRegionException: public std::logic_error {
public:
    NotImplementedMemoryRegionException() 
        : std::logic_error("An unimplemented memory region access was made.") {};
};


class IMmu {
public:
    virtual uint8_t read_byte(uint16_t addr) = 0;
    virtual uint16_t read_word(uint16_t addr) = 0;
    virtual void write_byte(uint16_t addr, uint8_t val) = 0;
    virtual void write_word(uint16_t addr, uint16_t val) = 0;

    virtual ~IMmu() {}
};
