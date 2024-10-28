#include "float.hpp"
#include "getresult.hpp"
#include <random>
#include <utility>
#include <vector>
#include <chrono>

typedef union {
    int i;
    float f;
} FL;
typedef union {
    long int i;
    double d;
} DL;

extern std::vector<double> layer2Input;
extern std::vector<double> layer3Input;
bool layer2Flag;

std::pair<double, double> DoubleFunction::processPositiveRangeLayer1(const double &start, const double &end) {
    DL dl_half_start, dl_half_end;
    dl_half_start.d = start;
    dl_half_start.i = dl_half_start.i & 0x7FFFFC0000000000;
    dl_half_end.d = end;
    dl_half_end.i = dl_half_end.i & 0x7FFFFC0000000000;
    // cout << "Detection interval: [" << start << ", " << end << "]" << endl;
    double input_x = 0.0, ULP = 0.0;
    for (long int i = dl_half_start.i; i <= dl_half_end.i; i += 0x40000000000) {
        DL dl_input;
        dl_input.i = i;
        double input_one = dl_input.d;
        std::cout << input_one << " " << i << std::endl;
        double origin = getDoubleOfOrigin(input_one);
        double ulp = getULP(input_one, origin);
        if (isinf(ulp)) {
            continue;
        } else if (isnan(ulp)) {
            continue;
        }
        if (ULP < ulp) {
            ULP = ulp;
            input_x = input_one;
        }
    }
    return std::make_pair(input_x, ULP);
}

//第二种检测方法，操作double的尾数位和增加显著误差判断分支,其中start和end都大于等于0
void DoubleFunction::processPositiveRangeLayer23(double input_x, double ULP, double start, double end, const std::vector<std::pair<double, double>> &intervals) {

    double origin_relative = getDoubleOfOrigin(input_x);
    double relative = getRelativeError(input_x, origin_relative);
    double input_x2 = 0, ULP2 = 0;
    printf("preprocessing: x = %.6lf, maximumULP = %.2lf, maximumRelative = %e\n", input_x, ULP, relative);
    cout << endl;
    if (ULP <= 100) {
        cout << "---------------No significant error, excute two-layer search------------------" << endl;
        double ULP2 = 0.0, input_x2 = 0.0;
        DL dl_float_start, dl_float_end;
        dl_float_start.d = start;
        dl_float_start.i = dl_float_start.i & 0x7FFFFFFFE0000000;
        dl_float_end.d = end;
        dl_float_end.i = dl_float_end.i & 0x7FFFFFFFE0000000;
        layer2Flag = true;
        vector<double> vec1 = random_test(dl_float_start.d, dl_float_end.d);
        layer2Flag = false;
        input_x2 = vec1[0];
        ULP2 = vec1[1];
        origin_relative = getDoubleOfOrigin(input_x2);
        relative = getRelativeError(input_x2, origin_relative);
        printf("float-precision layer: x = %.8lf, maximumULP = %.2lf, maximumRelative = %e\n", input_x2, ULP2, relative);
        cout << endl;
        DL dl_double_start, dl_double_end;
        dl_double_start.d = input_x2;
        dl_double_end.d = input_x2;
        dl_double_start.i = dl_double_start.i & 0x7FFFFFFFE0000000;
        dl_double_end.i = dl_double_end.i | 0x000000001FFFFFFF;
        vector<double> vec2 = random_test(dl_double_start.d, dl_double_end.d);
        input_x2 = vec2[0];
        ULP2 = vec2[1];
        origin_relative = getDoubleOfOrigin(input_x2);
        relative = getRelativeError(input_x2, origin_relative);
        printf("double-precision layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, BitsError = %.1lf\n", input_x2, ULP2, relative, log2(ULP2 + 1));
    } else {
        cout << "--------------------existing significant error, excute three-layer search----------------------" << endl;
        origin_relative = getDoubleOfOrigin(input_x);
        relative = getRelativeError(input_x, origin_relative);
        printf("half-precisoin layer: x = %.6lf, maximumULP = %.2lf, maximumRelative = %e\n", input_x, ULP, relative);
        cout << endl;
        // !==============================================
        auto startTime = std::chrono::steady_clock::now();
        double input_second = input_x;
        DL dl_float_start, dl_float_end;
        dl_float_start.d = input_second;
        dl_float_end.i = dl_float_start.i | 0x000003FFE0000000;
        for (long int i = dl_float_start.i; i <= dl_float_end.i; i += 0x20000000) {
            DL dl_input;
            dl_input.i = i;
            double input_two = dl_input.d;
            double origin = getDoubleOfOrigin(input_two);
            double ulp = getULP(input_two, origin);
            if (isinf(ulp)) { continue; }
            if (ULP < ulp) {
                ULP = ulp;
                input_x = input_two;
            }
            layer2Input.emplace_back(dl_input.d);
        }
        origin_relative = getDoubleOfOrigin(input_x);
        relative = getRelativeError(input_x, origin_relative);
        auto finishTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = finishTime - startTime;
        printf("float-precisoin layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, elapsedTime = %.6f", input_x, ULP, relative, elapsedTime.count());
        // !==============================================
        cout << endl;
        DL dl_double_start, dl_double_end;
        dl_double_start.d = input_x;
        dl_double_end.i = dl_double_start.i | 0x000000001FFFFFFF;
        vector<double> vec3 = random_test(dl_double_start.d, dl_double_end.d);
        input_x = vec3[0];
        ULP = vec3[1];
        origin_relative = getDoubleOfOrigin(input_x);
        relative = getRelativeError(input_x, origin_relative);
        printf("double-precision layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, BitsError = %.1lf\n", input_x, ULP, relative, log2(ULP + 1));
    }
}


