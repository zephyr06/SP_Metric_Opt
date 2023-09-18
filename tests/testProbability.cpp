// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Safety_Performance_Metric/Probability.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/readwrite.h"
using ::testing::AtLeast;  // #1
using ::testing::Return;
using namespace std;
using namespace SP_OPT_PA;
using namespace GlobalVariablesDAGOpt;
TEST(CDF, v1) {
    GaussianDist gau_dis(3, 1);
    EXPECT_EQ(0.5, gau_dis.CDF(3));
}

int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}