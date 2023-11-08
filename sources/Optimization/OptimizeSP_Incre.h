#pragma once
#include "sources/Optimization/OptimizeSP_BF.h"

std::string Priority2Str(const SP_OPT_PA::PriorityVec& pa_vec) {
    std::string str = "";
    for (uint i = 0; i < pa_vec.size(); i++)
        str += std::to_string(pa_vec[i]) + ",";
    return str;
}
template <>
struct std::hash<SP_OPT_PA::PriorityVec> {
    std::size_t operator()(const SP_OPT_PA::PriorityVec& key) const {
        std::string str = Priority2Str(key);
        return std::hash<std::string>{}(str);
    }
};

namespace SP_OPT_PA {

inline DAG_Model GetEmptyDAG(int N) {
    TaskSet tasks(N, Task(0, FiniteDist(GaussianDist(), 0, 0, 1), 1, 1, 0));
    DAG_Model dags;
    dags.tasks = tasks;
    return dags;
}

struct Task_SP_Info {
    Task_SP_Info() : task_id(-1), hp_task_ids({-1}) {}
    Task_SP_Info(int task_id, const std::vector<int>& hp_task_ids,
                 const FiniteDist& rta_dist)
        : task_id(task_id), hp_task_ids(hp_task_ids), rta_dist(rta_dist) {}

    int task_id;
    std::vector<int> hp_task_ids;
    FiniteDist rta_dist;
};

struct Chain_SP_Info {
    Chain_SP_Info() : chain_id(-1) {}
    Chain_SP_Info(int chain_id, const FiniteDist& rta_dist)
        : chain_id(chain_id), rta_dist(rta_dist) {}

    int chain_id;
    FiniteDist rta_dist;
};
struct SP_Per_PA_Info {
    SP_Per_PA_Info() {}
    // TODO: make chain_num an input
    SP_Per_PA_Info(int tasks_num, int chains_num = 10) {
        sp_rec_per_task = std::vector<Task_SP_Info>(tasks_num);
        sp_rec_per_chain = std::vector<Chain_SP_Info>(chains_num);
    }

    void update(int task_id, const std::vector<int>& hp_task_ids,
                const FiniteDist& rta_dist) {
        sp_rec_per_task[task_id] = Task_SP_Info(task_id, hp_task_ids, rta_dist);
    }
    Task_SP_Info read(int task_id) const { return sp_rec_per_task[task_id]; }

    void update_chain(int chain_id, const FiniteDist& rta_dist) {
        sp_rec_per_chain[chain_id] = Chain_SP_Info(chain_id, rta_dist);
    }
    Chain_SP_Info read_chain(int chain_id) const {
        return sp_rec_per_chain[chain_id];
    }

   private:
    std::vector<Task_SP_Info> sp_rec_per_task;
    std::vector<Chain_SP_Info> sp_rec_per_chain;
};

std::vector<FiniteDist> GetExecutionTimeVector(const DAG_Model& dag_tasks) {
    std::vector<FiniteDist> prev_exex_rec;
    prev_exex_rec.reserve(dag_tasks.tasks.size());
    for (uint i = 0; i < dag_tasks.tasks.size(); i++) {
        prev_exex_rec.push_back(dag_tasks.GetTask(i).execution_time_dist);
    }
    return prev_exex_rec;
}

// task id must begin with 0 and be continuous integers
class OptimizePA_Incre {
   public:
    // OptimizePA_Incre() {}
    OptimizePA_Incre(const DAG_Model& dag_tasks,
                     const SP_Parameters& sp_parameters)
        : dag_tasks_(dag_tasks),
          sp_parameters_(sp_parameters),
          N(dag_tasks.tasks.size()),
          opt_sp_(INT_MIN),
          start_time_((std::chrono::high_resolution_clock::now())) {}

    void IterateAllPAs(PriorityVec& priority_assignment,
                       std::unordered_set<int>& tasks_assigned_priority,
                       int start);

    std::vector<FiniteDist> ProbabilisticRTA_TaskSet(
        const PriorityVec& priority_assignment, const TaskSet& tasks_input);

    template <typename ObjectiveFunctionType>
    std::vector<FiniteDist> GetRTDA_Dist_AllChains(
        const PriorityVec& priority_assignment, const DAG_Model& dag_tasks);

    // ordered by task id, i-th element is false if the task with i-th id
    // changes ET
    std::vector<bool> FindTasksWithSameET(const DAG_Model& dag_tasks) const;
    bool SameHpTasks(const std::vector<int>& hp_task_ids_ite,
                     const std::vector<int>& hp_task_ids_prev) const;

    bool SameChains(const std::vector<int>& chain) const;

    double EvalAndRecordSP(const PriorityVec& priority_assignment,
                           const DAG_Model& dag_tasks_eval);

    PriorityVec Optimize(const DAG_Model& dag_tasks);

    // data members
    DAG_Model dag_tasks_;
    SP_Parameters sp_parameters_;
    std::vector<FiniteDist> prev_exex_rec_;
    int N;
    double opt_sp_;
    PriorityVec opt_pa_;
    TimerType start_time_;
    std::vector<bool>
        tasks_ET_if_same_;  // true means not changed; false means changed;
    std::unordered_map<PriorityVec, SP_Per_PA_Info> prev_sp_rec_;
};

inline PriorityVec OptimizePA_Incremental(const DAG_Model& dag_tasks,
                                          const SP_Parameters& sp_parameters) {
    OptimizePA_Incre opt(dag_tasks, sp_parameters);
    return opt.Optimize(dag_tasks);
}
}  // namespace SP_OPT_PA
