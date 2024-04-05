// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Safety_Performance_Metric/Probability.h"
#include "sources/Safety_Performance_Metric/RTA.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/readwrite.h"
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
    }

    // data members
    TaskSet tasks;
};
TEST_F(TaskSetForTest_2tasks, RTA) {
    std::vector<Value_Proba> dist_vec0 = {
        Value_Proba(1, 0.6), Value_Proba(2, 0.3), Value_Proba(3, 0.1)};
    FiniteDist rta0_expected(dist_vec0);

    std::vector<Value_Proba> dist_vec1 = {
        Value_Proba(5, 0.42),   Value_Proba(7, 0.234),  Value_Proba(8, 0.213),
        Value_Proba(9, 0.105),  Value_Proba(10, 0.025), Value_Proba(12, 0.0018),
        Value_Proba(13, 0.0012)};
    FiniteDist rta1_expected(dist_vec1);
    vector<FiniteDist> rtas_actual = ProbabilisticRTA_TaskSet(tasks);
    EXPECT_EQ(2, rtas_actual.size());
    EXPECT_EQ(rta0_expected, rtas_actual[0]);

    EXPECT_TRUE(rta1_expected == rtas_actual[1]);
    rta1_expected.print();
    rtas_actual[1].print();
}

TEST_F(TaskSetForTest_2tasks, RTA_change_priority) {
    std::vector<Value_Proba> dist_vec0 = {
        Value_Proba(1, 0.6), Value_Proba(2, 0.3), Value_Proba(3, 0.1)};
    FiniteDist rta0_expected(dist_vec0);

    std::vector<Value_Proba> dist_vec1 = {
        Value_Proba(5, 0.42),   Value_Proba(7, 0.234),  Value_Proba(8, 0.213),
        Value_Proba(9, 0.105),  Value_Proba(10, 0.025), Value_Proba(12, 0.0018),
        Value_Proba(13, 0.0012)};
    FiniteDist rta1_expected(dist_vec1);

    // swap task 0 and task 1
    Task temp = tasks[0];
    tasks[0] = tasks[1];
    tasks[1] = temp;

    vector<FiniteDist> rtas_actual = ProbabilisticRTA_TaskSet(tasks);
    EXPECT_EQ(2, rtas_actual.size());
    EXPECT_EQ(rta0_expected, rtas_actual[1]);
    EXPECT_TRUE(rta1_expected == rtas_actual[0]);
}
TEST_F(TaskSetForTest_2tasks, GetDDL_MissProbability) {
    std::vector<FiniteDist> rtas = ProbabilisticRTA_TaskSet(tasks);

    std::vector<Value_Proba> dist_vec0 = {
        Value_Proba(1, 0.6), Value_Proba(2, 0.3), Value_Proba(3, 0.1)};
    FiniteDist rta0_expected(dist_vec0);

    std::vector<Value_Proba> dist_vec1 = {
        Value_Proba(5, 0.42),   Value_Proba(7, 0.234),  Value_Proba(8, 0.213),
        Value_Proba(9, 0.105),  Value_Proba(10, 0.025), Value_Proba(12, 0.0018),
        Value_Proba(13, 0.0012)};
    FiniteDist rta1_expected(dist_vec1);
    vector<FiniteDist> rtas_actual = ProbabilisticRTA_TaskSet(tasks);
    EXPECT_NEAR(0.0012, GetDDL_MissProbability(rtas[1], 12), 1e-6);
}

TEST_F(TaskSetForTest_2tasks, GetDDL_MissProbability_v2) {
    FiniteDist dists({5, 6, 7, 8, 5, 6, 7, 8}, 10);

    EXPECT_NEAR(0.0, GetDDL_MissProbability(dists, 10), 1e-6);
    EXPECT_NEAR(1.0, GetDDL_MissProbability(dists, 4), 1e-6);
    EXPECT_NEAR(0.75, GetDDL_MissProbability(dists, 5), 1e-6);
    EXPECT_NEAR(0.5, GetDDL_MissProbability(dists, 6.5), 1e-6);
}
TEST_F(TaskSetForTest_2tasks, GetDDL_MissProbability_v3) {
    FiniteDist dists({5}, 10);

    EXPECT_NEAR(0.0, GetDDL_MissProbability(dists, 10), 1e-6);
    EXPECT_NEAR(1.0, GetDDL_MissProbability(dists, 4), 1e-6);
    EXPECT_NEAR(0.0, GetDDL_MissProbability(dists, 5), 1e-6);
}
TEST_F(TaskSetForTest_2tasks, GetDDL_MissProbability_v4) {
    FiniteDist dists({5, 10000, 10000}, 10);

    EXPECT_NEAR(0.66666666, GetDDL_MissProbability(dists, 10), 1e-3);
    EXPECT_NEAR(1.0, GetDDL_MissProbability(dists, 4), 1e-3);
    EXPECT_NEAR(0.66666666, GetDDL_MissProbability(dists, 5), 1e-3);
}
class TaskSetForTest_4tasks : public ::testing::Test {
   public:
    void SetUp() override {
        string file_path =
            GlobalVariables::PROJECT_PATH + "TaskData/test_robotics_v3.yaml";
        dag_tasks = ReadDAG_Tasks(file_path);
    }

    // data members
    DAG_Model dag_tasks;
};
// TEST_F(TaskSetForTest_4tasks, GetDDL_MissProbability) {
//     dag_tasks.tasks[0].priority = 1;
//     dag_tasks.tasks[1].priority = 0;
//     dag_tasks.tasks[2].priority = 2;
//     dag_tasks.tasks[3].priority = 3;

//     std::vector<FiniteDist> rtas = ProbabilisticRTA_TaskSet(dag_tasks.tasks);
// }
int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}