std::pair<double, double> DoubleFunction::processNegativeRangeLayer1(const double &start, const double &end) {
    DL dl_first_start, dl_first_end;
    dl_first_start.d = start, dl_first_end.d = end;
    dl_first_start.i = dl_first_start.i & 0xFFFFFC0000000000;
    dl_first_end.i = dl_first_end.i & 0xFFFFFC0000000000;
    // cout << "detection interval[" << start << ", " << end << "]" << endl;
    double input_x = 0.0, ULP = 0.0, relative = 0.0;
    for (long int i = dl_first_end.i; i <= dl_first_start.i; i += 0x40000000000) {
        DL dl_input;
        dl_input.i = i;
        double input_one = dl_input.d;
        double origin = getDoubleOfOrigin(input_one);
        double ulp = getULP(input_one, origin);
        if (isinf(ulp)) { continue; }
        if (ULP < ulp) {
            ULP = ulp;
            input_x = input_one;
        }
    }
    return std::make_pair(input_x, ULP);
}

//start和end都小于等于0的情况
void DoubleFunction::processNegativeRangeLayer23(double input_x, double ULP, double start, double end, const std::vector<std::pair<double, double>> &intervals) {

    double origin_relative = getDoubleOfOrigin(input_x);
    double relative = getRelativeError(input_x, origin_relative);
    double input_x2 = 0, ULP2 = 0;
    printf("preprocessing: x = %.6lf, maximumULP = %.2lf, maximumRelative = %e\n", input_x, ULP, relative);
    if (ULP <= 100) {
        cout << "----------------No significant error, excute two-layer search-------------------" << endl;
        double ULP2 = 0.0, input_x2 = 0.0;
        DL dl_float_start, dl_float_end;
        dl_float_start.d = start;
        dl_float_start.i = dl_float_start.i & 0xFFFFFFFFE0000000;
        dl_float_end.d = end;
        dl_float_end.i = dl_float_end.i & 0xFFFFFFFFE0000000;
        layer2Flag = true;
        vector<double> vec1 = random_test(dl_float_start.d, dl_float_end.d);
        layer2Flag = false;
        input_x2 = vec1[0];
        ULP2 = vec1[1];

        printf("float-precision layer: x = %.8lf, maximumULP = %.2lf\n", input_x2, ULP2);
        cout << endl;
        DL dl_double_start, dl_double_end;
        dl_double_start.d = input_x2;
        dl_double_end.d = input_x2;
        dl_double_start.i = dl_double_start.i & 0xFFFFFFFFE0000000;
        dl_double_end.i = dl_double_end.i | 0x000000001FFFFFFF;

        vector<double> vec2 = random_test(dl_double_start.d, dl_double_end.d);
        input_x2 = vec2[0];
        ULP2 = vec2[1];
        origin_relative = getDoubleOfOrigin(input_x2);
        relative = getRelativeError(input_x2, origin_relative);
        printf("double-precision layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, BitsError = %.1lf\n", input_x2, ULP2, relative, log2(ULP2 + 1));
    } else {
        cout << "-------------Existing significant error, excute three-layer search----------------" << endl;
        printf("half-precision layer: x = %.6lf, maximumULP = %.2lf\n", input_x, ULP);
        cout << endl;
        // !=========================================================================
        auto startTime = std::chrono::steady_clock::now();
        double input_second = input_x;
        DL dl_float_start, dl_float_end;
        dl_float_start.d = input_second;
        dl_float_end.i = dl_float_start.i | 0x000003FFE0000000;
        for (long int i = dl_float_end.i; i <= dl_float_start.i; i += 0x20000000) {
            DL dl_input;
            dl_input.i = i;
            double input_two = dl_input.d;
            double origin = getDoubleOfOrigin(input_two);
            double ulp = getULP(input_two, origin);
            if (isinf(ulp)) { continue; }
            if (ULP < ulp) {
                ULP = ulp;
                input_x = input_two;
            }
            layer2Input.emplace_back(dl_input.d);
        }
        origin_relative = getDoubleOfOrigin(input_x);
        relative = getRelativeError(input_x, origin_relative);
        auto finishTime = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsedTime = finishTime - startTime;
        printf("float-precisoin layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, elapsedTime = %.6f", input_x, ULP, relative, elapsedTime.count());
        // !=========================================================================
        cout << endl;
        DL dl_double_start, dl_double_end;
        dl_double_start.d = input_x;
        dl_double_end.i = dl_double_start.i | 0x000000001FFFFFFF;
        vector<double> vec3 = random_test(dl_double_start.d, dl_double_end.d);
        input_x = vec3[0];
        ULP = vec3[1];
        origin_relative = getDoubleOfOrigin(input_x);
        relative = getRelativeError(input_x, origin_relative);
        printf("double-precision layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, BitsError = %.1lf\n", input_x, ULP, relative, log2(ULP + 1));
    }
}

