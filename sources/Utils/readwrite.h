#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "sources/Utils/testMy.h"

namespace SP_OPT_PA {
std::vector<double> ReadTxtFile(std::string kernel_name) {
    std::string path = std::string(PROJECT_ROOT_DIR) +
                       "/ExperimentsData/FormatedData/" + kernel_name + ".txt";
    std::ifstream inputFile(path);

    if (!inputFile.is_open()) {
        CoutError("Failed to open the file.");
    }

    std::vector<double> data;
    std::string line;

    while (std::getline(inputFile, line)) {
        std::istringstream iss(line);
        double value;

        while (iss >> value) {
            data.push_back(value);
        }
    }

    inputFile.close();

    return data;
}
}  // namespace SP_OPT_PA