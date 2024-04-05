#pragma once
#include <Eigen/Core>
#include <iostream>

#include "sources/Utils/testMy.h"
// All the global variables should be const

namespace GlobalVariables {

extern const std::string PROJECT_PATH;

// optimization settings
extern int debugMode;
extern int TIME_LIMIT;
extern int printRTA;

extern int Granularity;
extern int Layer_Node_During_Incremental_Optimization;
}  // namespace GlobalVariables