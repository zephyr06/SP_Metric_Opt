// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/RTDA/ImplicitCommunication/ScheduleSimulation.h"
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

class TaskSetForTest_scheduling_v1 : public ::testing::Test {
   public:
    void SetUp() override {
        std::string file_name = "test_n3_v18";
        std::string path =
            GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
        dag_tasks = ReadDAG_Tasks(path);
        tasks = dag_tasks.tasks;
        sp_parameters = SP_Parameters(tasks);
        AssignTaskSetPriorityById(dag_tasks.tasks);
        tasks_info = TaskSetInfoDerived(dag_tasks.tasks);

        dag_tasks.tasks[0].setExecutionTime(1);
        dag_tasks.tasks[1].setExecutionTime(2);
        dag_tasks.tasks[2].setExecutionTime(3);
    }

    // data members
    TaskSet tasks;
    DAG_Model dag_tasks;
    TaskSetInfoDerived tasks_info;
    SP_Parameters sp_parameters;
};
TEST_F(TaskSetForTest_scheduling_v1, simulate_schedule) {
    Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
    EXPECT_EQ(4, schedule.size());
    EXPECT_EQ(0, schedule[JobCEC(0, 0)].start);
    EXPECT_EQ(1, schedule[JobCEC(1, 0)].start);
    EXPECT_EQ(3, schedule[JobCEC(2, 0)].start);
    EXPECT_EQ(10, schedule[JobCEC(0, 1)].start);
}
TEST_F(TaskSetForTest_scheduling_v1, simulate_schedule_v2) {
    dag_tasks.tasks[0].priority = 3;
    dag_tasks.tasks[1].priority = 2;
    dag_tasks.tasks[2].priority = 1;
    Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
    EXPECT_EQ(4, schedule.size());
    EXPECT_EQ(5, schedule[JobCEC(0, 0)].start);
    EXPECT_EQ(3, schedule[JobCEC(1, 0)].start);
    EXPECT_EQ(0, schedule[JobCEC(2, 0)].start);
    EXPECT_EQ(10, schedule[JobCEC(0, 1)].start);
}
TEST_F(TaskSetForTest_scheduling_v1, simulate_schedule_v3) {
    dag_tasks.tasks[0].processorId = 1;
    Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
    EXPECT_EQ(4, schedule.size());
    EXPECT_EQ(0, schedule[JobCEC(0, 0)].start);
    EXPECT_EQ(0, schedule[JobCEC(1, 0)].start);
    EXPECT_EQ(2, schedule[JobCEC(2, 0)].start);
    EXPECT_EQ(10, schedule[JobCEC(0, 1)].start);
}
int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}