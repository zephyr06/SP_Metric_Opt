#include "Parameters.h"

#include <yaml-cpp/yaml.h>

namespace GlobalVariables {
const std::string PROJECT_PATH = std::string(PROJECT_ROOT_DIR) + "/";
YAML::Node loaded_doc =
    YAML::LoadFile(PROJECT_PATH + "sources/parameters.yaml");

// optimization settings
int debugMode = loaded_doc["debugMode"].as<int>();
int TIME_LIMIT = loaded_doc["TIME_LIMIT"].as<int>();
int printRTA = loaded_doc["printRTA"].as<int>();

const std::string priorityMode = loaded_doc["priorityMode"].as<std::string>();

int Granularity = loaded_doc["Granularity"].as<int>();
}  // namespace GlobalVariables