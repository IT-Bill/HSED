#include <getresult.hpp>

extern int idx;

double getULP(double x, double origin) {
	mpfr_t mpfr_origin, mpfr_oracle, mp1;
	mpfr_inits2(128, mpfr_origin, mpfr_oracle, mp1, (mpfr_ptr) 0);
	mpfr_set_d(mp1, x, MPFR_RNDN);

	mpfr_set(mpfr_oracle, mp1, MPFR_RNDN);
	mpfr_set_d(mpfr_origin, origin, MPFR_RNDN);
	double ulp = computeULPDiff(mpfr_origin, mpfr_oracle);
	mpfr_clears(mpfr_origin, mpfr_oracle, mp1, (mpfr_ptr) 0);
	mpfr_free_cache();
	return ulp;
}

double getRelativeError(double x, double origin) {
	mpfr_t mpfr_origin, mpfr_oracle, mpfr_relative, mpfr_absolute, mp1;
	mpfr_inits2(128, mpfr_origin, mpfr_oracle, mpfr_relative, mpfr_absolute, mp1, (mpfr_ptr) 0);
	mpfr_set_d(mp1, x, MPFR_RNDN);

	mpfr_set(mpfr_oracle, mp1, MPFR_RNDN);
	mpfr_set_d(mpfr_origin, origin, MPFR_RNDN);
	mpfr_sub(mpfr_absolute, mpfr_oracle, mpfr_origin, MPFR_RNDN);
	mpfr_div(mpfr_relative, mpfr_absolute, mpfr_oracle, MPFR_RNDN);
	double relative = mpfr_get_d(mpfr_relative, MPFR_RNDN);
	relative = abs(relative);
	mpfr_clears(mpfr_origin, mpfr_oracle, mpfr_relative, mpfr_absolute, mp1, (mpfr_ptr) 0);
	mpfr_free_cache();
	return relative;
}
double getFloatToDoubleOfOrigin(const float &inputx) {
	double x = (double)inputx;
	gsl_sf_result gslres;
	GSLFuncList[idx](x, &gslres);
	return gslres.val;
}
double getDoubleOfOrigin(const double &inputx) {
	double x = inputx;
	gsl_sf_result gslres;
	GSLFuncList[idx](x, &gslres);
	return gslres.val;
}
double getFloatToDoubleOfOrigin2(const float &inputx) {
	double x = (double)inputx;
	gsl_sf_result gslres;
	GSLFuncList[idx](x, &gslres);
	return gslres.val;
}
double getDoubleOfOrigin2(const double &inputx) {
	double x = inputx;
	gsl_sf_result gslres;
	GSLFuncList[idx](x, &gslres);
	return gslres.val;
}
