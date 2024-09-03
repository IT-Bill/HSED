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
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
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
	mpfr_t mpfr_origin, mpfr_oracle, mpfr_relative, mpfr_absolute, mp1, mp2, mp3;
	mpfr_inits2(128, mpfr_origin, mpfr_oracle, mpfr_relative, mpfr_absolute, mp1, mp2, mp3, (mpfr_ptr) 0);
	mpfr_set_d(mp1, x, MPFR_RNDN);
	mpfr_set_d(mp2, 1, MPFR_RNDN);
	mpfr_add(mp3, mp1, mp2, MPFR_RNDN);

	mpfr_set(mpfr_oracle, mp3, MPFR_RNDN);
	mpfr_set_d(mpfr_origin, origin, MPFR_RNDN);
	mpfr_sub(mpfr_absolute, mpfr_oracle, mpfr_origin, MPFR_RNDN);
	mpfr_div(mpfr_relative, mpfr_absolute, mpfr_oracle, MPFR_RNDN);
	double relative = mpfr_get_d(mpfr_relative, MPFR_RNDN);
	relative = abs(relative);
	mpfr_clears(mpfr_origin, mpfr_oracle, mpfr_relative, mpfr_absolute, mp1, mp2, mp3, (mpfr_ptr) 0);
	mpfr_free_cache();
	return relative;
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
