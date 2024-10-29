#include "common.hpp"


class DoubleFunction {
public:
    //第二种误差检测方法，操作double的尾数位，且增加显著误差判断分支决定起始操作的尾数位位数
    static std::pair<double, double> processPositiveRangeLayer1(double start, double end);
    static void processPositiveRangeLayer23(double input_x, double ULP, double start, double end, bool skip2 = false);
    static std::pair<double, double> processNegativeRangeLayer1(double start, double end);
    static void processNegativeRangeLayer23(double input_x, double ULP, double start, double end, bool skip2 = false);
    static std::pair<double, double> processCrossZeroLayer1(double start, double end);
    static void processCrossZeroLayer23(double input_x, double ULP, double start, double end, bool skip2 = false);
    //随机采样算法（目的是于模拟退火算法进行效果对比）
    static vector<double> random_test(const std::vector<std::pair<double, double>> &intervals);
    static vector<double> random_test(double start, double end, int num = 100000);
};
