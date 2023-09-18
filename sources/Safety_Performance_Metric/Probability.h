#pragma once
#include <cmath>
#include <unordered_map>
#include <vector>

#include "sources/Utils/testMy.h"
namespace SP_OPT_PA {

class ProbabilityDistributionBase {
   public:
    ProbabilityDistributionBase() {}

    double CDF(double x) const;

    // virtual void UpdateDistribution(std::vector<double> execution_time_data);

    // data members
};

class GaussianDist : public ProbabilityDistributionBase {
   public:
    GaussianDist() {}
    GaussianDist(double mu, double sigma) : mu(mu), sigma(sigma) {}

    inline double CDF(double x) const {
        double z = (x - mu) / (sigma * std::sqrt(2.0));
        return 0.5 * (1.0 + std::erf(z));
    }

    // data
    double mu;
    double sigma;
};

struct Value_Proba {
    Value_Proba() {}
    Value_Proba(double v, double p) : value(v), probability(p) {}

    double value;
    double probability;
};
// stores a probability mass function, i.e., value-probability pair
// this should be the major way to describe probability distribution
class FiniteDist : public ProbabilityDistributionBase {
   public:
    FiniteDist() {}
    FiniteDist(const GaussianDist& gauss_dist, double min_val, double max_val,
               double granularity)
        : min_execution_time(min_val), max_execution_time(max_val) {
        distribution.reserve(granularity);
        if (granularity < 1)
            CoutError("Invalid granularity!");

        double step = (max_val - min_val) / (granularity - 1);
        distribution.push_back(Value_Proba(min_val, gauss_dist.CDF(min_val)));

        for (double execution_time = min_val + step; execution_time <= max_val;
             execution_time += step) {
            double chance = gauss_dist.CDF(execution_time) -
                            gauss_dist.CDF(execution_time - step);
            distribution.push_back(Value_Proba(execution_time, chance));
        }
    }

    FiniteDist(const std::vector<Value_Proba>& distribution_input) {
        distribution = distribution_input;
        SortDist();
        min_execution_time = distribution[0].value;
        max_execution_time = distribution.back().value;
    }

    inline size_t size() const { return distribution.size(); }

    inline Value_Proba& operator[](size_t i) { return distribution[i]; }
    inline const Value_Proba& at(size_t i) { return distribution.at(i); }

    // ''merge'' two distribution
    void Coalesce(const FiniteDist& other);

    // standard convolution to perform probabilitistic addition
    void Convolve(const FiniteDist& other);

    void AddPreemption(const FiniteDist& execution_time, double preempt_time) {}

    void Normalize() {
        int p_sum = 0;
        for (const Value_Proba& element : distribution)
            p_sum += element.probability;
        for (Value_Proba& element : distribution) element.probability /= p_sum;
    }
    std::unordered_map<double, double> GetV_PMap() const {
        std::unordered_map<double, double> m;
        for (const Value_Proba& element : distribution)
            m[element.value] = element.probability;
        return m;
    }

    // sort distribution
    void SortDist() {
        std::sort(distribution.begin(), distribution.end(),
                  [](const Value_Proba& dis1, const Value_Proba& dis2) {
                      return dis1.value < dis2.value;
                  });
    }
    void UpdateDistribution(const std::unordered_map<double, double>& m_v2p);

    // data
    // saves the probability that x<= value
    std::vector<Value_Proba> distribution;
    double min_execution_time;
    double max_execution_time;
};
}  // namespace SP_OPT_PA