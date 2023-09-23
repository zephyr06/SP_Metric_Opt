#pragma once
#include "sources/RTDA/ObjDataAge.h"
#include "sources/RTDA/ObjReactionTime.h"

inline bool IfRT_Trait(const std::string &trait) {
    return trait == "ReactionTime" || trait == "ReactionTimeApprox";
}
