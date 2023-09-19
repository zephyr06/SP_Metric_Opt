// #include <gtest/gtest.h>

#include "gmock/gmock.h"  // Brings in gMock.
#include "sources/TaskModel/RegularTasks.h"
#include "sources/Utils/Parameters.h"
#include "sources/Utils/readwrite.h"
using ::testing::AtLeast;  // #1
using ::testing::Return;
using namespace std;
using namespace SP_OPT_PA;
using namespace GlobalVariables;
TEST(BasicExample, v1) {
    vector<double> data = ReadTxtFile("test");
    EXPECT_EQ(5, data.size());
    for (int i = 0; i < 5; i++) EXPECT_EQ(i + 1, data[i]);
}
TEST(read_write, task_sets) {
    std::string file_name = "test_robotics_v1";
    string path =
        GlobalVariables::PROJECT_PATH + "TaskData/" + file_name + ".yaml";
    TaskSet tasks = ReadTaskSet(path);
    EXPECT_EQ(4, tasks.size());
    EXPECT_EQ(20, tasks[0].period);
}

int main(int argc, char **argv) {
    // ::testing::InitGoogleTest(&argc, argv);
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}