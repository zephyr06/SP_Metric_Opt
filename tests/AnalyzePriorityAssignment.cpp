#include <yaml-cpp/yaml.h>

#include <iostream>

#include "sources/Optimization/OptimizeSP_BF.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/argparse.hpp"
#include "sources/Utils/profilier.h"

using namespace std;
using namespace SP_OPT_PA;

int main(int argc, char *argv[]) {
    TimerType start_time = CurrentTimeInProfiler;

    argparse::ArgumentParser program("program name");
    program.add_argument("--file_path")
        .default_value(std::string("TaskData/task_characteristics.yaml"))
        .help(
            "the relative path of the yaml file that saves information about "
            "the tasks. Example: TaskData/test_robotics_v1.yaml");
    program.add_argument("--output_file_path")
        .default_value(std::string("TaskData/pa_res_test_robotics_v1.yaml"))
        .help(
            "the relative path of the file that saves priority assignment "
            "results. Example: TaskData/pa_res_test_robotics_v1.yaml");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }

    string file_path = program.get<std::string>("--file_path");
    if (file_path[0] != '/')
        file_path = GlobalVariables::PROJECT_PATH + file_path;

    string output_file_path = GlobalVariables::PROJECT_PATH +
                              program.get<std::string>("--output_file_path");

    DAG_Model dag_tasks = ReadDAG_Tasks(file_path);
    // dag_tasks.tasks[0].priority = 3;
    // dag_tasks.tasks[1].priority = 2;
    // dag_tasks.tasks[2].priority = 0;
    // dag_tasks.tasks[3].priority = 1;
    dag_tasks.tasks[0].priority = 0;
    dag_tasks.tasks[1].priority = 1;
    dag_tasks.tasks[2].priority = 2;
    dag_tasks.tasks[3].priority = 3;
    SP_Parameters sp_parameters = SP_Parameters(dag_tasks);
    PriorityVec pa_opt = OptimizePA_BruteForce(dag_tasks, sp_parameters);
    PrintPriorityVec(dag_tasks.tasks, pa_opt);

    TimerType finish_time = CurrentTimeInProfiler;
    double time_taken = GetTimeTaken(start_time, finish_time);
    WritePriorityAssignments(output_file_path, dag_tasks.tasks, pa_opt,
                             time_taken);
}