// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/Safety_Performance_Metric/SP_Metric.h"
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

        sp_parameters = SP_Parameters(tasks);
    }

    // data members
    TaskSet tasks;
    SP_Parameters sp_parameters;
};
TEST_F(TaskSetForTest_2tasks, SP_Calculation) {
    double sp_actual = ObtainSP_TaskSet(tasks, sp_parameters);
    double sp_expected = log(1 + 0.5) + log(1 + 0.5 - 0.0012) - 0.5 * 2;
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

        dag_tasks = DAG_Model(tasks, {{0, 1}}, {10});
        sp_parameters = SP_Parameters(dag_tasks);
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
    double penalty =
        0.18 + 0.21 + 0.09 + 0.07 + 0.03 - 0.5;  // for end-to-end latency
    double sp_expected_dag = log(1 + 0.5) + log(1 + 0.5 - 0.003) +
                             -0.01 * exp(10 * abs(penalty)) - 0.5 * (2 + 1);
    EXPECT_NEAR(sp_expected_dag, sp_actual_dag, 1e-8);
}

class TaskSetForTest_robotics_v1 : public ::testing::Test {
   public:
    void SetUp() override {
        std::string file_name = "test_robotics_v3";
        std::string path =
            GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
        dag_tasks = ReadDAG_Tasks(path, 5);
        sp_parameters = SP_Parameters(dag_tasks);
    }

    // data members
    DAG_Model dag_tasks;
    SP_Parameters sp_parameters;
};

TEST_F(TaskSetForTest_robotics_v1, SP_Calculation_dag) {
    string slam_path =
        GlobalVariables::PROJECT_PATH +
        "TaskData/AnalyzeSP_Metric/SLAM_response_time_200_210.txt";
    string rrt_path = GlobalVariables::PROJECT_PATH +
                      "TaskData/AnalyzeSP_Metric/RRT_response_time_200_210.txt";
    string mpc_path = GlobalVariables::PROJECT_PATH +
                      "TaskData/AnalyzeSP_Metric/MPC_response_time_200_210.txt";
    string tsp_path = GlobalVariables::PROJECT_PATH +
                      "TaskData/AnalyzeSP_Metric/TSP_response_time_200_210.txt";
    string chain0_path =
        GlobalVariables::PROJECT_PATH + "TaskData/AnalyzeSP_Metric/chain0.txt";

    int granularity = 10;
    std::vector<FiniteDist> node_rts_dists;
    // std::string folder_path="TaskData/AnalyzeSP_Metric/";
    node_rts_dists.push_back(FiniteDist(ReadTxtFile(tsp_path), granularity));
    node_rts_dists.push_back(FiniteDist(ReadTxtFile(mpc_path), granularity));
    node_rts_dists.push_back(FiniteDist(ReadTxtFile(rrt_path), granularity));
    node_rts_dists.push_back(FiniteDist(ReadTxtFile(slam_path), granularity));

    std::vector<FiniteDist> path_latency_dists;
    path_latency_dists.push_back(
        FiniteDist(ReadTxtFile(chain0_path), granularity));

    SP_Parameters sp_parameters = SP_Parameters(dag_tasks);
    double sp_metric_val = ObtainSP_DAG_From_Dists(
        dag_tasks, sp_parameters, node_rts_dists, path_latency_dists);
    cout << "SP-Metric: " << sp_metric_val << "\n";
    EXPECT_THAT(sp_metric_val, testing::Le(-4.5 + log(1.5) - 0.5 * (4 + 1)));
}

class TaskSetForTest_robotics_v8 : public ::testing::Test {
   public:
    void SetUp() override {
        std::string file_name = "test_robotics_v8";
        std::string path =
            GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
        dag_tasks = ReadDAG_Tasks(path, 5);
        sp_parameters = ReadSP_Parameters(path);
    }

