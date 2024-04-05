#pragma once

#include <math.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "sources/Safety_Performance_Metric/Probability.h"
#include "sources/Utils/DeclareDAG.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/colormod.h"
#include "sources/Utils/testMy.h"

typedef std::map<int, std::vector<int>> ProcessorTaskSet;

namespace SP_OPT_PA {

inline bool CompareStringNoCase(const std::string& s1, const std::string s2) {
    return strcasecmp(s1.c_str(), s2.c_str()) == 0;
}
class Task {
   public:
    // Task() {}
    // Default priority assignment is the index of the task in TaskSet
    Task(int id, const FiniteDist& exec, double period, double ddl,
         double priority, std::string name = "")
        : id(id),
          execution_time_dist(exec),
          period(period),
          deadline(ddl),
          priority(priority),
          name(name) {
        if (name == "")
            name = "Task_" + std::to_string(id);
        processorId = 0;
        executionTime = -1;
    }

    void print() {
        std::cout << "The period is: " << period << " The deadline is "
                  << deadline << std::endl;
    }

    double getExecutionTime() const {
        if (executionTime > 0)
            return executionTime;
        else
            CoutError("Execution time is not set!");
        return 0;
    }
    void setExecutionTime(double x) { executionTime = x; }

    // TODO: test this scale function
    void Scale(double k) {
        period *= k;
        executionTime *= k;
        execution_time_dist.Scale(k);
        deadline *= k;
    }

    double utilization() const { return double(getExecutionTime()) / period; }

    inline void setExecGaussian(const GaussianDist& exec_gau) {
        exec_time_gauss = exec_gau;
    }

    inline GaussianDist getExecGaussian() const { return exec_time_gauss; }
    // Member list
    int id;
    FiniteDist execution_time_dist;
    int period;  // must be integer for the system simulation to work; if not
                 // integer, must be scaled to be so;
    double deadline;
    double priority;   // smaller value mean higher priority
    std::string name;  // optional
    int processorId;

   private:
    double executionTime;
    GaussianDist exec_time_gauss;  // not used except IO part
};
typedef std::vector<SP_OPT_PA::Task> TaskSet;

inline double Utilization(const TaskSet& tasks) {
    double utilization = 0;
    for (uint i = 0; i < tasks.size(); i++)
        utilization += tasks[i].utilization();
    return utilization;
}

void ScaleToInteger(TaskSet& tasks);

inline void SortTasksByPriority(TaskSet& tasks) {
    std::sort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
        return t1.priority < t2.priority;
    });
}

long long int HyperPeriod(const TaskSet& tasks);

TaskSet ReadTaskSet(std::string path,
                    int granulairty = GlobalVariables::Granularity);

void AssignTaskSetPriorityById(TaskSet& tasks);
/**
 * @brief
 *
 * @param tasks
 * @return ProcessorTaskSet map type! processorId tostd::vector<task ID>
 */
ProcessorTaskSet ExtractProcessorTaskSet(const TaskSet& tasks);
class TaskSetInfoDerived {
   public:
    TaskSetInfoDerived() {}

    TaskSetInfoDerived(const TaskSet& tasksInput) {
        tasks = tasksInput;
        N = tasks.size();
        hyper_period = HyperPeriod(tasks);
        variableDimension = 0;
        length = 0;
        for (int i = 0; i < N; i++) {
            LLint size = hyper_period / tasks[i].period;
            sizeOfVariables.push_back(size);
            variableDimension += size;
            length += sizeOfVariables[i];
        }
        processorTaskSet = ExtractProcessorTaskSet(tasks);
        RecordTaskPosition();
    }

    inline const Task& GetTask(uint task_id) const {
        return tasks[task_id2position_.at(task_id)];
    }
    inline const TaskSet& GetTaskSet() const { return tasks; }

    void RecordTaskPosition() {
        for (int i = 0; i < static_cast<int>(tasks.size()); i++) {
            task_id2position_[tasks[i].id] = i;
        }
    }
    inline int GetVariableSize(uint task_id) const {
        return sizeOfVariables[task_id2position_.at(task_id)];
    }

    // data members
    int N;
    LLint variableDimension;
    LLint hyper_period;
    LLint length;
    ProcessorTaskSet processorTaskSet;
    std::unordered_map<int, int> task_id2position_;

    TaskSet tasks;

   private:
    std::vector<LLint> sizeOfVariables;
};
int GetHyperPeriod(const TaskSetInfoDerived& tasks_info,
                   const std::vector<int>& chain);

template <typename T>
VectorDynamic GetParameterVD(const TaskSet& taskset,
                             std::string parameterType) {
    uint N = taskset.size();
    VectorDynamic parameterList;
    parameterList.resize(N, 1);
    parameterList.setZero();

    for (uint i = 0; i < N; i++) {
        if (parameterType == "period")
            parameterList(i, 0) = ((T)(taskset[i].period));
        else if (parameterType == "executionTime")
            parameterList(i, 0) = ((T)(taskset[i].getExecutionTime()));
        else if (parameterType == "deadline")
            parameterList(i, 0) = ((T)(taskset[i].deadline));
        else {
            std::cout << Color::red
                      << "parameterType in GetParameter is not recognized!\n"
                      << Color::def << std::endl;
            throw;
        }
    }
    return parameterList;
}

template <typename T>
std::vector<T> GetParameter(const TaskSet& taskset, std::string parameterType) {
    VectorDynamic resEigen = GetParameterVD<T>(taskset, parameterType);
    return Eigen2Vector<T>(resEigen);
}

void WriteTaskSet(std::string path, const TaskSet& tasks);
}  // namespace SP_OPT_PA