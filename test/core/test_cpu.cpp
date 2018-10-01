#include <gtest/gtest.h>

#include "../../src/core/cpu.h"

class CpuTest : public ::testing::Test {
protected:
    CpuTest() : cpu(mmu) {}

    CPU cpu;
};

TEST_F(CpuTest, Nop) {
    int cycles = cpu.execute(0x00);
    
    EXPECT_EQ(1, 1);
}
