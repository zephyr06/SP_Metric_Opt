// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/OptimizeSP.h"
#include "sources/Utils/Parameters.h"

using ::testing::AtLeast;  // #1
using ::testing::Return;
using namespace std;
using namespace SP_OPT_PA;
using namespace GlobalVariablesDAGOpt;

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
TEST_F(TaskSetForTest_2tasks, optimize) {
    PriorityVec pa_vec = {0, 1};
    UpdateTaskSetPriorities(tasks, pa_vec);
    EXPECT_EQ(0, tasks[0].id);
    EXPECT_EQ(0, tasks[0].priority);
    EXPECT_EQ(1, tasks[1].id);
    EXPECT_EQ(1, tasks[1].priority);

    pa_vec = {1, 0};
    tasks = UpdateTaskSetPriorities(tasks, pa_vec);
    EXPECT_EQ(1, tasks[0].id);
    EXPECT_EQ(0, tasks[0].priority);
    EXPECT_EQ(0, tasks[1].id);
    EXPECT_EQ(1, tasks[1].priority);
}

TEST_F(TaskSetForTest_2tasks, Optimize) {
    sp_parameters.thresholds = {0, 0};
    PriorityVec pa_opt = OptimizePA_BruteForce(tasks, sp_parameters);
}
int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}