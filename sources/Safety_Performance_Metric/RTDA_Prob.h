#pragma once
// TODO: generalize to working with execution time distribution
#include <unordered_map>

#include "sources/RTDA/ImplicitCommunication/ScheduleSimulation.h"
#include "sources/RTDA/ObjectiveFunction.h"
#include "sources/Safety_Performance_Metric/RTA.h"

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

template <typename ObjectiveFunctionType>
void travRTDACombinations(
    DAG_Model& dag_tasks, TaskSetInfoDerived& tasks_info,
    const std::vector<int>& chain, uint index, double probab,
    std::unordered_map<double, double>& response2prob_map) {
    if (index == chain.size()) {
        Schedule schedule = SimulateFixedPrioritySched(dag_tasks, tasks_info);
        double response_chain = ObjectiveFunctionType::Obj(
            dag_tasks, tasks_info, schedule, {chain});
        if (response2prob_map.count(response_chain))
            response2prob_map[response_chain] += probab;
        else
            response2prob_map[response_chain] = probab;
        return;
    }
    const FiniteDist& execution_time_dist =
        dag_tasks.GetTask(chain[index]).execution_time_dist;
    for (const Value_Proba& exec_pair : execution_time_dist.distribution) {
        dag_tasks.tasks[chain[index]].setExecutionTime(exec_pair.value);
        tasks_info.tasks[chain[index]].setExecutionTime(exec_pair.value);
        travRTDACombinations<ObjectiveFunctionType>(
            dag_tasks, tasks_info, chain, index + 1,
            probab * exec_pair.probability, response2prob_map);
    }
}

template <typename ObjectiveFunctionType>
FiniteDist GetRTDA_Dist_SingleChain(const DAG_Model& dag_tasks_input,
                                    const std::vector<int>& chain) {
    DAG_Model dag_tasks = dag_tasks_input;
    TaskSetInfoDerived tasks_info(dag_tasks.tasks);
    std::unordered_map<double, double> response2prob_map;
    UnsetExecutionTime(dag_tasks);
    double probab = 1;
    travRTDACombinations<ObjectiveFunctionType>(dag_tasks, tasks_info, chain, 0,
                                                probab, response2prob_map);
    return GetFiniteDist(response2prob_map);
}

template <typename ObjectiveFunctionType>
std::vector<FiniteDist> GetRTDA_Dist_AllChains(const DAG_Model& dag_tasks) {
    std::vector<FiniteDist> dists;
    dists.reserve(dag_tasks.chains_.size());
    for (const auto& chain : dag_tasks.chains_) {
        dists.push_back(
            GetRTDA_Dist_SingleChain<ObjectiveFunctionType>(dag_tasks, chain));
    }
    return dists;
}

}  // namespace SP_OPT_PA