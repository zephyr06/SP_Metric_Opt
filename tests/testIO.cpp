// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/OptimizeSP_BF.h"
#include "sources/Safety_Performance_Metric/ParametersSP.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/TaskModel/RegularTasks.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/readwrite.h"
using ::testing::AtLeast;  // #1
using ::testing::Return;
using namespace std;
using namespace SP_OPT_PA;
using namespace GlobalVariables;
TEST(BasicExample, v1) {
    vector<double> data = ReadTxtFile(GlobalVariables::PROJECT_PATH +
                                      "ExperimentsData/FormatedData/test.txt");
    EXPECT_EQ(5, data.size());
    for (int i = 0; i < 5; i++) EXPECT_EQ(i + 1, data[i]);
}
TEST(read_write, task_sets) {
    std::string file_name = "test_robotics_v2";
    string path =
        GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
    TaskSet tasks = ReadTaskSet(path, 5);
    EXPECT_EQ(4, tasks.size());
    EXPECT_EQ(10, tasks[0].period);
    EXPECT_EQ(5, tasks[3].execution_time_dist.size());
    EXPECT_EQ(0.052, tasks[3].execution_time_dist[0].value);
}

TEST(read, DAG) {
    std::string file_name = "test_robotics_v2";
    string path =
        GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
    DAG_Model dag_tasks = ReadDAG_Tasks(path);
    EXPECT_EQ(2, dag_tasks.chains_.size());
    EXPECT_EQ(3, dag_tasks.chains_[0].size());
    EXPECT_EQ(0, dag_tasks.chains_[0][0]);
    EXPECT_EQ(2, dag_tasks.chains_[1].size());
    EXPECT_EQ(0, dag_tasks.tasks[0].processorId);
    EXPECT_EQ(1000, dag_tasks.chains_deadlines_[0]);
}
TEST(ReadSP_Parameters, V1) {
    std::string file_name = "test_robotics_v5";
    string path =
        GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
    SP_Parameters sp_par = ReadSP_Parameters(path);
    EXPECT_EQ(5, sp_par.weights_node[3]);
    EXPECT_EQ(0.1, sp_par.thresholds_node[2]);
}
TEST(write, DAG) {
    std::string file_name = "test_robotics_v2";
    string path =
        GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
    DAG_Model dag_tasks = ReadDAG_Tasks(path);
    string path_output =
        GlobalVariables::PROJECT_PATH + "TaskData/test_io_v1.yaml";
    WriteDAG_Tasks(path_output, dag_tasks);
    DAG_Model dag_tasks_read = ReadDAG_Tasks(path_output);
    EXPECT_EQ(2, dag_tasks_read.chains_.size());
    EXPECT_EQ(3, dag_tasks_read.chains_[0].size());
    EXPECT_EQ(0, dag_tasks.chains_[0][0]);

    TaskSet tasks = dag_tasks_read.tasks;
    EXPECT_EQ(4, tasks.size());
    EXPECT_EQ(10, tasks[0].period);
    EXPECT_EQ(GlobalVariables::Granularity,
              tasks[3].execution_time_dist.size());
    EXPECT_EQ(0.052, tasks[3].execution_time_dist[0].value);
}
// TEST(TranslatePriorityVec, PA_VEC_v1) {
//     PriorityVec pa = {0, 1, 2, 3};
//     std::vector<int> translated_pa_val = {40, 30, 20, 10};
//     EXPECT_EQ(translated_pa_val, TranslatePriorityVec(pa));
// }
// TEST(TranslatePriorityVec, PA_VEC_v2) {
//     PriorityVec pa = {3, 2, 0, 1};
//     std::vector<int> translated_pa_val = {20, 10, 30, 40};
//     EXPECT_EQ(translated_pa_val, TranslatePriorityVec(pa));
// }
int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}