#include "RegularTasks.h"
namespace SP_OPT_PA {

// Recursive function to return gcd of a and b
long long gcd(long long int a, long long int b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Function to return LCM of two numbers
long long int lcm(long long int a, long long int b) {
    return (a / gcd(a, b)) * b;
}

long long int HyperPeriod(const TaskSet &tasks) {
    int N = tasks.size();
    if (N == 0) {
        std::cout << Color::red << "Empty task set in HyperPeriod()!\n";
        throw;
    } else {
        long long int hyper = tasks[0].period;
        for (int i = 1; i < N; i++) {
            hyper = lcm(hyper, tasks[i].period);
            if (hyper < 0 || hyper > LLONG_MAX) {
                std::cout << Color::red << "The hyper-period over flows!"
                          << Color::def << std::endl;
                throw;
            }
        }
        return hyper;
    }
}

}  // namespace SP_OPT_PA