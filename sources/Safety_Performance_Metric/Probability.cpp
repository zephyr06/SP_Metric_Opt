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
    UpdateMinMaxValues();
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
        std::upper_bound(distribution.begin(), distribution.end(), preempt_time,
                         [](double preempt_time, const Value_Proba& element) {
                             return preempt_time < element.value;
                         });
    if (itr == distribution.end())
        return {};
    else {
        std::vector<Value_Proba> tail_dist(itr, distribution.end());
        return tail_dist;
    }
}

std::vector<Value_Proba> FiniteDist::GetHeadDistribution(size_t tail_size) {
    std::vector<Value_Proba> head_dist(
        distribution.begin(), distribution.begin() + size() - tail_size);
    return head_dist;
}

// return whether new preemption is added
bool FiniteDist::AddOnePreemption(const FiniteDist& execution_time_dist,
                                  double preempt_time) {
    std::vector<Value_Proba> tail_dist = GetTailDistribution(preempt_time);
    if (tail_dist.size() == 0)
        return false;
    else {
        FiniteDist head(GetHeadDistribution(tail_dist.size()));
        FiniteDist tail(tail_dist);
        tail.Convolve(execution_time_dist);
        head.Coalesce(tail);
        UpdateDistribution(head.distribution);
        return true;
    }
}
void FiniteDist::CompressDeadlineMissProbability(double deadline) {
    auto itr_exceed_ddl =
        std::upper_bound(distribution.begin(), distribution.end(), deadline,
                         [](double deadline, const Value_Proba& element) {
                             return deadline < element.value;
                         });
    double ddl_miss = 0;
    for (auto itr = itr_exceed_ddl; itr != distribution.end(); itr++) {
        ddl_miss += itr->probability;
    }
    distribution.erase(itr_exceed_ddl, distribution.end());
    if (ddl_miss > 0)
        distribution.push_back(Value_Proba(deadline + 1, ddl_miss));
}
void FiniteDist::AddPreemption(const FiniteDist& execution_time_dist_hp,
                               double period_hp, double deadline_this) {
    // backlog
    Convolve(execution_time_dist_hp);
    int hp_instance_considered = 1;
    while (max_time <= deadline_this &&
           max_time > hp_instance_considered * period_hp) {
        AddOnePreemption(execution_time_dist_hp,
                         hp_instance_considered * period_hp);
        hp_instance_considered++;
    }
    CompressDeadlineMissProbability(deadline_this);
}

FiniteDist::FiniteDist(const std::vector<double>& data_raw, int granularity) {
    std::vector<double> data = data_raw;
    std::sort(data.begin(), data.end());
    min_time = data[0];
    max_time = data[data.size() - 1];
    double range = max_time - min_time;
    double step = range / (granularity - 1);

    distribution.reserve(granularity);
    int value_index = 0;
    for (int i = 0; i < granularity; i++) {
        double val = min_time + i * step;
        int count = 0;
        while (value_index < static_cast<int>(data.size()) &&
               data[value_index] - 1e-3 <= val) {
            count++;
            value_index++;
        }
        if (count > 0)
            distribution.push_back(
                Value_Proba(val, double(count) / data.size()));
    }
    double sum = 0;
    for (int i = 0; i < distribution.size(); i++) {
        sum += distribution[i].probability;
    }
    if (abs(sum - 1.0) > 1e-4) {
        CoutError("Error in FiniteDist constructor: sum of probabilities is " +
                  std::to_string(sum));
    }
}

}  // namespace SP_OPT_PA