std::pair<double, double> DoubleFunction::processCrossZeroLayer1(const double &start, const double &end) {
    DL dl_first_start, dl_first_end;
    dl_first_start.d = start, dl_first_end.d = end;
    dl_first_start.i = dl_first_start.i & 0xFFFFFC0000000000;
    dl_first_end.i = dl_first_end.i & 0x7FFFFC0000000000;
    // cout << "Detection interval[" << start << ", " << end << "]" << endl;
    double input_x = 0.0, ULP = 0.0, relative = 0.0, origin_relative = 0.0;
    double ULP2 = 0.0, input_x2 = 0.0;
    for (long int i = dl_first_start.i; i <= dl_first_end.i; i -= 0x40000000000) {
        DL dl_input;
        dl_input.i = i;
        double input_one = dl_input.d;
        double origin = getDoubleOfOrigin(input_one);
        double ulp = getULP(input_one, origin);
        if (isinf(ulp) || isnan(ulp)) { continue; }
        if (ULP < ulp) {
            ULP = ulp;
            input_x = input_one;
        }
    }
    for (long int i = 0; i <= dl_first_end.i; i += 0x40000000000) {
        DL dl_input;
        dl_input.i = i;
        double input_one = dl_input.d;
        double origin = getDoubleOfOrigin(input_one);
        double ulp = getULP(input_one, origin);
        if (isinf(ulp) || isnan(ulp)) { continue; }
        if (ULP < ulp) {
            ULP = ulp;
            input_x = input_one;
        }
    }
    return std::make_pair(input_x, ULP);
}

