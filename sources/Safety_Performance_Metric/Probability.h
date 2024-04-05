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
    GaussianDist(double mu, double sigma) : mu(mu), sigma(sigma) {
        if (abs(sigma) < 1e-6)
            CoutError("Invalid sigma value!");
    }

    inline double CDF(double x) const {
        double z = (x - mu) / (sigma * std::sqrt(2.0));
        return 0.5 * (1.0 + std::erf(z));
    }
    inline bool operator==(const GaussianDist& other) const {
        return mu == other.mu && sigma == other.sigma;
    }
    inline bool operator!=(const GaussianDist& other) const {
        return !(*this == other);
    }

    // data
    double mu;
    double sigma;
};

struct Value_Proba {
    Value_Proba() {}
    Value_Proba(double v, double p) : value(v), probability(p) {}

    inline bool operator==(const Value_Proba& other) const {
        double tolerance = 1e-4;
        return abs(value - other.value) < tolerance &&
               abs(probability - other.probability) < tolerance;
    }
    inline bool operator!=(const Value_Proba& other) const {
        return !(*this == other);
    }

    double value;
    double probability;
};

inline bool ifDiff(double a, double b, double tolerance) {
    return std::abs(a - b) > tolerance;
}

// stores a probability mass function, i.e., value-probability pair
// this should be the major way to describe probability distribution
class FiniteDist : public ProbabilityDistributionBase {
   public:
    FiniteDist() {}
    FiniteDist(const GaussianDist& gauss_dist, double min_val, double max_val,
               int granularity);

    FiniteDist(const GaussianDist& gauss_dist, int granularity)
        : FiniteDist(gauss_dist, gauss_dist.mu - 2.3263 * gauss_dist.sigma,
                     gauss_dist.mu + 2.3263 * gauss_dist.sigma, granularity) {}

    FiniteDist(const std::vector<Value_Proba>& distribution_input) {
        UpdateDistribution(distribution_input);
    }

    FiniteDist(const std::vector<double>& data_raw, int granularity);

    void CheckDistributionValid() const;
    void UpdateDistribution(
        const std::vector<Value_Proba>& distribution_input) {
        distribution = distribution_input;
        SortDist();
        UpdateMinMaxValues();
    }

    void UpdateMinMaxValues() {
        if (size() > 0) {
            min_time = distribution[0].value;
            max_time = distribution.back().value;
        }
    }

    inline size_t size() const { return distribution.size(); }

    inline Value_Proba& operator[](size_t i) { return distribution[i]; }
    inline const Value_Proba& at(size_t i) { return distribution.at(i); }

    // ''merge'' two distribution
    void Coalesce(const FiniteDist& other);

    // standard convolution to perform probabilitistic addition
    void Convolve(const FiniteDist& other);

    void Normalize() {
        int p_sum = 0;
        for (const Value_Proba& element : distribution)
            p_sum += element.probability;
        for (Value_Proba& element : distribution) element.probability /= p_sum;
    }

    std::unordered_map<double, double> GetV_PMap() const;

    // sort distribution
    inline void SortDist() {
        std::sort(distribution.begin(), distribution.end(),
                  [](const Value_Proba& dis1, const Value_Proba& dis2) {
                      return dis1.value < dis2.value;
                  });
    }
    void UpdateDistribution(const std::unordered_map<double, double>& m_v2p);

    std::vector<Value_Proba> GetTailDistribution(double preempt_time);
    std::vector<Value_Proba> GetHeadDistribution(size_t tail_size);

    bool AddOnePreemption(const FiniteDist& execution_time_dist,
                          double preempt_time);
    void AddPreemption(const FiniteDist& execution_time_dist_hp,
                       double period_hp, double dealine_this);
    void CompressDeadlineMissProbability(double deadline);

    void print() const {
        for (const auto vp : distribution)
            std::cout << "(" << vp.value << ", " << vp.probability << ") ";
        std::cout << "\n";
    }

    void Scale(double k) {
        for (Value_Proba& pair : distribution) pair.value *= k;
    }

    bool operator==(const FiniteDist& other) const {
        if (distribution.size() != other.distribution.size())
            return false;
        double tolerance = 1e-5;
        for (uint i = 0; i < distribution.size(); i++) {
            if (distribution[i] != other.distribution[i])
                return false;
        }
        if (ifDiff(min_time, other.min_time, tolerance))
            return false;
        if (ifDiff(max_time, other.max_time, tolerance))
            return false;
        return true;
    }
    bool operator!=(const FiniteDist& other) const {
        return !((*this) == other);
    }

    double CDF(double x) const;

    // data members
    // saves the probability that x<= value, this is probability mass function
    // rather than cumulative function
    std::vector<Value_Proba> distribution;
    double min_time;
    double max_time;
};

}  // namespace SP_OPT_PA