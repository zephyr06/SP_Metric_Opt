#pragma once

#include <queue>
#include <unordered_set>

#include "sources/Optimization/OptimizeSP_Base.h"

namespace SP_OPT_PA {
struct PriorityPartialPath {
    PriorityPartialPath() {}
    PriorityPartialPath(const DAG_Model& dag_tasks,
                        const SP_Parameters& sp_parameters);

    void AssignAndUpdateSP(int task_id);

    void UpdateSP(int task_id);
    inline void AssertValidIndex(size_t i) const {
        if (i >= pa_vec_lower_pri.size())
            CoutError("Empty path in GetTaskWeight");
    }
    inline int GetTaskWeight(size_t i) const {
        AssertValidIndex(i);
        return sp_parameters.weights_node.at(pa_vec_lower_pri[i]);
    }
    inline int GetTaskPeriod(size_t i) const {
        AssertValidIndex(i);
        return dag_tasks.tasks[pa_vec_lower_pri[i]].period;
    }
    // inline int GetLastTaskMinUtil(size_t i) const {
    //     AssertValidIndex(i);
    //     return dag_tasks.tasks[pa_vec_lower_pri[i]].utilization();
    // }
    inline int GetTaskMinEt(size_t i) const {
        AssertValidIndex(i);
        return dag_tasks.tasks[pa_vec_lower_pri[i]]
            .execution_time_dist.min_time;
    }

    // const DAG_Model& dag_tasks;
    // const SP_Parameters& sp_parameters;
    DAG_Model dag_tasks;
    SP_Parameters sp_parameters;
    double sp;
    PriorityVec pa_vec_lower_pri;
    std::unordered_set<int> tasks_to_assign;
};

struct CompPriorityPath {
    // return true if rhs is better than lhs
    bool operator()(const PriorityPartialPath& lhs,
                    const PriorityPartialPath& rhs) const;
};

std::vector<int> FindTaskWithDifferentEt(const DAG_Model& dag_tasks,
                                         const DAG_Model& dag_tasks_updated);

PriorityVec RemoveOneTask(const PriorityVec& pa_vec, int task_id);

std::vector<PriorityVec> FindPriorityVec1D_Variations(const PriorityVec& pa_vec,
                                                      int task_id);

class OptimizePA_Incre : public OptimimizePA_Base {
   public:
    OptimizePA_Incre(const DAG_Model& dag_tasks,
                     const SP_Parameters& sp_parameters)
        : OptimimizePA_Base(dag_tasks, sp_parameters) {}

    // TODO: Current implementation doesn't consider end-to-end latency, need to
    // add later! One way to do it is by modifying the parameters of
    // sp_parameters
    PriorityVec OptimizeFromScratch(int K);

    PriorityVec OptimizeIncre(const DAG_Model& dag_tasks_update);
};

inline PriorityVec PerformOptimizePA_Incre(const DAG_Model& dag_tasks,
                                           const SP_Parameters& sp_parameters) {
    OptimizePA_Incre opt(dag_tasks, sp_parameters);
    return opt.OptimizeFromScratch(
        GlobalVariables::Layer_Node_During_Incremental_Optimization);
}
}  // namespace SP_OPT_PA