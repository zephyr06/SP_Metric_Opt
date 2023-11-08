
#include "sources/Optimization/OptimizeSP_Incre.h"

namespace SP_OPT_PA {

// true if a task's ET is the same as prev
std::vector<bool> OptimizePA_Incre::FindTasksWithSameET(
    const DAG_Model& dag_tasks) const {
    std::vector<bool> diff_rec(dag_tasks_.GetTaskSet().size(), false);
    if (prev_exex_rec_.size() == 0)
        return diff_rec;
    else {
        std::vector<FiniteDist> exec_vec_curr =
            GetExecutionTimeVector(dag_tasks);
        for (int i = 0; i < N; i++) {
            bool diff = bool(exec_vec_curr[i] == prev_exex_rec_[i]);
            diff_rec[i] = diff;
        }
    }
    return diff_rec;
}

PriorityVec OptimizePA_Incre::Optimize(const DAG_Model& dag_tasks) {
    tasks_ET_if_same_ = FindTasksWithSameET(dag_tasks);
    // maybe not very necessary to create an initial PA
    double initial_sp = ObtainSP_TaskSet(dag_tasks_.tasks, sp_parameters_);
    PriorityVec pa = {};
    std::unordered_set<int> tasks_assigned_priority;
    opt_sp_ = initial_sp;
    opt_pa_ = GetPriorityAssignments(dag_tasks_.tasks);
    IterateAllPAs(pa, tasks_assigned_priority, 0);

    std::cout << "Initial SP is: " << initial_sp << "\n";
    std::cout << "Optimal SP is: " << opt_sp_ << "\n";
    prev_exex_rec_ = GetExecutionTimeVector(dag_tasks);
    return opt_pa_;
}

bool OptimizePA_Incre::SameHpTasks(
    const std::vector<int>& hp_task_ids_ite,
    const std::vector<int>& hp_task_ids_prev) const {
    if (hp_task_ids_ite.size() != hp_task_ids_ite.size())
        return false;
    for (uint i = 0; i < hp_task_ids_ite.size(); i++) {
        if (hp_task_ids_ite[i] != hp_task_ids_prev[i])
            return false;
    }
    for (int id : hp_task_ids_ite) {
        if (tasks_ET_if_same_[id] == false)
            return false;
    }
    return true;
}

std::vector<FiniteDist> OptimizePA_Incre::ProbabilisticRTA_TaskSet(
    const PriorityVec& priority_assignment, const TaskSet& tasks_input) {
    if (!prev_sp_rec_.count(priority_assignment))
        prev_sp_rec_[priority_assignment] = SP_Per_PA_Info(tasks_input.size());

    TaskSet tasks = tasks_input;
    std::sort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
        return t1.priority < t2.priority;
    });
    int n = tasks.size();
    std::vector<FiniteDist> rtas;
    rtas.reserve(n);
    TaskSet hp_tasks;
    std::vector<int> hp_task_ids;
    hp_tasks.reserve(n - 1);
    hp_task_ids.reserve(n - 1);
    for (int i = 0; i < n; i++) {
        const SP_Per_PA_Info& sp_prev_pa_eval =
            prev_sp_rec_[priority_assignment];
        Task_SP_Info prev_task_sp_info = sp_prev_pa_eval.read(tasks[i].id);

        if (SameHpTasks(hp_task_ids, prev_task_sp_info.hp_task_ids))
            rtas.push_back(prev_task_sp_info.rta_dist);
        else {
            FiniteDist rta_curr = GetRTA_OneTask(tasks[i], hp_tasks);
            prev_sp_rec_[priority_assignment].update(tasks[i].id, hp_task_ids,
                                                     rta_curr);
            rtas.push_back(rta_curr);
        }

        hp_tasks.push_back(tasks[i]);
        hp_task_ids.push_back(tasks[i].id);
    }
    return rtas;
}

// this function does the same as ObtainSP_DAG() except it updates the
// prev_sp_rec_
double OptimizePA_Incre::EvalAndRecordSP(const PriorityVec& priority_assignment,
                                         const DAG_Model& dag_tasks_eval) {
    // task set SP
    std::vector<FiniteDist> rtas_task_set = ProbabilisticRTA_TaskSet(
        priority_assignment, dag_tasks_eval.GetTaskSet());
    std::vector<double> deadlines_task_set =
        GetParameter<double>(dag_tasks_eval.GetTaskSet(), "deadline");

    // chain SP
    // BIG TODO: update this function's logic
    std::vector<FiniteDist> reaction_time_dists =
        GetRTDA_Dist_AllChains<ObjReactionTime>(dag_tasks_eval);
    std::vector<double> chains_ddl = GetChainsDDL(dag_tasks_eval);
    return ObtainSP(rtas_task_set, deadlines_task_set,
                    sp_parameters_.thresholds_node) +
           ObtainSP(reaction_time_dists, chains_ddl,
                    sp_parameters_.thresholds_path);
}

void OptimizePA_Incre::IterateAllPAs(
    PriorityVec& priority_assignment,
    std::unordered_set<int>& tasks_assigned_priority, int start) {
    if (ifTimeout(start_time_))
        return;
    if (start == N) {
        TaskSet tasks_eval =
            UpdateTaskSetPriorities(dag_tasks_.tasks, priority_assignment);
        DAG_Model dag_tasks_eval = dag_tasks_;
        dag_tasks_eval.tasks = tasks_eval;

        double sp_eval = ObtainSP_DAG(dag_tasks_eval, sp_parameters_);
        sp_eval = EvalAndRecordSP(priority_assignment, dag_tasks_eval);
        // double sp_eval2 = EvalAndRecordSP(priority_assignment,
        // dag_tasks_eval);

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
}  // namespace SP_OPT_PA