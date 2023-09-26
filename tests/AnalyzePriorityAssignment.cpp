#include <yaml-cpp/yaml.h>

#include <iostream>

#include "sources/Optimization/OptimizeSP.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/argparse.hpp"

int main(int argc, char *argv[]) {
    using namespace std;
    using namespace SP_OPT_PA;

    argparse::ArgumentParser program("program name");
    program.add_argument("--file_path")
        .default_value(std::string("TaskData/test_robotics_v1.yaml"))
        .help(
            "the relative path of the yaml file that saves information about "
            "the tasks. Example: TaskData/test_robotics_v1.yaml");

    try {
        program.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }

    string file_path =
        GlobalVariables::PROJECT_PATH + program.get<std::string>("--file_path");
    DAG_Model dag_tasks = ReadDAG_Tasks(file_path);
    SP_Parameters sp_parameters = SP_Parameters(dag_tasks);
    PriorityVec pa_opt = OptimizePA_BruteForce(dag_tasks, sp_parameters);
    PrintPriorityVec(dag_tasks.tasks, pa_opt);
}