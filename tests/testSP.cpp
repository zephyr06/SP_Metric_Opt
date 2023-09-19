// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Safety_Performance_Metric/SP_Metric.h"
#include "sources/Utils/Parameters.h"

using ::testing::AtLeast;  // #1
using ::testing::Return;
using namespace std;
using namespace SP_OPT_PA;
using namespace GlobalVariables;

class TaskSetForTest_2tasks : public ::testing::Test {
   public:
    void SetUp() override {
        std::vector<Value_Proba> dist_vec1 = {
            Value_Proba(1, 0.6), Value_Proba(2, 0.3), Value_Proba(3, 0.1)};
        std::vector<Value_Proba> dist_vec2 = {Value_Proba(4, 0.7),
                                              Value_Proba(5, 0.3)};
        tasks.push_back(Task(0, dist_vec1, 5, 5, 0));
        tasks.push_back(Task(1, dist_vec2, 12, 12, 1));

        sp_parameters = SP_Parameters(tasks.size());
    }

    // data members
    TaskSet tasks;
    SP_Parameters sp_parameters;
};
TEST_F(TaskSetForTest_2tasks, SP_Calculation) {
    double sp_actual = ObtainSP(tasks, sp_parameters);
    double sp_expected = log(1 + 0.5) + log(1 + 0.5 - 0.0012);
    EXPECT_NEAR(sp_expected, sp_actual, 1e-6);
}
int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}