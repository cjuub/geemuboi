#pragma once

#include <cstdint>
#include <stdexcept>

namespace geemuboi::core {


class NotImplementedMemoryRegionException: public std::logic_error {
public:
    NotImplementedMemoryRegionException(
        const std::string& region_name_in,
        uint16_t addr_in,
        const std::string& access_in)
            : std::logic_error("An unimplemented memory region access was made."),
            region_name{region_name_in},
            address{addr_in},
            access{access_in} {};

    const std::string& get_region_name() const { return region_name; }
    uint16_t get_address() const { return address; }
    const std::string& get_access() const { return access; }
private:
    const std::string region_name;
    uint16_t address;
    const std::string access;
};


class IMmu {
public:
    virtual uint8_t read_byte(uint16_t addr) = 0;
    virtual uint16_t read_word(uint16_t addr) = 0;
    virtual void write_byte(uint16_t addr, uint8_t val) = 0;
    virtual void write_word(uint16_t addr, uint16_t val) = 0;

    virtual ~IMmu() {}
};


}
