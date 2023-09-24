#pragma once

#include "sources/TaskModel/RegularTasks.h"
#include "sources/Utils/MatirxConvenient.h"
#include "sources/Utils/colormod.h"
#include "sources/Utils/profilier.h"
namespace SP_OPT_PA {

class RTA_BASE {
   public:
    TaskSet tasks;
    RTA_BASE() {}
    RTA_BASE(const TaskSet &tasks) : tasks(tasks) {}

    /**
     * @brief
     *
     * @param tasks are inverse priority ordered
     * @param index
     * @return double
     */
    virtual double RTA_Common_Warm(double beginTime, int index) {
        CoutError("Calling RTA_Common_Warm that is supposed to be overwriten!");
        return 0;
    }

    VectorDynamic ResponseTimeOfTaskSet(const VectorDynamic &warmStart) {
        int N = tasks.size();
        VectorDynamic res = GenerateVectorDynamic(N);
        if (GlobalVariables::printRTA) {
            std::cout << "Response time analysis of the task set is:"
                      << std::endl;
        }
        for (int i = 0; i < N; i++) {
            res(i, 0) = RTA_Common_Warm(warmStart(i, 0), i);
            if (GlobalVariables::printRTA) {
                std::cout << "Task " << i << ": " << res(i, 0) << std::endl;
            }
            if (res(i, 0) >= INT32_MAX - 10000) {
                int a = 1;
                a *= a;
            }
        }
        return res;
    }

    VectorDynamic ResponseTimeOfTaskSet() {
        BeginTimer("ResponseTimeOfTaskSet");

        VectorDynamic warmStart =
            GetParameterVD<double>(tasks, "executionTime");
        auto res = ResponseTimeOfTaskSet(warmStart);
        EndTimer("ResponseTimeOfTaskSet");
        return res;
    }

    /**
     * @brief
     *
     * @tparam Schedul_Analysis
     * @param tasks
     * @param warmStart
     * @param whetherPrint
     * @param tol: positive value, makes schedulability check more strict
     * @return true: system is schedulable
     * @return false: system is not schedulable
     */
    bool CheckSchedulability(const VectorDynamic &warmStart,
                             bool whetherPrint = false, double tol = 0) {
        int N = tasks.size();
        for (int i = 0; i < N; i++) {
            double rta = RTA_Common_Warm(warmStart(i, 0), i);
            if (whetherPrint)
                std::cout << "response time for task " << i << " is " << rta
                          << " and deadline is "
                          << min(tasks[i].deadline, tasks[i].period)
                          << std::endl;
            if (rta + tol > min(tasks[i].deadline, tasks[i].period)) {
                if (whetherPrint) {
                    std::cout << "The current task set is not schedulable "
                                 "because of task "
                              << i << " "
                              << "!\n";
                }

                return false;
            }
        }
        if (whetherPrint)
            std::cout << std::endl;
        return true;
    }

    bool CheckSchedulability(bool whetherPrint = false) {
        VectorDynamic warmStart =
            GetParameterVD<double>(tasks, "executionTime");
        return CheckSchedulability(warmStart, whetherPrint);
    }

    bool CheckSchedulabilityDirect(const VectorDynamic &rta) {
        int N = tasks.size();
        for (int i = 0; i < N; i++) {
            if (rta(i, 0) > min(tasks[i].deadline, tasks[i].period)) {
                if (GlobalVariables::debugMode == 1)
                    std::cout << "Check2: RTA\n"
                              << rta(i, 0) << ", " << tasks[i].period << "\n";
                return false;
            }
        }
        return true;
    }
};
}  // namespace SP_OPT_PA