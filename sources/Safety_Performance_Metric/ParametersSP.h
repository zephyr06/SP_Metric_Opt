#pragma once
#include <vector>

#include "sources/TaskModel/DAG_Model.h"

namespace SP_OPT_PA {

struct SP_Parameters {
    SP_Parameters() {}
    SP_Parameters(const TaskSet& tasks) {
        thresholds_node = std::vector<double>(tasks.size(), 0.5);
        weights_node = std::vector<double>(tasks.size(), 1);
    }
    SP_Parameters(const DAG_Model& dag_tasks) {
        thresholds_node = std::vector<double>(dag_tasks.tasks.size(), 0.5);
        weights_node = std::vector<double>(dag_tasks.tasks.size(), 1);
        thresholds_path = std::vector<double>(dag_tasks.chains_.size(), 0.5);
        weights_path = std::vector<double>(dag_tasks.chains_.size(), 1);
    }

    void reserve(size_t size) {
        thresholds_node.reserve(size);
        weights_node.reserve(size);
        thresholds_path.reserve(size);
        weights_path.reserve(size);
    }

    // data
    std::vector<double> thresholds_node;
    std::vector<double> weights_node;
    std::vector<double> thresholds_path;
    std::vector<double> weights_path;
};

SP_Parameters ReadSP_Parameters(const std::string& filename);
}  // namespace SP_OPT_PA