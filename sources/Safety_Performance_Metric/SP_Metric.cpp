
#include "sources/Safety_Performance_Metric/SP_Metric.h"

namespace SP_OPT_PA {
std::vector<double> GetChainsDDL(const DAG_Model& dag_tasks) {
    // std::vector<double> chains_ddl(dag_tasks.chains_.size(),
    //                                HyperPeriod(dag_tasks.tasks));
    return dag_tasks.chains_deadlines_;
}
double ObtainSP(const FiniteDist& dist, double deadline,
                double ddl_miss_threshold, double weight) {
    double ddl_miss_chance = GetDDL_MissProbability(dist, deadline);
    return SP_Func(ddl_miss_chance, ddl_miss_threshold) * weight;
}

// double ObtainSP(const std::vector<FiniteDist>& dists,
//                 const std::vector<double>& deadline,
//                 const std::unordered_map<int, double>& ddl_miss_thresholds,
//                 const std::unordered_map<int, double>& weights) {
//     int n = dists.size();
//     double sp_overall = 0;
//     for (int i = 0; i < n; i++) {
//         double ddl_miss_chance = GetDDL_MissProbability(dists[i],
//         deadline[i]); sp_overall +=
//             SP_Func(ddl_miss_chance, ddl_miss_thresholds[i]) * weights[i];
//     }
//     return sp_overall;
// }

double ObtainSP_TaskSet(const TaskSet& tasks,
                        const SP_Parameters& sp_parameters) {
    std::vector<FiniteDist> rtas = ProbabilisticRTA_TaskSet(tasks);
    // std::vector<double> deadlines = GetParameter<double>(tasks, "deadline");
    // return ObtainSP(rtas, deadlines, sp_parameters.thresholds_node,
    //                 sp_parameters.weights_node);
    double sp_overall = 0;
    for (int i = 0; i < tasks.size(); i++) {
        int task_id = tasks[i].id;
        double ddl_miss_chance =
            GetDDL_MissProbability(rtas[i], tasks[i].deadline);
        sp_overall += SP_Func(ddl_miss_chance,
                              sp_parameters.thresholds_node.at(task_id)) *
                      sp_parameters.weights_node.at(task_id);
    }
    return sp_overall;
}

double ObtainSP_DAG(const DAG_Model& dag_tasks,
                    const SP_Parameters& sp_parameters) {
    double sp_overall = ObtainSP_TaskSet(dag_tasks.tasks, sp_parameters);

    std::vector<FiniteDist> reaction_time_dists =
        GetRTDA_Dist_AllChains<ObjReactionTime>(dag_tasks);
    std::vector<double> chains_ddl = GetChainsDDL(dag_tasks);

    for (int i = 0; i < reaction_time_dists.size(); i++) {
        int chain_id = i;
        double ddl_miss_chance =
            GetDDL_MissProbability(reaction_time_dists[i], chains_ddl[i]);
        sp_overall += SP_Func(ddl_miss_chance,
                              sp_parameters.thresholds_path.at(chain_id)) *
                      sp_parameters.weights_path.at(chain_id);
    }
    return sp_overall;
}

double ObtainSP_DAG_From_Dists(
    const DAG_Model& dag_tasks, const SP_Parameters& sp_parameters,
    const std::vector<FiniteDist>& node_rts_dists,
    const std::vector<FiniteDist>& path_latency_dists) {
    double sp_overall = 0;
    for (uint i = 0; i < dag_tasks.tasks.size(); i++) {
        int task_id = dag_tasks.tasks[i].id;
        sp_overall += ObtainSP(node_rts_dists[i], dag_tasks.tasks[i].deadline,
                               sp_parameters.thresholds_node.at(task_id),
                               sp_parameters.weights_node.at(task_id));
    }
    for (uint i = 0; i < dag_tasks.chains_.size(); i++) {
        sp_overall +=
            ObtainSP(path_latency_dists[i], dag_tasks.chains_deadlines_[i],
                     sp_parameters.thresholds_path.at(i),
                     sp_parameters.weights_path.at(i));
    }
    return sp_overall;
}
}  // namespace SP_OPT_PA