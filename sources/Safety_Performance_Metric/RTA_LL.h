#pragma once

#include "sources/Safety_Performance_Metric/RTA_BASE.h"
namespace SP_OPT_PA {

class RTA_LL : public RTA_BASE {
   public:
    RTA_LL(const TaskSet &tasksI) { tasks = tasksI; }

    double RTA_Common_Warm(double beginTime, int index) override {
        TaskSet tasksHp;
        for (int i = 0; i < index; i++) {
            tasksHp.push_back(tasks.at(i));
        }
        return ResponseTimeAnalysisWarm(beginTime, tasks.at(index), tasksHp);
    }

    // the rest are helper functions
    static std::string type() { return "LL"; }

    double ResponseTimeAnalysisWarm_util_nece(
        double beginTime, const Task &taskCurr,
        const TaskSet &tasksHighPriority) {
        const std::vector<int> periodHigh =
            GetParameter<int>(tasksHighPriority, "period");
        const std::vector<double> executionTimeHigh =
            GetParameter<double>(tasksHighPriority, "executionTime");
        int N = periodHigh.size();

        if (beginTime < 0) {
            if (GlobalVariables::debugMode == 1) {
                CoutWarning(
                    "During optimization, some variables drop below 0\n");
            }
            beginTime = 0;
        }
        if (isnan(taskCurr.getExecutionTime()) || isnan(beginTime)) {
            std::cout << Color::red << "Nan executionTime detected"
                      << Color::def << std::endl;
            throw "Nan";
        }
        if (taskCurr.getExecutionTime() < 0) {
            return INT32_MAX;
        }
        for (int i = 0; i < int(executionTimeHigh.size()); i++) {
            if (executionTimeHigh[i] < 0) {
                if (GlobalVariables::debugMode) {
                    CoutWarning(
                        "During optimization, some variables drop below 0\n");
                }
                return INT32_MAX;
            }
        }
        double utilAll = Utilization(tasksHighPriority);
        double utilTol = 1e-3;
        if (utilAll >= 1.0 - utilTol) {
            // cout << "The given task set is unschedulable\n";
            return INT32_MAX;
        }
        for (uint i = 0; i < tasksHighPriority.size(); i++) {
            if (tasksHighPriority[i].period < 0)
                return INT32_MAX;
        }

        bool stop_flag = false;

        double responseTimeBefore = beginTime;
        int loopCount = 0;
        while (not stop_flag) {
            double responseTime = taskCurr.getExecutionTime();
            for (int i = 0; i < N; i++)
                responseTime +=
                    ceil(responseTimeBefore / double(periodHigh[i])) *
                    executionTimeHigh[i];
            if (responseTime == responseTimeBefore) {
                stop_flag = true;
                if (responseTime > taskCurr.period) {
                    return INT32_MAX;
                }
                return responseTime;
            } else {
                responseTimeBefore = responseTime;
            }
            loopCount++;
            if (loopCount > 1500) {
                CoutWarning("LoopCount error in RTA_LL");
                return INT32_MAX;
            }
        }
        std::cout << "RTA analysis stops unexpectedly!\n";
        throw;
    }

    double ResponseTimeAnalysisWarm(const double beginTime,
                                    const Task &taskCurr,
                                    const TaskSet &tasksHighPriority) {
        if (Utilization(tasksHighPriority) + taskCurr.utilization() >
            1.0 + 1e-6) {
            return INT32_MAX;
        }
        return ResponseTimeAnalysisWarm_util_nece(beginTime, taskCurr,
                                                  tasksHighPriority);
    }

    double ResponseTimeAnalysis(const Task &taskCurr,
                                const TaskSet &tasksHighPriority) {
        const std::vector<double> executionTimeHigh =
            GetParameter<double>(tasksHighPriority, "executionTime");
        double executionTimeAll = taskCurr.getExecutionTime();
        for (auto &task : tasksHighPriority)
            executionTimeAll += task.getExecutionTime();
        return ResponseTimeAnalysisWarm(executionTimeAll, taskCurr,
                                        tasksHighPriority);
    }
};

inline bool CheckTaskSetSchedulabilityLL(const TaskSet &tasks_input) {
    TaskSet tasks = tasks_input;
    std::sort(tasks.begin(), tasks.end(),
              [](const Task &task1, const Task &task2) {
                  return task1.priority < task2.priority;
              });
    RTA_LL r(tasks);
    return r.CheckSchedulability();
}
}  // namespace SP_OPT_PA