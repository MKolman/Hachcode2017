#ifndef SRC_INCLUDES_HPP_
#define SRC_INCLUDES_HPP_

/**
 * @file
 * @brief An include file with all libraries that will not change often
 * and will be precompiled.
 */

/// @cond
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <valarray>
#include <vector>
#include <omp.h>


/*
 * This is a trick to disable all calls for this functions without std.
 * If called simply as abs(x), one of the two things can happen:
 * - either the declaration below is the only available one, and you will get a linker error
 * - or math.h or another library also has a declaration, in which case you will get an error saying
 *   error: call of overloaded ‘abs(int)’ is ambiguous. You should fix this by using the
 *   std:: versions of functions, like std::abs, which is overloaded for doubles as well.
 */

namespace never_use {

double acos(double x);
double asin(double x);
double atan(double x);
double atan2(double y, double x);
double ceil(double x);
double cos(double x);
double cosh(double x);
double exp(double x);
double fabs(double x);
double floor(double x);
double fmod(double x, double m);
double frexp(double x, int* n);
double ldexp(double x, int n);
double log(double x);
double log10(double x);
double modf(double x, double* y);
double pow(double x, double y);
double sin(double x);
double sinh(double x);
double sqrt(double x);
double tan(double x);
double tanh(double x);
double erf(double x);
double erfc(double x);
double gamma(double x);
double hypot(double x, double y);
double j0(double x);
double j1(double x);
double jn(int, double x);
double lgamma(double x);
double y0(double x);
double y1(double x);
double yn(int, double x);
int    isnan(double x);
double acosh(double x);
double asinh(double x);
double atanh(double x);
double cbrt(double x);
double expm1(double x);
int    ilogb(double x);
double log1p(double x);
double logb(double x);
double nextafter(double x, double y);
double remainder(double x, double y);
double rint(double x);
double scalb(double x, double y);
int    abs(int n);

}  // namespace never_use
using namespace never_use;  // NOLINT(*)

/// @endcond

#endif  // SRC_INCLUDES_HPP_
