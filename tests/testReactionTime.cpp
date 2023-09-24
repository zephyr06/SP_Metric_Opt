// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/RTDA/ImplicitCommunication/ScheduleSimulation.h"
#include "sources/RTDA/ObjectiveFunction.h"
#include "sources/Safety_Performance_Metric/ParametersSP.h"
#include "sources/Safety_Performance_Metric/RTDA_Prob.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/TaskModel/RegularTasks.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/readwrite.h"
using ::testing::AtLeast;  // #1
using ::testing::Return;
using namespace std;
using namespace SP_OPT_PA;
using namespace GlobalVariables;

class TaskSetForTest_deterministic : public ::testing::Test {
   public:
    void SetUp() override {
        std::string file_name = "test_n3_v18";
        std::string path =
            GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
        dag_tasks = ReadDAG_Tasks(path);
        tasks = dag_tasks.tasks;
        sp_parameters = SP_Parameters(tasks);
        AssignTaskSetPriorityById(dag_tasks.tasks);

        dag_tasks.tasks[0].setExecutionTime(1);
        dag_tasks.tasks[1].setExecutionTime(2);
        dag_tasks.tasks[2].setExecutionTime(3);
        tasks_info = TaskSetInfoDerived(dag_tasks.tasks);
    }

    // data members
    TaskSet tasks;
    DAG_Model dag_tasks;
    TaskSetInfoDerived tasks_info;
    SP_Parameters sp_parameters;
};

// TEST_F(TaskSetForTest_deterministic, UnsetExecutionTime) {
//     UnsetExecutionTime(dag_tasks);
//     EXPECT_EQ(-1, dag_tasks.GetTask(0).getExecutionTime());
//     EXPECT_EQ(-1, dag_tasks.GetTask(1).getExecutionTime());
//     EXPECT_EQ(-1, dag_tasks.GetTask(2).getExecutionTime());
// }

TEST_F(TaskSetForTest_deterministic, RT_dist_v1) {
    FiniteDist reaction_time_dist = GetRTDA_Dist_SingleChain<ObjReactionTime>(
        dag_tasks, dag_tasks.chains_[0]);
    EXPECT_EQ(1, reaction_time_dist.size());
    EXPECT_EQ(16, reaction_time_dist[0].value);
    EXPECT_NEAR(1, reaction_time_dist[0].probability, 1e-6);
}
TEST_F(TaskSetForTest_deterministic, RT_dist_v2) {
    dag_tasks.tasks[0].priority = 3;
    dag_tasks.tasks[1].priority = 2;
    dag_tasks.tasks[2].priority = 1;
    Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
    EXPECT_EQ(4, schedule.size());
    EXPECT_EQ(5, schedule[JobCEC(0, 0)].start);
    EXPECT_EQ(3, schedule[JobCEC(1, 0)].start);
    EXPECT_EQ(0, schedule[JobCEC(2, 0)].start);
    EXPECT_EQ(10, schedule[JobCEC(0, 1)].start);

    EXPECT_EQ(43 - 5, ObjReactionTime::Obj(dag_tasks, tasks_info, schedule,
                                           dag_tasks.chains_));
    FiniteDist reaction_time_dist = GetRTDA_Dist_SingleChain<ObjReactionTime>(
        dag_tasks, dag_tasks.chains_[0]);
    EXPECT_EQ(1, reaction_time_dist.size());
    EXPECT_EQ(43 - 5, reaction_time_dist[0].value);
    EXPECT_NEAR(1, reaction_time_dist[0].probability, 1e-6);
}

class TaskSetForTest_probabilistic : public ::testing::Test {
   public:
    void SetUp() override {
        std::string file_name = "test_n3_v18";
        std::string path =
            GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
        dag_tasks = ReadDAG_Tasks(path);
        tasks = dag_tasks.tasks;
        sp_parameters = SP_Parameters(tasks);
        AssignTaskSetPriorityById(dag_tasks.tasks);

        std::vector<Value_Proba> exec0 = {Value_Proba(1, 0.5),
                                          Value_Proba(2, 0.5)};
        std::vector<Value_Proba> exec1 = {Value_Proba(2, 0.5),
                                          Value_Proba(3, 0.5)};
        dag_tasks.tasks[0].execution_time_dist.UpdateDistribution(exec0);
        dag_tasks.tasks[1].execution_time_dist.UpdateDistribution(exec1);

        tasks_info = TaskSetInfoDerived(dag_tasks.tasks);
    }

    // data members
    TaskSet tasks;
    DAG_Model dag_tasks;
    TaskSetInfoDerived tasks_info;
    SP_Parameters sp_parameters;
};
TEST_F(TaskSetForTest_probabilistic, RT_dist_v1_1) {
    FiniteDist reaction_time_dist = GetRTDA_Dist_SingleChain<ObjReactionTime>(
        dag_tasks, dag_tasks.chains_[0]);

    std::vector<Value_Proba> dist_vec1 = {
        Value_Proba(16, 0.25), Value_Proba(17, 0.5), Value_Proba(18, 0.25)};
    FiniteDist reaction_time_dist_expected(dist_vec1);
    EXPECT_EQ(reaction_time_dist_expected, reaction_time_dist);
}
TEST_F(TaskSetForTest_probabilistic, GetRTDA_Dist_AllChains) {
    auto dists = GetRTDA_Dist_AllChains<ObjReactionTime>(dag_tasks);

    std::vector<Value_Proba> dist_vec1 = {
        Value_Proba(16, 0.25), Value_Proba(17, 0.5), Value_Proba(18, 0.25)};
    FiniteDist reaction_time_dist_expected(dist_vec1);
    EXPECT_EQ(reaction_time_dist_expected, dists[0]);
}
int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}