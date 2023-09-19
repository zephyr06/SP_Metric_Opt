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

TaskSet ReadTaskSet(std::string path, int granulairty) {
    if (!(std::filesystem::exists(path)))
        CoutError(path + " not exist!");
    YAML::Node config = YAML::LoadFile(path);
    YAML::Node tasksNode;
    if (config["tasks"]) {
        tasksNode = config["tasks"];
    } else {
        CoutError("Input file doesn't follow Nasri format: " + path);
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

        tasks.push_back(task);
    }
    return tasks;
}

}  // namespace SP_OPT_PA