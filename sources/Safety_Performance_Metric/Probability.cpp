#include "sources/Safety_Performance_Metric/Probability.h"

#include <unordered_map>

namespace SP_OPT_PA {

void FiniteDist::Coalesce(const FiniteDist& other) {
    std::unordered_map<int, double> rec_this = GetV_PMap();
    for (const Value_Proba& element : other.distribution) {
        if (rec_this.count(element.value))
            rec_this[element.value] += element.probability;
        else
            rec_this[element.value] = element.probability;
    }
    distribution.clear();
    distribution.reserve(rec_this.size());
    for (auto itr = rec_this.begin(); itr != rec_this.end(); itr++) {
        distribution.push_back(Value_Proba(itr->first, itr->second));
    }
    SortDist();
}

}  // namespace SP_OPT_PA