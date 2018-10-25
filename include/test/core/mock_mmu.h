#include "gmock/gmock.h"

#include "core/immu.h"

namespace geemuboi::test::core {


class MockMmu : public geemuboi::core::IMmu {
public:
    MOCK_METHOD1(read_byte, uint8_t(uint16_t addr));
    MOCK_METHOD1(read_word, uint16_t(uint16_t addr));
    MOCK_METHOD2(write_byte, void(uint16_t addr, uint8_t val));
    MOCK_METHOD2(write_word, void(uint16_t addr, uint16_t val));
};


}