//start和end跨越0的情况
void DoubleFunction::processCrossZeroLayer23(double input_x, double ULP, double start, double end, const std::vector<std::pair<double, double>> &intervals) {

    double origin_relative = getDoubleOfOrigin(input_x);
    double relative = getRelativeError(input_x, origin_relative);
    double input_x2 = 0, ULP2 = 0;
    printf("preprocessing: x = %.6lf, maximumULP = %.2lf, maximumRelative = %e\n", input_x, ULP, relative);
    if (ULP <= 100) {
        cout << "---------------No significant error, excute two-layer search-------------------" << endl;

        DL dl_float_start, dl_float_end;
        dl_float_start.d = start;
        dl_float_start.i = dl_float_start.i & 0xFFFFFFFFE0000000;
        dl_float_end.d = end;
        dl_float_end.i = dl_float_end.i & 0x7FFFFFFFE0000000;
        layer2Flag = true;
        vector<double> vec1 = random_test(dl_float_start.d, dl_float_end.d);
        layer2Flag = false;
        input_x2 = vec1[0];
        ULP2 = vec1[1];
        origin_relative = getDoubleOfOrigin(input_x2);
        relative = getRelativeError(input_x2, origin_relative);
        printf("float-precision layer: x = %.8lf, maximumULP = %.2lf, maximumRelative = %e\n", input_x2, ULP2, relative);
        cout << endl;
        if (input_x2 <= 0) {
            DL dl_double_start, dl_double_end;
            dl_double_start.d = input_x2;
            dl_double_end.d = input_x2;
            dl_double_start.i = dl_double_start.i & 0xFFFFFFFFE0000000;
            dl_double_end.i = dl_double_end.i | 0x000000001FFFFFFF;
            vector<double> vec2 = random_test(dl_double_start.d, dl_double_end.d);
            input_x2 = vec2[0];
            ULP2 = vec2[1];
            origin_relative = getDoubleOfOrigin(input_x2);
            relative = getRelativeError(input_x2, origin_relative);
            printf("double-precision layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, BitsError = %.1lf\n", input_x2, ULP2, relative, log2(ULP2 + 1));
        } else {
            DL dl_double_start, dl_double_end;
            dl_double_start.d = input_x2;
            dl_double_end.d = input_x2;
            dl_double_start.i = dl_double_start.i & 0x7FFFFFFFE0000000;
            dl_double_end.i = dl_double_end.i | 0x000000001FFFFFFF;
            vector<double> vec3 = random_test(dl_double_start.d, dl_double_end.d);
            input_x2 = vec3[0];
            ULP2 = vec3[1];
            origin_relative = getDoubleOfOrigin(input_x2);
            relative = getRelativeError(input_x2, origin_relative);
            printf("double-precision layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, BitsError = %.1lf\n", input_x2, ULP2, relative, log2(ULP2 + 1));
        }
    } else {
        if (input_x2 <= 0) {
            cout << "----------------Existing significant error, excute three-layer search----------------" << endl;
            printf("half-precision layer: x = %.6lf, maximumULP = %.2lf, maximumRelative = %e\n", input_x, ULP, relative);
            cout << endl;
            // !=========================================================================
            auto startTime = std::chrono::steady_clock::now();
            double input_second = input_x;
            DL dl_float_start, dl_float_end;
            dl_float_start.d = input_second;
            dl_float_end.i = dl_float_start.i | 0x000003FFE0000000;
            for (long int i = dl_float_end.i; i <= dl_float_start.i; i += 0x20000000) {
                DL dl_input;
                dl_input.i = i;
                double input_two = dl_input.d;
                double origin = getDoubleOfOrigin(input_two);
                double ulp = getULP(input_two, origin);
                if (isinf(ulp) || isnan(ulp)) { continue; }
                if (ULP < ulp) {
                    ULP = ulp;
                    input_x = input_two;
                }
                layer2Input.emplace_back(dl_input.d);
            }
            origin_relative = getDoubleOfOrigin(input_x);
            relative = getRelativeError(input_x, origin_relative);
            auto finishTime = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsedTime = finishTime - startTime;
            printf("float-precisoin layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, elapsedTime = %.6f", input_x, ULP, relative, elapsedTime.count());
            // !=========================================================================
            cout << endl;
            DL dl_double_start, dl_double_end;
            dl_double_start.d = input_x;
            dl_double_end.i = dl_double_start.i | 0x000000001FFFFFFF;
            vector<double> vec4 = random_test(dl_double_start.d, dl_double_end.d);
            input_x = vec4[0];
            ULP = vec4[1];
            origin_relative = getDoubleOfOrigin(input_x);
            relative = getRelativeError(input_x, origin_relative);
            printf("double-precision layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, BitsError = %.1lf\n", input_x, ULP, relative, log2(ULP + 1));
        } else {    
            cout << "------------Existing significant error, excute three-layer search------------" << endl;
            printf("half-precision layer: x = %.6lf, maximumULP = %.2lf, maximumRelative = %e\n", input_x, ULP, relative);
            cout << endl;
            // !=========================================================================
            auto startTime = std::chrono::steady_clock::now();
            double input_second = input_x;
            DL dl_float_start, dl_float_end;
            dl_float_start.d = input_second;
            dl_float_end.i = dl_float_start.i | 0x000003FFE0000000;
            for (long int i = dl_float_start.i; i <= dl_float_end.i; i += 0x20000000) {
                DL dl_input;
                dl_input.i = i;
                double input_two = dl_input.d;
                double origin = getDoubleOfOrigin(input_two);
                double ulp = getULP(input_two, origin);
                if (isinf(ulp) || isnan(ulp)) { continue; }
                if (ULP < ulp) {
                    ULP = ulp;
                    input_x = input_two;
                }
                layer2Input.emplace_back(dl_input.d);
            }
            origin_relative = getDoubleOfOrigin(input_x);
            relative = getRelativeError(input_x, origin_relative);
            auto finishTime = std::chrono::steady_clock::now();
            std::chrono::duration<double> elapsedTime = finishTime - startTime;
            printf("float-precisoin layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, elapsedTime = %.6f", input_x, ULP, relative, elapsedTime.count());
            // !=========================================================================
            cout << endl;
            DL dl_double_start, dl_double_end;
            dl_double_start.d = input_x;
            dl_double_end.i = dl_double_start.i | 0x000000001FFFFFFF;
            //采样
            vector<double> vec5 = random_test(dl_double_start.d, dl_double_end.d);
            input_x = vec5[0];
            ULP = vec5[1];
            origin_relative = getDoubleOfOrigin(input_x);
            relative = getRelativeError(input_x, origin_relative);
            printf("double-precisoin layer: x = %.16lf, maximumULP = %.2lf, maximumRelative = %e, BitsError = %.1lf\n", input_x, ULP, relative, log2(ULP + 1));
        }   
    }
}

