
#include "sources/Optimization/OptimizeSP_BF.h"
namespace SP_OPT_PA {

void OptimizePA_BF::IterateAllPAs(
    PriorityVec& priority_assignment,
    std::unordered_set<int>& tasks_assigned_priority, int start) {
    if (ifTimeout(start_time_))
        return;
    if (start == N) {
        // TaskSet tasks_eval =
        //     UpdateTaskSetPriorities(dag_tasks_.tasks, priority_assignment);
        // DAG_Model dag_tasks_eval = dag_tasks_;
        // dag_tasks_eval.tasks = tasks_eval;

        // double sp_eval = ObtainSP_DAG(dag_tasks_eval, sp_parameters_);
        double sp_eval = EvaluateSPWithPriorityVec(dag_tasks_, sp_parameters_,
                                                   priority_assignment);
        PrintPA_IfDebugMode(priority_assignment, sp_eval);
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
    double initial_sp = ObtainSP_DAG(dag_tasks_, sp_parameters_);
    PriorityVec pa = {};
    std::unordered_set<int> tasks_assigned_priority;
    opt_sp_ = initial_sp;
    opt_pa_ = GetPriorityAssignments(dag_tasks_.tasks);
    IterateAllPAs(pa, tasks_assigned_priority, 0);

    std::cout << "Initial SP is: " << initial_sp << "\n";
    std::cout << "Optimal SP is: " << opt_sp_ << "\n";
    return opt_pa_;
}

// std::vector<int> TranslatePriorityVec(const PriorityVec& pa_vec) {
//     std::vector<int> res(pa_vec.size());
//     // int min_pa=10;
//     int max_pa = 10 * pa_vec.size();
//     for (int i = 0; i < pa_vec.size(); i++) {
//         res[pa_vec[i]] = max_pa - i * 10;
//     }
//     return res;
// }

}  // namespace SP_OPT_PA