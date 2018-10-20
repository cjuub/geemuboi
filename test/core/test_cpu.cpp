#include <gtest/gtest.h>

#include "core/cpu.h"

class CpuTest : public ::testing::Test {
protected:
    CpuTest() {}
};

TEST_F(CpuTest, Nop) {
    EXPECT_EQ(1, 1);
}