/**
 * @brief 第二种随机采样方法，使用随机种子，每次产生的随机数不一样
 * 
 * @param start 
 * @param end 
 * @return vector<double> 
 */
vector<double> DoubleFunction::random_test(const double &start, const double &end) {
    vector<double> result(2);
    // srand(static_cast<unsigned>(time(nullptr)));//设置随机种子
    // 伪随机数
    double dbNumber = 0.0, origin = 0.0, ULP = 0.0;
    while (isinf(ULP) || isnan(ULP)) {
        dbNumber = start + static_cast<double> (rand()) / (static_cast<double> (RAND_MAX / (end - start)));
        origin = getDoubleOfOrigin(dbNumber);
        ULP = getULP(dbNumber, origin);
    }
    double maximum = ULP;
    double inputx = 0.0;
    double dE, current;
    //循环9w6次，9w6的采样获取一个最大误差和输入传给result容器
    for (int i = 0; i < 100000; ++i) {
        double dbNumber2 = start + static_cast<double> (rand()) / (static_cast<double> (RAND_MAX / (end - start)));
        double origin2 = getDoubleOfOrigin(dbNumber2);
        current = getULP(dbNumber2, origin2);
        if (isinf(current) || isnan(current)) { continue; }
        dE = maximum - current;
        if (dE < 0) {
            maximum = current;
            inputx = dbNumber2;
        }
        if (layer2Flag) {
            layer2Input.emplace_back(dbNumber2);
        } else {
            layer3Input.emplace_back(dbNumber2);
        }
        

    }
    result[0] = inputx;
    result[1] = maximum;
    return result;
}