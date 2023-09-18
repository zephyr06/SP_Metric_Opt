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
    EXPECT_EQ(5, finite_dis.min_execution_time);
    EXPECT_EQ(15, finite_dis.max_execution_time);
    EXPECT_NEAR(2.866e-7, finite_dis[0].probability, 1e-8);

    EXPECT_EQ(6, finite_dis[1].value);
    EXPECT_NEAR(3.167e-5 - 2.866e-7, finite_dis[1].probability, 1e-6);
}

TEST(FiniteDist, Coalesce_v1) {
    std::vector<Value_Proba> dist_vec1 = {Value_Proba(3, 0.1),
                                          Value_Proba(7, 0.9)};
    std::vector<Value_Proba> dist_vec2 = {Value_Proba(0, 0.9),
                                          Value_Proba(4, 0.1)};
    FiniteDist dist1(dist_vec1);
    FiniteDist dist2(dist_vec2);
    dist1.Coalesce(dist2);
    EXPECT_EQ(4, dist1.size());
    EXPECT_EQ(0, dist1[0].value);
    EXPECT_EQ(0.9, dist1[0].probability);

    EXPECT_EQ(3, dist1[1].value);
    EXPECT_EQ(0.1, dist1[1].probability);

    EXPECT_EQ(4, dist1[2].value);
    EXPECT_EQ(0.1, dist1[2].probability);

    EXPECT_EQ(7, dist1[3].value);
    EXPECT_EQ(0.9, dist1[3].probability);
}

TEST(FiniteDist, Coalesce_v2) {
    std::vector<Value_Proba> dist_vec1 = {Value_Proba(5, 0.18),
                                          Value_Proba(8, 0.02)};
    std::vector<Value_Proba> dist_vec2 = {Value_Proba(5, 0.72),
                                          Value_Proba(6, 0.08)};
    FiniteDist dist1(dist_vec1);
    FiniteDist dist2(dist_vec2);
    dist1.Coalesce(dist2);
    EXPECT_EQ(3, dist1.size());
    EXPECT_EQ(5, dist1[0].value);
    EXPECT_NEAR(0.9, dist1[0].probability, 1e-3);

    EXPECT_EQ(6, dist1[1].value);
    EXPECT_EQ(0.08, dist1[1].probability);

    EXPECT_EQ(8, dist1[2].value);
    EXPECT_EQ(0.02, dist1[2].probability);
}

TEST(FiniteDist, Convolve) {
    std::vector<Value_Proba> dist_vec1 = {Value_Proba(3, 0.1),
                                          Value_Proba(7, 0.9)};
    std::vector<Value_Proba> dist_vec2 = {Value_Proba(0, 0.9),
                                          Value_Proba(4, 0.1)};
    FiniteDist dist1(dist_vec1);
    FiniteDist dist2(dist_vec2);
    dist1.Convolve(dist2);
    EXPECT_EQ(3, dist1.size());
    EXPECT_EQ(3, dist1[0].value);
    EXPECT_NEAR(0.09, dist1[0].probability, 1e-6);

    EXPECT_EQ(7, dist1[1].value);
    EXPECT_NEAR(0.82, dist1[1].probability, 1e-6);

    EXPECT_EQ(11, dist1[2].value);
    EXPECT_NEAR(0.09, dist1[2].probability, 1e-6);
}

int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}