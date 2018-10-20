#include "gtest/gtest.h"

#include "mock_mmu.h"
#include "core/cpu.h"

class CpuTest : public ::testing::Test {
protected:
    CpuTest() : mmu{}, cpu{mmu} {}

    MockMmu mmu;
    CPU cpu;
};

TEST_F(CpuTest, nop) {

}
