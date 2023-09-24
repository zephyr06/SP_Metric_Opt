
#include "sources/Safety_Performance_Metric/RTDA_Prob.h"

namespace SP_OPT_PA {
void UnsetExecutionTime(DAG_Model& dag_tasks) {
    for (uint i = 0; i < dag_tasks.tasks.size(); i++) {
        dag_tasks.tasks[i].setExecutionTime(-1);
    }
}

// ObjReactionTime or ObjDataAge
// TODO: add granularity
FiniteDist GetFiniteDist(
    const std::unordered_map<double, double>& response2prob_map) {
    std::vector<Value_Proba> distribution_pairs;
    distribution_pairs.reserve(response2prob_map.size());
    for (auto itr = response2prob_map.begin(); itr != response2prob_map.end();
         itr++) {
        distribution_pairs.push_back(Value_Proba(itr->first, itr->second));
    }
    return FiniteDist(distribution_pairs);
}

}  // namespace SP_OPT_PA