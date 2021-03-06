#pragma once

#include <cstdint>
#include <stdexcept>

namespace geemuboi::core {


class NotImplementedInstructionException : public std::logic_error {
public:
    NotImplementedInstructionException()
        : std::logic_error("An unimplemented instruction was called.") {};
};


class UndefinedInstructionException: public std::logic_error {
public:
    UndefinedInstructionException() 
        : std::logic_error("An undefined instruction was called.") {};
};


class ICpu {
public:
    static const int Z_FLAG = 0x80;
    static const int N_FLAG = 0x40;
    static const int H_FLAG = 0x20;
    static const int C_FLAG = 0x10;

    struct Registers {
        uint8_t a;
        uint8_t b;
        uint8_t c;
        uint8_t d;
        uint8_t e;
        uint8_t h;
        uint8_t l;
        uint8_t f;

        uint16_t pc;
        uint16_t sp;
    };

    virtual int execute() = 0;
    virtual unsigned get_cycles_executed() = 0;

    virtual ~ICpu() {}
};


}
