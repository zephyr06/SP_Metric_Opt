#pragma once
#include <vector>

namespace SP_OPT_PA {

struct SP_Parameters {
    SP_Parameters() {}
    SP_Parameters(int n) { thresholds = std::vector<double>(n, 0.5); }
    // data
    std::vector<double> thresholds;
};
}  // namespace SP_OPT_PA