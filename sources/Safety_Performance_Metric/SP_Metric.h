#pragma once
#include "sources/Safety_Performance_Metric/ParametersSP.h"
#include "sources/Safety_Performance_Metric/Probability.h"
#include "sources/Safety_Performance_Metric/RTA.h"
#include "sources/TaskModel/RegularTasks.h"

namespace SP_OPT_PA {

inline double PenaltyFunc(double violate_probability, double threshold) {
    return -0.01 * exp(10 * abs(threshold - violate_probability));
}
inline double RewardFunc(double violate_probability, double threshold) {
    return log((threshold - violate_probability) + 1);
}
inline double SP_Func(double violate_probability, double threshold) {
    if (threshold >= violate_probability)
        return RewardFunc(violate_probability, threshold);
    else
        return PenaltyFunc(violate_probability, threshold);
}

double ObtainSP(const TaskSet& tasks, const SP_Parameters& sp_parameters);

}  // namespace SP_OPT_PA