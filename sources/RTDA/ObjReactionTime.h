#pragma once
#include "sources/RTDA/JobScheduleInfo.h"
#include "sources/RTDA/RTDA_Factor.h"
#include "sources/Utils/profilier.h"
namespace SP_OPT_PA {

// task_index_in_chain: the index of a task in a cause-effect chain
// for example: consider a chain 0->1->2, task0's index is 0, task1's index is
// 1, task2's index is 2;

class ObjReactionTime {
   public:
    static const std::string type_trait;
    static double Obj(const DAG_Model &dag_tasks,
                      const TaskSetInfoDerived &tasks_info,
                      const Schedule &schedule,
                      const std::vector<std::vector<int>> &chains_to_analyze) {
        return ObjReactionTimeFromSChedule(dag_tasks, tasks_info,
                                           chains_to_analyze, schedule);
    }
};

}  // namespace SP_OPT_PA