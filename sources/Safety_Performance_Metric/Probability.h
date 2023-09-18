#pragma once
#include <vector>

namespace SP_OPT {

class ProbabilityDistributionBase {
   public:
    ProbabilityDistributionBase() {}

    virtual double CDF(double x);

    virtual void UpdateDistribution(std::vector<double> execution_time_data);

    // data members
};

// stores a probability mass function, i.e., value-probability pair
// this should be the major way to describe probability distribution
class FiniteDist : public ProbabilityDistributionBase {
    FiniteDist() {}
    FiniteDist(double granularity) {}
};

class GaussianDist : public ProbabilityDistributionBase {};

}  // namespace SP_OPT