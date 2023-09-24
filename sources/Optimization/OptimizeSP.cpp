
#include "sources/Optimization/OptimizeSP.h"
namespace SP_OPT_PA {

TaskSet UpdateTaskSetPriorities(const TaskSet& tasks,
                                const PriorityVec& priority_assignment) {
    TaskSet tasks_res = tasks;
    for (uint i = 0; i < priority_assignment.size(); i++) {
        if (tasks_res[priority_assignment[i]].id != priority_assignment[i])
            CoutError("Inconsistent PA!");
        tasks_res[priority_assignment[i]].priority = i;
    }
    SortTasksByPriority(tasks_res);
    return tasks_res;
}

void OptimizePA_BF::IterateAllPAs(
    PriorityVec& priority_assignment,
    std::unordered_set<int>& tasks_assigned_priority, int start) {
    if (start == N) {
        TaskSet tasks_eval =
            UpdateTaskSetPriorities(tasks_, priority_assignment);
        double sp_eval = ObtainSP_TaskSet(tasks_eval, sp_parameters_);
        if (GlobalVariables::debugMode == 1) {
            std::cout << "Try PA assignments: ";
            for (int x : priority_assignment) std::cout << x << ", ";
            std::cout << sp_eval << "\n";
        }

        if (sp_eval > opt_sp_) {
            opt_sp_ = sp_eval;
            opt_pa_ = priority_assignment;
        }
    } else {
        for (int i = 0; i < N; i++) {
            if (tasks_assigned_priority.count(i) == 0) {
                priority_assignment.push_back(i);
                tasks_assigned_priority.insert(i);
                IterateAllPAs(priority_assignment, tasks_assigned_priority,
                              start + 1);
                tasks_assigned_priority.erase(i);
                priority_assignment.pop_back();
            }
        }
    }
}
PriorityVec OptimizePA_BF::Optimize() {
    double initial_sp = ObtainSP_TaskSet(tasks_, sp_parameters_);
    PriorityVec pa = {};
    std::unordered_set<int> tasks_assigned_priority;
    IterateAllPAs(pa, tasks_assigned_priority, 0);

    std::cout << "Initial SP is: " << initial_sp << "\n";
    std::cout << "Optimal SP is: " << opt_sp_ << "\n";
    return opt_pa_;
}

void PrintPriorityVec(const TaskSet& tasks,
                      const PriorityVec& priority_assignment) {
    std::cout << "Priority assignment:\n";
    for (uint i = 0; i < priority_assignment.size(); i++) {
        std::cout << tasks[priority_assignment[i]].name << ": " << i << "\n";
    }
}

}  // namespace SP_OPT_PA