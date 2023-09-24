#pragma once

#include "sources/Safety_Performance_Metric/SP_Metric.h"

namespace SP_OPT_PA {

// task id sequence;
typedef std::vector<int> PriorityVec;
void PrintPriorityVec(const TaskSet& tasks,
                      const PriorityVec& priority_assignment);
TaskSet UpdateTaskSetPriorities(const TaskSet& tasks,
                                const PriorityVec& priority_assignment);

class OptimizePA_BF {
   public:
    OptimizePA_BF() {}
    OptimizePA_BF(const DAG_Model& dag_tasks, const SP_Parameters& sp_parameters)
        : dag_tasks_(dag_tasks),
          sp_parameters_(sp_parameters),
          N(dag_tasks.tasks.size()),
          opt_sp_(INT_MIN) {}

    void IterateAllPAs(PriorityVec& priority_assignment,
                       std::unordered_set<int>& tasks_assigned_priority,
                       int start);

    PriorityVec Optimize();

    // data members
    DAG_Model dag_tasks_;
    SP_Parameters sp_parameters_;
    int N;
    double opt_sp_;
    PriorityVec opt_pa_;
};

inline PriorityVec OptimizePA_BruteForce(const DAG_Model& dag_tasks,
                                         const SP_Parameters& sp_parameters) {
    OptimizePA_BF opt(dag_tasks, sp_parameters);
    return opt.Optimize();
}

}  // namespace SP_OPT_PA