#include "getresult.hpp"
#include <sstream>
#include <iostream>
#include <cstdio>
#include <memory>

extern int funcIdx;

double getULP(double x, double origin) {
    // 使用 popen 运行 Python 脚本并读取其输出
	std::stringstream cmd;
	cmd << "python3 script/gsl_mpmath.py " << funcIdx << " " << x << " " << origin;
    std::shared_ptr<FILE> pipe(popen(cmd.str().c_str(), "r"), pclose);
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
    double ulp_error = std::stod(result);
	return ulp_error;
}
double getRelativeError(double x, double origin) {
	return -1;
}
double getFloatToDoubleOfOrigin(const float &inputx) {
	double x = (double)inputx;
	gsl_sf_result gslres;
	GSLFuncList[funcIdx](x, &gslres);
	return gslres.val;
}
double getDoubleOfOrigin(const double &inputx) {
	double x = inputx;
	gsl_sf_result gslres;
	GSLFuncList[funcIdx](x, &gslres);
	return gslres.val;
}
double getFloatToDoubleOfOrigin2(const float &inputx) {
	double x = (double)inputx;
	return x;
}
double getDoubleOfOrigin2(const double &inputx) {
	double x = inputx;
	return x;
}
