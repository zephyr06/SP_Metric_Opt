#pragma once
#include <vector>

#include "sources/TaskModel/DAG_Model.h"

namespace SP_OPT_PA {

struct SP_Parameters {
    SP_Parameters() {}
    SP_Parameters(const TaskSet& tasks) {
        thresholds_node = std::vector<double>(tasks.size(), 0.5);
    }
    SP_Parameters(const DAG_Model& dag_tasks) {
        thresholds_node = std::vector<double>(dag_tasks.tasks.size(), 0.5);
        thresholds_path = std::vector<double>(dag_tasks.chains_.size(), 0.5);
    }

    // data
    std::vector<double> thresholds_node;
    std::vector<double> thresholds_path;
};
}  // namespace SP_OPT_PA