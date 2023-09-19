#pragma once
// TODO: generalize to working with execution time distribution

#include "sources/TaskModel/RegularTasks.h"
namespace SP_OPT_PA {
FiniteDist GetRTA_OneTask(const Task& task_curr, const TaskSet& hp_tasks);
std::vector<FiniteDist> ProbabilisticRTA_TaskSet(const TaskSet& tasks);

}  // namespace SP_OPT_PA