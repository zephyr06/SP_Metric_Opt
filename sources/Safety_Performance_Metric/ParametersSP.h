#pragma once
#include <vector>

namespace SP_OPT_PA {

struct SP_Parameters {
    SP_Parameters() {}
    SP_Parameters(int n_node) {
        thresholds_node = std::vector<double>(n_node, 0.5);
    }
    SP_Parameters(int n_node, int n_path) {
        thresholds_node = std::vector<double>(n_node, 0.5);
        thresholds_path = std::vector<double>(n_path, 0.5);
    }

    // data
    std::vector<double> thresholds_node;
    std::vector<double> thresholds_path;
};
}  // namespace SP_OPT_PA