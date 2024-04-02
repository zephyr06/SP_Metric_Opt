
#include "sources/Safety_Performance_Metric/SP_Metric.h"

namespace SP_OPT_PA {
std::vector<double> GetChainsDDL(const DAG_Model& dag_tasks) {
    // std::vector<double> chains_ddl(dag_tasks.chains_.size(),
    //                                HyperPeriod(dag_tasks.tasks));
    return dag_tasks.chains_deadlines_;
}

double ObtainSP(const std::vector<FiniteDist>& dists,
                const std::vector<double>& deadline,
                const std::vector<double>& ddl_miss_thresholds,
                const std::vector<double>& weights) {
    int n = dists.size();
    double sp_overall = 0;
    for (int i = 0; i < n; i++) {
        double ddl_miss_chance = GetDDL_MissProbability(dists[i], deadline[i]);
        sp_overall +=
            SP_Func(ddl_miss_chance, ddl_miss_thresholds[i]) * weights[i];
    }
    return sp_overall;
}

double ObtainSP_TaskSet(const TaskSet& tasks,
                        const SP_Parameters& sp_parameters) {
    std::vector<FiniteDist> rtas = ProbabilisticRTA_TaskSet(tasks);
    std::vector<double> deadlines = GetParameter<double>(tasks, "deadline");
    return ObtainSP(rtas, deadlines, sp_parameters.thresholds_node,
                    sp_parameters.weights_node);
}

double ObtainSP_DAG(const DAG_Model& dag_tasks,
                    const SP_Parameters& sp_parameters) {
    double sp_overall = ObtainSP_TaskSet(dag_tasks.tasks, sp_parameters);

    std::vector<FiniteDist> reaction_time_dists =
        GetRTDA_Dist_AllChains<ObjReactionTime>(dag_tasks);
    std::vector<double> chains_ddl = GetChainsDDL(dag_tasks);

    sp_overall +=
        ObtainSP(reaction_time_dists, chains_ddl, sp_parameters.thresholds_path,
                 sp_parameters.weights_path);
    return sp_overall;
}
}  // namespace SP_OPT_PA