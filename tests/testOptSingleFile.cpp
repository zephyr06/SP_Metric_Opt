
#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/OptimizeSP.h"
#include "sources/Utils/Parameters.h"

using ::testing::AtLeast;  // #1
using ::testing::Return;
using namespace std;
using namespace SP_OPT_PA;
using namespace GlobalVariables;

class TaskSetForTest_robotics_v1 : public ::testing::Test {
   public:
    void SetUp() override {
        std::string file_name = "test_robotics_v1";
        std::string path =
            GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
        tasks = ReadTaskSet(path, 5);
        sp_parameters = SP_Parameters(tasks.size());
    }

    // data members
    TaskSet tasks;
    SP_Parameters sp_parameters;
};
TEST_F(TaskSetForTest_robotics_v1, optimize) {
    // sp_parameters.thresholds_node = {0, 0};
    PriorityVec pa_opt = OptimizePA_BruteForce(tasks, sp_parameters);
    PrintPriorityVec(tasks, pa_opt);
    EXPECT_EQ("TSP", tasks[pa_opt[3]].name);
}
int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}