    // data members
    DAG_Model dag_tasks;
    SP_Parameters sp_parameters;
    int N = dag_tasks.tasks.size();
};
// TEST_F(TaskSetForTest_robotics_v8, SP_Calculation) {
//     double sp_actual = ObtainSP_TaskSet(dag_tasks.tasks, sp_parameters);
//     double sp_expected = log(1 + 0.5) + log(1 + 0.5 - 0.0012) + 1.5 * 2;
//     // EXPECT_NEAR(sp_expected, sp_actual, 1e-6);
// }
TEST_F(TaskSetForTest_robotics_v1, read_sp) {}
// TEST_F(TaskSetForTest_robotics_v1, SP_Calculation_dag_v2) {
//     string slam_path =
//         GlobalVariables::PROJECT_PATH +
//         "TaskData/AnalyzeSP_Metric/SLAM_response_time_240_250.txt";
//     string rrt_path = GlobalVariables::PROJECT_PATH +
//                       "TaskData/AnalyzeSP_Metric/RRT_response_time_240_250.txt";
//     string mpc_path = GlobalVariables::PROJECT_PATH +
//                       "TaskData/AnalyzeSP_Metric/MPC_response_time_240_250.txt";
//     string tsp_path = GlobalVariables::PROJECT_PATH +
//                       "TaskData/AnalyzeSP_Metric/TSP_response_time_240_250.txt";

//     int granularity = 10;
//     std::vector<FiniteDist> dists;
//     // std::string folder_path="TaskData/AnalyzeSP_Metric/";
//     dists.push_back(FiniteDist(ReadTxtFile(tsp_path), granularity));
//     dists.push_back(FiniteDist(ReadTxtFile(mpc_path), granularity));
//     dists.push_back(FiniteDist(ReadTxtFile(rrt_path), granularity));
//     dists.push_back(FiniteDist(ReadTxtFile(slam_path), granularity));
//     std::vector<double> deadlines =
//         GetParameter<double>(dag_tasks.GetTaskSet(), "deadline");

//     SP_Parameters sp_parameters = SP_Parameters(dag_tasks);
// double sp_metric_val =
//     ObtainSP(dists, deadlines, sp_parameters.thresholds_node,
//              sp_parameters.weights_node);
// cout << "SP-Metric: " << sp_metric_val << "\n";
// EXPECT_THAT(sp_metric_val, testing::Le(-5.9));
// }

// TEST_F(TaskSetForTest_robotics_v1, SP_Calculation_dag_v3) {
//     string slam_path =
//         GlobalVariables::PROJECT_PATH +
//         "TaskData/AnalyzeSP_Metric/SLAM_response_time_300_310.txt";
//     string rrt_path = GlobalVariables::PROJECT_PATH +
//                       "TaskData/AnalyzeSP_Metric/RRT_response_time_300_310.txt";
//     string mpc_path = GlobalVariables::PROJECT_PATH +
//                       "TaskData/AnalyzeSP_Metric/MPC_response_time_300_310.txt";
//     string tsp_path = GlobalVariables::PROJECT_PATH +
//                       "TaskData/AnalyzeSP_Metric/TSP_response_time_300_310.txt";

//     int granularity = 10;
//     std::vector<FiniteDist> dists;
//     // std::string folder_path="TaskData/AnalyzeSP_Metric/";
//     dists.push_back(FiniteDist(ReadTxtFile(tsp_path), granularity));
//     dists.push_back(FiniteDist(ReadTxtFile(mpc_path), granularity));
//     dists.push_back(FiniteDist(ReadTxtFile(rrt_path), granularity));
//     dists.push_back(FiniteDist(ReadTxtFile(slam_path), granularity));
//     std::vector<double> deadlines =
//         GetParameter<double>(dag_tasks.GetTaskSet(), "deadline");

//     SP_Parameters sp_parameters = SP_Parameters(dag_tasks);
// double sp_metric_val =
//     ObtainSP(dists, deadlines, sp_parameters.thresholds_node,
//              sp_parameters.weights_node);
// cout << "SP-Metric: " << sp_metric_val << "\n";
// EXPECT_THAT(sp_metric_val, testing::Le(-5.9));
// }
int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}