#pragma once
#ifndef JOBCEC_H
#define JOBECE_H

#include "sources/TaskModel/RegularTasks.h"
#include "unordered_map"

namespace SP_OPT_PA {
struct JobCEC {
    int taskId;
    LLint jobId;
    JobCEC() : taskId(-1), jobId(0) {}
    JobCEC(int taskId, LLint jobId) : taskId(taskId), jobId(jobId) {}
    JobCEC(std::pair<int, LLint> p) : taskId(p.first), jobId(p.second) {}
    JobCEC GetJobWithinHyperPeriod(const TaskSetInfoDerived &tasksInfo) const {
        return JobCEC(taskId, jobId % tasksInfo.GetVariableSize(taskId));
    }

    bool operator==(const JobCEC &other) const {
        return taskId == other.taskId && jobId == other.jobId;
    }
    bool operator!=(const JobCEC &other) const { return !(*this == other); }

    bool EqualWithinHyperPeriod(const JobCEC &other,
                                const TaskSetInfoDerived &tasksInfo) const {
        return (other.taskId == taskId) &&
               (other.jobId % tasksInfo.GetVariableSize(taskId) ==
                jobId % tasksInfo.GetVariableSize(taskId));
    }

    std::string ToString() const {
        return "T" + std::to_string(taskId) + "_" + std::to_string(jobId);
    }
};

inline double GetDeadline(JobCEC job, const TaskSetInfoDerived &tasksInfo) {
    return tasksInfo.GetTask(job.taskId).period * job.jobId +
           tasksInfo.GetTask(job.taskId).deadline;
}

inline double GetPeriod(JobCEC job, const TaskSetInfoDerived &tasksInfo) {
    return tasksInfo.GetTask(job.taskId).period;
}

inline double GetActivationTime(JobCEC job,
                                const TaskSetInfoDerived &tasksInfo) {
    return tasksInfo.GetTask(job.taskId).period * job.jobId;
}

std::vector<std::pair<std::pair<double, double>, JobCEC>> SortJobSchedule(
    std::vector<std::pair<std::pair<double, double>, JobCEC>> &timeJobVector);

void PrintSchedule(const TaskSetInfoDerived &tasksInfo, const VectorDynamic &x);

// map the job to the first hyper period and return job's unique id
LLint GetJobUniqueId(const JobCEC &jobCEC, const TaskSetInfoDerived &tasksInfo);

// similar as above, except that it maps jobs outside of a hyper-period into one
// hyper-period
LLint GetJobUniqueIdWithinHyperPeriod(const JobCEC &jobCEC,
                                      const TaskSetInfoDerived &tasksInfo);

inline double GetExecutionTime(LLint id, const TaskSetInfoDerived &tasksInfo) {
    return tasksInfo.GetTask(id).getExecutionTime();
}
inline double GetExecutionTime(const JobCEC &jobCEC,
                               const TaskSetInfoDerived &tasksInfo) {
    return tasksInfo.GetTask(jobCEC.taskId).getExecutionTime();
}

}  // namespace SP_OPT_PA

template <>
struct std::hash<SP_OPT_PA::JobCEC> {
    std::size_t operator()(const SP_OPT_PA::JobCEC &jobCEC) const {
        return jobCEC.taskId * 1e4 + jobCEC.jobId;
    }
};

#endif

// class JobPosition {
//    public:
//     JobPosition() {}
//     JobPosition(LLint start, LLint finish) : start_(start), finish_(finish)
//     {}

//     inline void UpdateAfterRemoveInstance(LLint instIndex) {
//         // if (instIndex == start_ || instIndex == finish_) {
//         //   CoutError("Error in UpdateAfterRemoveInstance");
//         // }
//         if (instIndex <= start_)
//             start_--;
//         if (instIndex <= finish_)
//             finish_--;
//     }

//     inline void UpdateAfterInsertInstance(LLint instIndex) {
//         // if (instIndex == start_ || instIndex == finish_) {
//         //   CoutError("Error in UpdateAfterRemoveInstance");
//         // }
//         if (instIndex <= start_)
//             start_++;
//         if (instIndex <= finish_)
//             finish_++;
//     }

//     // data members
//     LLint start_;
//     LLint finish_;
// };