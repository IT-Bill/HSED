#include <iostream>
#include <iomanip>
#include <chrono>

int main() {
    auto clockFunc = +[]() -> std::chrono::steady_clock::time_point { return std::chrono::steady_clock::now(); };

    auto startTime = clockFunc();
    auto endTime = clockFunc();

    std::chrono::duration<double> a = endTime - startTime;
    std::cout << a.count() << std::endl;
}
