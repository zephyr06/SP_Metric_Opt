#include <time.h>

#include <cmath>

#include "sources/TaskModel/DAG_Model.h"
#include "sources/TaskModel/RegularTasks.h"
#include "sources/Utils/Parameters.h"

using namespace RegularTaskSystem;

// std::vector<int> PeriodSetAM = {1, 2, 5, 10, 20, 50, 100, 200, 1000};
// std::vector<int> PeriodSetAM = {2, 5, 10, 20, 50, 100, 200};
static std::vector<int> PeriodSetAM = {100, 200, 300};
// std::vector<int> PeriodSetAM = {100, 200, 300, 400, 500, 600, 800, 1000,
// 1200};
static const std::vector<int> PeriodSetWaters = {1,  2,   5,   10,  20,
                                                 50, 100, 200, 1000};
static const std::vector<int> PeriodPDFWaters = {3, 2, 2, 25, 25, 3, 20, 1, 4};
static const std::vector<int> PeriodCDFWaters = {3,  5,  7,  32, 57,
                                                 60, 80, 81, 85};

std::vector<double> Uunifast(int N, double utilAll, bool boundU = true);

struct TaskSetGenerationParameters {
  // data members
  int N;
  double totalUtilization_min;
  double totalUtilization_max;
  int numberOfProcessor;
  int coreRequireMax;
  double parallelismFactor;
  int period_generation_type;
  int deadlineType;
  int numCauseEffectChain;
  int chain_length;
  int SF_ForkNum;
  int fork_sensor_num_min;
  int fork_sensor_num_max;
};

TaskSet GenerateTaskSet(int N, double totalUtilization, int numberOfProcessor,
                        int coreRequireMax, int taskSetType = 1,
                        int deadlineType = 0);

void WriteTaskSets(std::ofstream &file, TaskSet &tasks);

using namespace SP_OPT;
DAG_Model GenerateDAG(const TaskSetGenerationParameters &tasks_gene_param);

void WriteDAG(std::ofstream &file, DAG_Model &tasksDAG);