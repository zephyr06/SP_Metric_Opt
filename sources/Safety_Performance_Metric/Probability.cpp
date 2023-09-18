#include "sources/Safety_Performance_Metric/Probability.h"

namespace SP_OPT_PA {

std::unordered_map<double, double> FiniteDist::GetV_PMap() const {
    std::unordered_map<double, double> m;
    for (const Value_Proba& element : distribution)
        m[element.value] = element.probability;
    return m;
}

// O(n log(n))
void FiniteDist::UpdateDistribution(
    const std::unordered_map<double, double>& m_v2p) {
    distribution.clear();
    distribution.reserve(m_v2p.size());
    for (auto itr = m_v2p.begin(); itr != m_v2p.end(); itr++) {
        distribution.push_back(Value_Proba(itr->first, itr->second));
    }
    SortDist();
}

// O(n log(n))
void FiniteDist::Coalesce(const FiniteDist& other) {
    std::unordered_map<double, double> m_v2p = GetV_PMap();
    for (const Value_Proba& element : other.distribution) {
        if (m_v2p.count(element.value))
            m_v2p[element.value] += element.probability;
        else
            m_v2p[element.value] = element.probability;
    }
    UpdateDistribution(m_v2p);
}

// O(n^2)
void FiniteDist::Convolve(const FiniteDist& other) {
    std::unordered_map<double, double> m_v2p;
    for (const auto& element_this : distribution) {
        for (const auto& element_other : other.distribution) {
            double value = element_this.value + element_other.value;
            double prob = element_this.probability * element_other.probability;
            if (m_v2p.count(value))
                m_v2p[value] += prob;
            else
                m_v2p[value] = prob;
        }
    }
    UpdateDistribution(m_v2p);
}

std::vector<Value_Proba> FiniteDist::GetTailDistribution(double preempt_time) {
    auto itr =
        lower_bound(distribution.begin(), distribution.end(), preempt_time,
                    [](const Value_Proba& element, double preempt_time) {
                        return element.value < preempt_time;
                    });
    if (itr == distribution.end())
        return {};
    else {
        std::vector<Value_Proba> tail_dist(itr, distribution.end());
        return tail_dist;
    }
}

void FiniteDist::AddPreemption(const FiniteDist& execution_time_dist,
                               double preempt_time) {
    std::vector<Value_Proba> tail_dist = GetTailDistribution(preempt_time);
}
}  // namespace SP_OPT_PA