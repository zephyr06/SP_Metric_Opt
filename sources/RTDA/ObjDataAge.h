#pragma once
#include "sources/RTDA/ObjReactionTime.h"
namespace SP_OPT_PA {

class ObjDataAge {
   public:
    static const std::string type_trait;

    static double Obj(const DAG_Model &dag_tasks,
                      const TaskSetInfoDerived &tasks_info,
                      const Schedule &schedule,
                      const std::vector<std::vector<int>> &chains_to_analyze) {
        return ObjDataAgeFromSChedule(dag_tasks, tasks_info, chains_to_analyze,
                                      schedule);
    }
};
}  // namespace SP_OPT_PA