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
    double sp_actual = ObtainSP_TaskSet(tasks, sp_parameters);
    double sp_expected = log(1 + 0.5) + log(1 + 0.5 - 0.0012);
    EXPECT_NEAR(sp_expected, sp_actual, 1e-6);
}

class TaskSetForTest_2tasks1chain : public ::testing::Test {
   public:
    void SetUp() override {
        std::vector<Value_Proba> dist_vec1 = {
            Value_Proba(1, 0.6), Value_Proba(2, 0.3), Value_Proba(3, 0.1)};
        std::vector<Value_Proba> dist_vec2 = {Value_Proba(4, 0.7),
                                              Value_Proba(5, 0.3)};
        tasks.push_back(Task(0, dist_vec1, 5, 5, 0));
        tasks.push_back(Task(1, dist_vec2, 10, 10, 1));

        dag_tasks = DAG_Model(tasks, {{0, 1}});
        sp_parameters = SP_Parameters(tasks.size(), 1);
    }

    // data members
    TaskSet tasks;
    DAG_Model dag_tasks;
    SP_Parameters sp_parameters;
};
TEST_F(TaskSetForTest_2tasks1chain, reaction_time_full_utilization) {
    dag_tasks.tasks[0].setExecutionTime(3);
    dag_tasks.tasks[1].setExecutionTime(4);
    TaskSetInfoDerived tasks_info(dag_tasks.tasks);
    Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
    EXPECT_EQ(5, schedule[JobCEC(0, 1)].start);
    EXPECT_EQ(8, schedule[JobCEC(0, 1)].finish);
    EXPECT_EQ(3, schedule[JobCEC(1, 0)].start);
    EXPECT_EQ(10, schedule[JobCEC(1, 0)].finish);
    EXPECT_EQ(15, ObjReactionTime::Obj(dag_tasks, tasks_info, schedule,
                                       dag_tasks.chains_));
}
TEST_F(TaskSetForTest_2tasks1chain, GetRTDA_Dist_AllChains) {
    auto dists = GetRTDA_Dist_AllChains<ObjReactionTime>(dag_tasks);

    std::vector<Value_Proba> dist_vec1 = {
        Value_Proba(10, 0.42),      Value_Proba(12, 0.6 * 0.3),
        Value_Proba(13, 0.3 * 0.7), Value_Proba(14, 0.3 * 0.3),
        Value_Proba(15, 0.1 * 0.7), Value_Proba(INT32_MAX, 0.1 * 0.3)};
    FiniteDist reaction_time_dist_expected(dist_vec1);
    EXPECT_EQ(reaction_time_dist_expected, dists[0]);
}

TEST_F(TaskSetForTest_2tasks1chain, SP_Calculation_dag) {
    double sp_actual_dag = ObtainSP_DAG(dag_tasks, sp_parameters);
    double penalty = 0.18 + 0.21 + 0.09 + 0.07 + 0.03 - 0.5;
    double sp_expected_dag =
        log(1 + 0.5) + log(1 + 0.5 - 0.003) + -0.01 * exp(10 * abs(penalty));
    EXPECT_NEAR(sp_expected_dag, sp_actual_dag, 1e-8);
}
int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}