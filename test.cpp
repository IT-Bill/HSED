#include <iostream>
#include <cstdio>
#include <memory>

int main() {
    // 使用 popen 运行 Python 脚本并读取其输出
    std::shared_ptr<FILE> pipe(popen("python3 script/gsl_mpmath.py 73 -2.457 0.129421421", "r"), pclose);
    if (!pipe) {
        std::cerr << "Failed to run Python script." << std::endl;
        return 1;
    }

    // 从 pipe 中读取输出
    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }

    // 将结果字符串转换为 double
    double output_value = std::stod(result);
    std::cout << output_value << std::endl;
    return 0;
}