#include "RegularTasks.h"

#include <yaml-cpp/yaml.h>

#include <filesystem>
namespace SP_OPT_PA {

// Recursive function to return gcd of a and b
long long gcd(long long int a, long long int b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Function to return LCM of two numbers
long long int lcm(long long int a, long long int b) {
    return (a / gcd(a, b)) * b;
}

long long int HyperPeriod(const TaskSet &tasks) {
    int N = tasks.size();
    if (N == 0) {
        std::cout << Color::red << "Empty task set in HyperPeriod()!\n";
        throw;
    } else {
        long long int hyper = tasks[0].period;
        for (int i = 1; i < N; i++) {
            hyper = lcm(hyper, tasks[i].period);
            if (hyper < 0 || hyper > LLONG_MAX) {
                std::cout << Color::red << "The hyper-period over flows!"
                          << Color::def << std::endl;
                throw;
            }
        }
        return hyper;
    }
}

ProcessorTaskSet ExtractProcessorTaskSet(const TaskSet &tasks) {
    int N = tasks.size();
    ProcessorTaskSet processorTasks;
    for (int i = 0; i < N; i++) {
        if (processorTasks.find(tasks.at(i).processorId) ==
            processorTasks.end()) {
            std::vector<int> ttt{tasks.at(i).id};
            processorTasks[tasks.at(i).processorId] = ttt;
        } else {
            processorTasks[tasks.at(i).processorId].push_back(tasks.at(i).id);
        }
    }
    return processorTasks;
}
TaskSet ReadTaskSet(std::string path, int granulairty) {
    if (!(std::filesystem::exists(path)))
        CoutError(path + " not exist!");
    YAML::Node config = YAML::LoadFile(path);
    YAML::Node tasksNode;
    if (config["tasks"]) {
        tasksNode = config["tasks"];
    } else {
        CoutError("Input file doesn't follow DAG format: " + path);
    }

    TaskSet tasks;
    tasks.reserve(tasksNode.size());
    int count = 0;
    for (size_t i = 0; i < tasksNode.size(); i++) {
        GaussianDist gauss(tasksNode[i]["execution_time_mu"].as<double>(),
                           tasksNode[i]["execution_time_sigma"].as<double>());
        FiniteDist finite_dist(
            gauss, tasksNode[i]["execution_time_min"].as<double>(),
            tasksNode[i]["execution_time_max"].as<double>(), granulairty);
        Task task(tasksNode[i]["id"].as<int>(), finite_dist,
                  tasksNode[i]["period"].as<double>(),
                  tasksNode[i]["deadline"].as<double>(), count++,
                  tasksNode[i]["name"].as<std::string>());
        if (tasksNode[i]["processorId"])
            task.processorId = tasksNode[i]["processorId"].as<int>();
        task.setExecGaussian(
            GaussianDist(tasksNode[i]["execution_time_mu"].as<double>(),
                         tasksNode[i]["execution_time_sigma"].as<double>()));
        tasks.push_back(task);
    }
    ScaleToInteger(tasks);
    return tasks;
}

void WriteTaskSet(std::string path, const TaskSet &tasks) {
    YAML::Node tasks_nodes;
    for (const Task &task : tasks) {
        YAML::Node task_node;
        task_node["id"] = std::to_string(task.id);
        task_node["execution_time_mu"] =
            std::to_string(task.getExecGaussian().mu);
        task_node["execution_time_sigma"] =
            std::to_string(task.getExecGaussian().sigma);
        task_node["execution_time_min"] =
            std::to_string(task.execution_time_dist.min_time);
        task_node["execution_time_max"] =
            std::to_string(task.execution_time_dist.max_time);
        task_node["period"] = std::to_string(task.period);
        task_node["deadline"] = std::to_string(task.deadline);
        task_node["name"] = task.name;
        tasks_nodes.push_back(task_node);
    }
    YAML::Node nodeRoot;
    nodeRoot["tasks"] = tasks_nodes;
    std::ofstream fout(path);
    fout << nodeRoot;
    fout.close();
}

void ScaleToInteger(TaskSet &tasks) {
    double min_period = tasks[0].period;
    for (uint i = 0; i < tasks.size(); i++) {
        min_period = min(tasks[i].period, min_period);
    }
    if (min_period < 1) {
        for (Task &task_curr : tasks) {
            task_curr.Scale(1 / min_period);
        }
    }
}

int GetHyperPeriod(const TaskSetInfoDerived &tasks_info,
                   const std::vector<int> &chain) {
    int hp = 1;
    for (int task_id : chain) {
        int period_curr = ceil(tasks_info.GetTask(task_id).period);
        hp = std::lcm(hp, period_curr);
    }
    return hp;
}

void AssignTaskSetPriorityById(TaskSet &tasks) {
    for (uint i = 0; i < tasks.size(); i++) tasks[i].priority = tasks[i].id;
}

}  // namespace SP_OPT_PA