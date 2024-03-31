#include "sources/Safety_Performance_Metric/ParametersSP.h"

namespace SP_OPT_PA {
SP_Parameters ReadSP_Parameters(const std::string& path) {
    if (!(std::filesystem::exists(path))) CoutError(path + " not exist!");
    YAML::Node config = YAML::LoadFile(path);
    YAML::Node tasksNode;
    if (config["tasks"]) {
        tasksNode = config["tasks"];
    } else {
        CoutError("Input file doesn't follow DAG format: " + path);
    }
    SP_Parameters parameters;
    parameters.reserve(tasksNode.size());

    for (size_t i = 0; i < tasksNode.size(); i++) {
        if (tasksNode[i]["sp_threshold"]) {
            parameters.thresholds_node.push_back(
                tasksNode[i]["sp_threshold"].as<double>());
        } else {
            parameters.thresholds_node.push_back(0.5);
        }
        if (tasksNode[i]["sp_Weight"]) {
            parameters.weights_node.push_back(
                tasksNode[i]["sp_Weight"].as<double>());
        } else {
            parameters.weights_node.push_back(1);
        }
    }

    if (config["chains"]) {
        YAML::Node chainsNode = config["chains"];
        for (size_t i = 0; i < chainsNode.size(); i++) {
            if (chainsNode[i]["sp_threshold"]) {
                parameters.thresholds_path.push_back(
                    chainsNode[i]["sp_threshold"].as<double>());
            } else {
                parameters.thresholds_path.push_back(0.5);
            }
            if (chainsNode[i]["sp_Weight"]) {
                parameters.weights_path.push_back(
                    chainsNode[i]["sp_Weight"].as<double>());
            } else {
                parameters.weights_path.push_back(1);
            }
        }
    }
    return parameters;
}
}  // namespace SP_OPT_PA