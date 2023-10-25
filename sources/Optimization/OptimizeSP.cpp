
#include "sources/Optimization/OptimizeSP.h"
namespace SP_OPT_PA {
bool ifTimeout(TimerType start_time) {
    auto curr_time = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(curr_time - start_time)
            .count() >= GlobalVariables::TIME_LIMIT) {
        std::cout << "\nTime out when running OptimizeOrder. Maximum time is "
                  << GlobalVariables::TIME_LIMIT << " seconds.\n\n";
        return true;
    }
    return false;
}

PriorityVec GetPriorityAssignments(const TaskSet& tasks) {
    TaskSet tasks_copy = tasks;
    SortTasksByPriority(tasks_copy);
    PriorityVec pa_vec;
    pa_vec.reserve(tasks.size());
    for (uint i = 0; i < tasks_copy.size(); i++)
        pa_vec.push_back(tasks_copy[i].id);
    return pa_vec;
}

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
    if (ifTimeout(start_time_)) return;
    if (start == N) {
        TaskSet tasks_eval =
            UpdateTaskSetPriorities(dag_tasks_.tasks, priority_assignment);
        DAG_Model dag_tasks_eval = dag_tasks_;
        dag_tasks_eval.tasks = tasks_eval;

        double sp_eval = ObtainSP_DAG(dag_tasks_eval, sp_parameters_);
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
    double initial_sp = ObtainSP_TaskSet(dag_tasks_.tasks, sp_parameters_);
    PriorityVec pa = {};
    std::unordered_set<int> tasks_assigned_priority;
    opt_sp_ = initial_sp;
    opt_pa_ = GetPriorityAssignments(dag_tasks_.tasks);
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

std::vector<int> TranslatePriorityVec(const PriorityVec& pa_vec) {
    std::vector<int> res(pa_vec.size());
    for (int i = 0; i < pa_vec.size(); i++) {
        res[pa_vec[i]] = (i + 1) * 10;
    }
    return res;
}

void WritePriorityAssignments(std::string path, const PriorityVec& pa_vec_input,
                              double time_taken) {
    PriorityVec pa_vec = TranslatePriorityVec(pa_vec_input);
    std::ofstream outputFile(path, std::ios::out);

    // Check if the file was opened successfully
    if (outputFile.is_open()) {
        outputFile << "# Priority assignments, task ids are ordered from the "
                      "highest priority to the lowest priority:\n";
        // Iterate through the vector and write each element to the file
        for (const int& element : pa_vec) {
            outputFile << element << "\n";
        }
        outputFile << "\n# Run-time: "
                   << std::to_string(time_taken) + " seconds\n";
        // Close the file
        outputFile.close();

        std::cout << "Vector has been written to " << path << std::endl;
    } else {
        std::cerr << "Unable to open the file: " << path << std::endl;
    }
}
}  // namespace SP_OPT_PA