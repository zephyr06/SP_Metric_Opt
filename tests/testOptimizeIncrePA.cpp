// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Optimization/OptimizeSP_BF.h"
#include "sources/Optimization/OptimizeSP_Incre.h"
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

        sp_parameters = SP_Parameters(tasks);
    }

    // data members
    TaskSet tasks;
    SP_Parameters sp_parameters;
};

class TaskSetForTest_robotics_v6 : public ::testing::Test {
   public:
    void SetUp() override {
        std::string file_name = "test_robotics_v6";
        std::string path =
            GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
        dag_tasks = ReadDAG_Tasks(path, 5);
        sp_parameters = SP_Parameters(dag_tasks);
    }

    // data members
    DAG_Model dag_tasks;
    SP_Parameters sp_parameters;
    int N = dag_tasks.tasks.size();
};

TEST_F(TaskSetForTest_robotics_v6, Compare_PriorityPartialPath) {
    std::priority_queue<PriorityPartialPath, std::vector<PriorityPartialPath>,
                        CompPriorityPath>
        pq;
    PriorityPartialPath path(dag_tasks, sp_parameters);
    for (int task_id : path.tasks_to_assign) {
        PriorityPartialPath new_path = path;
        new_path.AssignAndUpdateSP(task_id);
        pq.push(new_path);
    }
    EXPECT_EQ("TSP", dag_tasks.tasks[pq.top().pa_vec_lower_pri[0]].name);
    pq.pop();

    EXPECT_EQ("SLAM", dag_tasks.tasks[pq.top().pa_vec_lower_pri[0]].name);
    pq.pop();

    EXPECT_EQ("RRT", dag_tasks.tasks[pq.top().pa_vec_lower_pri[0]].name);
    pq.pop();

    EXPECT_EQ("MPC", dag_tasks.tasks[pq.top().pa_vec_lower_pri[0]].name);
    pq.pop();
}

TEST_F(TaskSetForTest_robotics_v6, GetPriorityAssignments) {
    OptimizePA_Incre opt(dag_tasks, sp_parameters);
    PriorityVec pa_vec1 = opt.OptimizeFromScratch(2);
    EXPECT_EQ(4, pa_vec1.size());
    EXPECT_EQ("MPC", dag_tasks.tasks[pa_vec1[0]].name);
    EXPECT_EQ("RRT", dag_tasks.tasks[pa_vec1[1]].name);
    EXPECT_EQ("TSP", dag_tasks.tasks[pa_vec1[2]].name);
    EXPECT_EQ("SLAM", dag_tasks.tasks[pa_vec1[3]].name);
}

class TaskSetForTest_robotics_v7 : public ::testing::Test {
   public:
    void SetUp() override {
        std::string file_name = "test_robotics_v7";
        std::string path =
            GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
        dag_tasks = ReadDAG_Tasks(path, 5);
        sp_parameters = SP_Parameters(dag_tasks);
    }

    // data members
    DAG_Model dag_tasks;
    SP_Parameters sp_parameters;
    int N = dag_tasks.tasks.size();
};
TEST_F(TaskSetForTest_robotics_v7, GetPriorityAssignments) {
    OptimizePA_Incre opt(dag_tasks, sp_parameters);
    PriorityVec pa_vec1 = opt.OptimizeFromScratch(2);
    EXPECT_EQ(4, pa_vec1.size());
    EXPECT_EQ("MPC", dag_tasks.tasks[pa_vec1[0]].name);
    EXPECT_EQ("RRT", dag_tasks.tasks[pa_vec1[1]].name);
    EXPECT_EQ("SLAM", dag_tasks.tasks[pa_vec1[2]].name);
    EXPECT_EQ("TSP", dag_tasks.tasks[pa_vec1[3]].name);
}
int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}