#pragma once
#include <cmath>
#include <vector>

namespace SP_OPT_PA {

class ProbabilityDistributionBase {
   public:
    ProbabilityDistributionBase() {}

    double CDF(double x);

    // virtual void UpdateDistribution(std::vector<double> execution_time_data);

    // data members
};

// stores a probability mass function, i.e., value-probability pair
// this should be the major way to describe probability distribution
class FiniteDist : public ProbabilityDistributionBase {
    FiniteDist() {}
    FiniteDist(double granularity) {}
};

class GaussianDist : public ProbabilityDistributionBase {
   public:
    GaussianDist() {}
    GaussianDist(double mu, double sigma) : mu(mu), sigma(sigma) {}

    inline double CDF(double x) {
        double z = (x - mu) / (sigma * std::sqrt(2.0));
        return 0.5 * (1.0 + std::erf(z));
    }

    // data
    double mu;
    double sigma;
};

}  // namespace SP_OPT_PA