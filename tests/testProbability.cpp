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
TEST(FiniteDist, V1) {
    GaussianDist gau_dis(10, 1);
    FiniteDist finite_dis(gau_dis, 5, 15, 11);
    EXPECT_EQ(11, finite_dis.size());
    EXPECT_EQ(5, finite_dis[0].value);
    EXPECT_NEAR(2.866e-7, finite_dis[0].probability, 1e-8);

    EXPECT_EQ(6, finite_dis[1].value);
    EXPECT_NEAR(3.167e-5 - 2.866e-7, finite_dis[1].probability, 1e-6);
}
int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}