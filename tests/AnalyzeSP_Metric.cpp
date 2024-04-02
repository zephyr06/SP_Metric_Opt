#include <yaml-cpp/yaml.h>

#include <iostream>

#include "sources/Optimization/OptimizeSP_BF.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/argparse.hpp"
#include "sources/Utils/profilier.h"
#include "sources/Utils/readwrite.h"
using namespace std;
using namespace SP_OPT_PA;

int main(int argc, char *argv[]) {
    argparse::ArgumentParser program("program name");
    program.add_argument("--slam_path")
        .default_value(std::string("TaskData/AnalyzeSP_Metric/slam.txt"))
        .help(
            "the relative path of the yaml file that saves information about "
            "the tasks. Example: TaskData/AnalyzeSP_Metric/slam.txt");
    program.add_argument("--rrt_path")
        .default_value(std::string("TaskData/AnalyzeSP_Metric/rrt.txt"))
        .help(
            "the relative path of the yaml file that saves information about "
            "the tasks. Example: TaskData/AnalyzeSP_Metric/rrt.txt");
    program.add_argument("--mpc_path")
        .default_value(std::string("TaskData/AnalyzeSP_Metric/mpc.txt"))
        .help(
            "the relative path of the yaml file that saves information about "
            "the tasks. Example: TaskData/AnalyzeSP_Metric/mpc.txt");
    program.add_argument("--tsp_path")
        .default_value(std::string("TaskData/AnalyzeSP_Metric/tsp.txt"))
        .help(
            "the relative path of the yaml file that saves information about "
            "the tasks. Example: TaskData/AnalyzeSP_Metric/tsp.txt");
    program.add_argument("--chain0_path")
        .default_value(std::string("TaskData/AnalyzeSP_Metric/chain0.txt"))
        .help(
            "the relative path of the yaml file that saves information about "
            "the tasks. Example: TaskData/AnalyzeSP_Metric/chain0.txt");

    // program.add_argument("--file_path")
    //     .default_value(std::string("TaskData/test_robotics_v1.yaml"))
    //     .help(
    //         "the relative path of the yaml file that saves information about
    //         " "the tasks. Example: TaskData/test_robotics_v1.yaml");
    // program.add_argument("--output_file_path")
    //     .default_value(std::string("TaskData/pa_res_test_robotics_v1.txt"))
    //     .help(
    //         "the relative path of the file that saves priority assignment "
    //         "results. Example: TaskData/pa_res_test_robotics_v1.txt");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }

    string slam_path = program.get<std::string>("--slam_path");
    if (slam_path[0] != '/')
        slam_path = GlobalVariables::PROJECT_PATH +
                    program.get<std::string>("--slam_path");

    string rrt_path = program.get<std::string>("--rrt_path");
    if (rrt_path[0] != '/')
        rrt_path = GlobalVariables::PROJECT_PATH + rrt_path;

    string mpc_path = program.get<std::string>("--mpc_path");
    if (mpc_path[0] != '/')
        mpc_path = GlobalVariables::PROJECT_PATH + mpc_path;

    string tsp_path = program.get<std::string>("--tsp_path");
    if (tsp_path[0] != '/')
        tsp_path = GlobalVariables::PROJECT_PATH + tsp_path;

    string chain0_path = program.get<std::string>("--chain0_path");
    if (chain0_path[0] != '/')
        chain0_path = GlobalVariables::PROJECT_PATH + chain0_path;

    string file_path_ref = "TaskData/test_robotics_v3.yaml";

    file_path_ref = GlobalVariables::PROJECT_PATH + file_path_ref;

    int granularity = 10;
    DAG_Model dag_tasks =
        ReadDAG_Tasks(file_path_ref);  // only read the tasks without worrying
                                       // about the execution time distribution

    SP_Parameters sp_parameters = ReadSP_Parameters(file_path_ref);
    std::vector<FiniteDist> node_rts_dists;
    // std::string folder_path="TaskData/AnalyzeSP_Metric/";
    node_rts_dists.push_back(FiniteDist(ReadTxtFile(tsp_path), granularity));
    node_rts_dists.push_back(FiniteDist(ReadTxtFile(mpc_path), granularity));
    node_rts_dists.push_back(FiniteDist(ReadTxtFile(rrt_path), granularity));
    node_rts_dists.push_back(FiniteDist(ReadTxtFile(slam_path), granularity));

    std::vector<FiniteDist> reaction_time_dists = {
        FiniteDist(ReadTxtFile(chain0_path), granularity)};

    double sp_value_overall = ObtainSP_DAG_From_Dists(
        dag_tasks, sp_parameters, node_rts_dists, reaction_time_dists);

    std::cout << "SP-Metric: " << sp_value_overall << "\n";
}