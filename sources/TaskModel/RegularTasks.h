#pragma once

#include <math.h>

#include <fstream>
#include <iostream>
#include <vector>

#include "sources/Safety_Performance_Metric/Probability.h"
#include "sources/Utils/DeclareDAG.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/colormod.h"
#include "sources/Utils/testMy.h"

typedef std::map<int, std::vector<int>> ProcessorTaskSet;

namespace SP_OPT_PA {

inline bool CompareStringNoCase(const std::string &s1, const std::string s2) {
    return strcasecmp(s1.c_str(), s2.c_str()) == 0;
}
class Task {
   public:
    // Member list
    int id;
    FiniteDist execution_time_dist;
    double period;
    double deadline;
    double priority;

    Task(int id, const FiniteDist &exec, double period, double ddl,
         double priority)
        : id(id),
          execution_time_dist(exec),
          deadline(ddl),
          period(period),
          priority(priority) {}

    /**
     * only used in ReadTaskSet because the input parameter's type is int
     **/
    Task(std::vector<double> dataInLine) {
        id = dataInLine[0];
        period = dataInLine[1];
        // executionTime = dataInLine[2];
        deadline = dataInLine[3];
    }

    void print() {
        std::cout << "The period is: " << period << " The deadline is "
                  << deadline << std::endl;
    }
};
typedef std::vector<SP_OPT_PA::Task> TaskSet;

long long int HyperPeriod(const TaskSet &tasks);
}  // namespace SP_OPT_PA