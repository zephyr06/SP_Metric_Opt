// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.

using ::testing::AtLeast;  // #1
using ::testing::Return;
using namespace std;
TEST(BasicExample, v1) { EXPECT_EQ(2, 2); }

int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}