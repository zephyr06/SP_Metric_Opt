#pragma once
#include "sources/Safety_Performance_Metric/Probability.h"
#include "vector"
namespace SP_OPT_PA {

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
}  // namespace SP_OPT_PA