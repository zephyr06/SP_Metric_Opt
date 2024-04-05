
#include "sources/Optimization/OptimizeSP_Base.h"

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
            CoutError(
                "Input task set to UpdateTaskSetPriorities must be ordered by "
                "task id!");
        tasks_res[priority_assignment[i]].priority = i;
    }
    SortTasksByPriority(tasks_res);
    return tasks_res;
}

void PrintPriorityVec(const TaskSet& tasks,
                      const PriorityVec& priority_assignment) {
    std::cout << "Priority assignment (small values have higher priority):\n";
    for (uint i = 0; i < priority_assignment.size(); i++) {
        std::cout << tasks[priority_assignment[i]].name << ": " << i << "\n";
    }
}

YAML::Node PriorityAssignmentToYaml(const TaskSet& tasks,
                                    const PriorityVec& priority_assignment) {
    YAML::Node dictionary;
    for (uint i = 0; i < priority_assignment.size(); i++) {
        dictionary[tasks[priority_assignment[i]].name] =
            priority_assignment.size() - i;
    }
    return dictionary;
}

void WritePriorityAssignments(std::string path, const TaskSet& tasks,
                              const PriorityVec& pa_vec_input,
                              double time_taken) {
    auto dictionary = PriorityAssignmentToYaml(tasks, pa_vec_input);
    std::ofstream outputFile(path, std::ios::out);

    // Check if the file was opened successfully
    if (outputFile.is_open()) {
        outputFile
            << "# Priority assignments for each task (ordered by task id);\n"
            << "#For example, the first value is the priority for the first "
               "task with id = 0\n\n";
        // Iterate through the vector and write each element to the file
        // for (const int& element : pa_vec) {
        //     outputFile << element << "\n";
        // }
        outputFile << dictionary;

        outputFile << "\n# Run-time: "
                   << std::to_string(time_taken) + " seconds\n";
        // Close the file
        outputFile.close();

        std::cout << "Vector has been written to " << path << std::endl;
    } else {
        std::cerr << "Unable to open the file: " << path << std::endl;
    }
}

double EvaluateSPWithPriorityVec(const DAG_Model& dag_tasks,
                                 const SP_Parameters& sp_parameters,
                                 const PriorityVec& priority_assignment) {
    TaskSet tasks_eval =
        UpdateTaskSetPriorities(dag_tasks.tasks, priority_assignment);
    DAG_Model dag_tasks_eval = dag_tasks;
    dag_tasks_eval.tasks = tasks_eval;
    return ObtainSP_DAG(dag_tasks_eval, sp_parameters);
}

void PrintPA_IfDebugMode(const PriorityVec& priority_assignment,
                         double sp_eval) {
    if (GlobalVariables::debugMode == 1) {
        std::cout
            << "Try PA assignments (Task ID, high priority to low priority): ";
        for (int x : priority_assignment) std::cout << x << ", ";
        std::cout << sp_eval << "\n";
    }
}
}  // namespace SP_OPT_PA