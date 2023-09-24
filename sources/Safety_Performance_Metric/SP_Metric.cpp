
#include "sources/Safety_Performance_Metric/SP_Metric.h"

namespace SP_OPT_PA {

double ObtainSP(const TaskSet& tasks, const SP_Parameters& sp_parameters) {
    int n = tasks.size();
    std::vector<FiniteDist> rtas = ProbabilisticRTA_TaskSet(tasks);
    double sp_overall = 0;
    for (int i = 0; i < n; i++) {
        double ddl_miss_chance =
            GetDDL_MissProbability(rtas[i], tasks[i].deadline);
        sp_overall += SP_Func(ddl_miss_chance, sp_parameters.thresholds_node[i]);
    }
    return sp_overall;
}

}  // namespace SP_OPT_PA