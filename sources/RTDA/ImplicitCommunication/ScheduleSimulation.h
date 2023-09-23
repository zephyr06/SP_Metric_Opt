#pragma once
#include "sources/RTDA/ImplicitCommunication/RunQueue.h"
#include "sources/TaskModel/DAG_Model.h"
#include "sources/Utils/JobCEC.h"
#include "sources/Utils/MatirxConvenient.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/colormod.h"
namespace SP_OPT_PA {

void AddTasksToRunQueue(RunQueue &run_queue, const DAG_Model &dag_tasks,
                        int processor_id, LLint time_now);

Schedule SimulatedFTP_SingleCore(const DAG_Model &dag_tasks,
                                 const TaskSetInfoDerived &tasks_info,
                                 int processor_id);

Schedule SimulateFixedPrioritySched(const DAG_Model &dag_tasks,
                                    const TaskSetInfoDerived &tasks_info);

}  // namespace SP_OPT_PA
