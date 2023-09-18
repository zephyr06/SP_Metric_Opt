#include "sources/Safety_Performance_Metric/Probability.h"

namespace SP_OPT_PA {

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

}  // namespace SP_OPT_PA