#include <yaml-cpp/yaml.h>

#include <iostream>

#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/argparse.hpp"

int main(int argc, char *argv[]) {
    using namespace std;
    using namespace SP_OPT_PA;

    argparse::ArgumentParser program("program name");
    program.add_argument("--task_id")
        .default_value(0)
        .help("the task which needs to update its parameters")
        .scan<'i', int>();
    program.add_argument("--parameter_name")
        .default_value(std::string("execution_time_mu"))
        .help(
            "example: execution_time_mu, execution_time_sigma, "
            "execution_time_min, execution_time_max, period, deadline");
    program.add_argument("--value")
        .default_value(0.4)
        .help("the value of the parameter (only accepts numbers)")
        .scan<'f', double>();
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

    int task_id = program.get<int>("--task_id");
    string parameter_name = program.get<std::string>("--parameter_name");
    double value = program.get<double>("--value");
    string file_path =
        GlobalVariables::PROJECT_PATH + program.get<std::string>("--file_path");

    DAG_Model dag_tasks = ReadDAG_Tasks(file_path);
    if (parameter_name == "execution_time_mu") {
        GaussianDist dist = dag_tasks.tasks[task_id].getExecGaussian();
        dist.mu = value;
        dag_tasks.tasks[task_id].setExecGaussian(dist);
    } else if (parameter_name == "execution_time_sigma") {
        GaussianDist dist = dag_tasks.tasks[task_id].getExecGaussian();
        dist.sigma = value;
        dag_tasks.tasks[task_id].setExecGaussian(dist);
    } else if (parameter_name == "execution_time_min") {
        dag_tasks.tasks[task_id].execution_time_dist.min_time = value;
    } else if (parameter_name == "execution_time_max") {
        dag_tasks.tasks[task_id].execution_time_dist.max_time = value;
    } else if (parameter_name == "period")
        dag_tasks.tasks[task_id].period = value;
    else if (parameter_name == "deadline")
        dag_tasks.tasks[task_id].period = value;
    WriteDAG_Tasks(file_path, dag_tasks);
}