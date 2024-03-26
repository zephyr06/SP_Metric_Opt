
#include <yaml-cpp/yaml.h>

#include <fstream>
#include <iostream>

#include "gmock/gmock.h"  // Brings in gMock.
#include "string"
using namespace std;

void VerifyFileExist(std::string filename) {
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
    }
}
void UpdatePriorityAssignments(std::string tasks_yaml,
                               std::string priority_yaml) {
    VerifyFileExist(tasks_yaml);
    VerifyFileExist(priority_yaml);

    YAML::Node tasks = YAML::LoadFile(tasks_yaml);
    YAML::Node priorities = YAML::LoadFile(priority_yaml);

    for (YAML::const_iterator it = priorities.begin(); it != priorities.end();
         ++it) {
        string app_name = it->first.as<std::string>();
        int priority = it->second.as<int>();
        // cout << app_name << ", " << priority << "\n";
        for (YAML::Node::iterator it_task = tasks["Tasks"].begin();
             it_task != tasks["Tasks"].end(); ++it_task) {
            if (app_name == it_task->operator[]("name").as<std::string>()) {
                it_task->operator[]("priority") = priority;
                break;
            }
        }
    }

    std::ofstream fout(tasks_yaml);
    YAML::Emitter emitter;

    // Emit the YAML content to the emitter
    emitter << tasks;
    // Write the YAML content to the file
    fout << emitter.c_str();

    // Close the file
    fout.close();
}

// Write the updated tasks YAML back to the first file
// std::ofstream fout(target_yaml, std::ios::out);
// fout << tasks;

// std::cout << "Updated first.yaml with priorities from second.yaml" <<
// std::endl;
// }
TEST(UpdatePriorityAssignments, v1) {
    string tasks_yaml =
        "/home/zephyr/Programming/ROS2-SP-APPs/src/real_time_manager/configs/"
        "local_cpu_and_priority.yaml";
    string priority_yaml =
        "/home/zephyr/Programming/SP_Metric_Opt/TaskData/"
        "pa_res_test_robotics_v1.yaml";
    UpdatePriorityAssignments(tasks_yaml, priority_yaml);
}

int main(int argc, char** argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}