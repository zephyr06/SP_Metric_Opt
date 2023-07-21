#pragma once
#include "sources/TaskModel/DAG_Model.h"
#include "sources/ResponseTimeAnalysis/RTA_LL.h"

namespace SP_OPT{

struct SP_Parameters{
SP_Parameters(){}
};

double ObtainSP(const DAG_Model& dag_tasks, const SP_Parameters& sp_parameters);
    
} // namespace SP_OPT