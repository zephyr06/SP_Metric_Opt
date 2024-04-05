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
TEST_F(TaskSetForTest_2tasks, GetPriorityAssignments) {
    PriorityVec pa_vec1 = GetPriorityAssignments(tasks);
    EXPECT_EQ(2, pa_vec1.size());
    EXPECT_EQ(0, pa_vec1[0]);
    EXPECT_EQ(1, pa_vec1[1]);
}
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

TEST_F(TaskSetForTest_2tasks, Optimize_bf) {
    Task task_t = tasks[0];
    tasks[0] = tasks[1];
    tasks[0].id = 0;
    tasks[1] = task_t;
    tasks[1].id = 1;
    sp_parameters.thresholds_node[0] = 0;
    sp_parameters.thresholds_node[1] = 0;
    DAG_Model dag_tasks(tasks, {}, {});
    PriorityVec pa_opt = OptimizePA_BruteForce(dag_tasks, sp_parameters);
    EXPECT_EQ(5, tasks[pa_opt[0]].period);
    EXPECT_EQ(12, tasks[pa_opt[1]].period);
}
// TEST_F(TaskSetForTest_2tasks, Optimize_incre) {
//     Task task_t = tasks[0];
//     tasks[0] = tasks[1];
//     tasks[0].id = 0;
//     tasks[1] = task_t;
//     tasks[1].id = 1;
//     sp_parameters.thresholds_node = {0, 0};
//     DAG_Model dag_tasks(tasks, {}, {});
//     PriorityVec pa_opt = OptimizePA_Incremental(dag_tasks, sp_parameters);
//     PrintPriorityVec(tasks, pa_opt);
//     EXPECT_EQ(5, tasks[pa_opt[0]].period);
//     EXPECT_EQ(12, tasks[pa_opt[1]].period);
// }
// TEST_F(TaskSetForTest_2tasks, Optimize_incre_v2) {
//     Task task_t = tasks[0];
//     tasks[0] = tasks[1];
//     tasks[0].id = 0;
//     tasks[1] = task_t;
//     tasks[1].id = 1;
//     sp_parameters.thresholds_node = {0, 0};
//     DAG_Model dag_tasks(tasks, {}, {});
//     OptimizePA_Incre opt(dag_tasks, sp_parameters);
//     BeginTimer("First-run");
//     PriorityVec pa_opt = opt.Optimize(dag_tasks);
//     EndTimer("First-run");
//     BeginTimer("Second-runs");
//     pa_opt = opt.Optimize(dag_tasks);
//     // pa_opt = opt.Optimize(dag_tasks);
//     // pa_opt = opt.Optimize(dag_tasks);
//     EndTimer("Second-runs");
//     PrintPriorityVec(tasks, pa_opt);
//     EXPECT_EQ(5, tasks[pa_opt[0]].period);
//     EXPECT_EQ(12, tasks[pa_opt[1]].period);
//     PrintTimer();
// }

// TEST_F(TaskSetForTest_2tasks, FindTasksWithSameET) {
//     Task task_t = tasks[0];
//     tasks[0] = tasks[1];
//     tasks[0].id = 0;
//     tasks[1] = task_t;
//     tasks[1].id = 1;
//     sp_parameters.thresholds_node = {0, 0};
//     DAG_Model dag_tasks(tasks, {}, {});

//     OptimizePA_Incre opt_inc_class(dag_tasks, sp_parameters);
//     opt_inc_class.prev_exex_rec_ = GetExecutionTimeVector(dag_tasks);
//     std::vector<Value_Proba> dist_vec1 = {
//         Value_Proba(1, 0.5), Value_Proba(2, 0.4), Value_Proba(3, 0.1)};
//     tasks[0].execution_time_dist = dist_vec1;
//     DAG_Model dag_tasks2(tasks, {}, {});
//     vector<bool> diff_rec = opt_inc_class.FindTasksWithSameET(dag_tasks2);
//     EXPECT_FALSE(diff_rec[0]);
//     EXPECT_TRUE(diff_rec[1]);
// }

int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}