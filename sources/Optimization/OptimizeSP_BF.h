#pragma once

#include "sources/Optimization/OptimizeSP_Base.h"

namespace SP_OPT_PA {

class OptimizePA_BF : public OptimimizePA_Base {
   public:
    OptimizePA_BF(const DAG_Model& dag_tasks,
                  const SP_Parameters& sp_parameters)
        : OptimimizePA_Base(dag_tasks, sp_parameters) {}

    void IterateAllPAs(PriorityVec& priority_assignment,
                       std::unordered_set<int>& tasks_assigned_priority,
                       int start);

    PriorityVec Optimize();
};

inline PriorityVec OptimizePA_BruteForce(const DAG_Model& dag_tasks,
                                         const SP_Parameters& sp_parameters) {
    OptimizePA_BF opt(dag_tasks, sp_parameters);
    return opt.Optimize();
}

}  // namespace SP_OPT_PA