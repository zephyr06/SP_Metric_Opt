#pragma once
#include "sources/ResponseTimeAnalysis/RTA_LL.h"
#include "sources/TaskModel/RegularTasks.h"

namespace SP_OPT_PA {

struct SP_Parameters {
    SP_Parameters() {}
};

double ObtainSP(const TaskSet& tasks, const SP_Parameters& sp_parameters);

}  // namespace SP_OPT_PA