#ifndef SRC_COMMON_HPP_
#define SRC_COMMON_HPP_

/**
 * @file
 * @brief provides some type independent functionalities, e.g., << overload for different
 * classes,
 * print macros...
 * @example common_test.cpp
 */

#include "includes.hpp"

// print macro
//! @{
#define VA_NUM_ARGS(...) VA_NUM_ARGS_IMPL(__VA_ARGS__, 5, 4, 3, 2, 1, 0)
#define VA_NUM_ARGS_IMPL(_1, _2, _3, _4, _5, N, ...) N
#define macro_dispatcher(func, ...)     macro_dispatcher_(func, VA_NUM_ARGS(__VA_ARGS__))
#define macro_dispatcher_(func, nargs)  macro_dispatcher__(func, nargs)
#define macro_dispatcher__(func, nargs) func ## nargs
/// Quick and neat print function printing in matlab format.
#define prn(...) macro_dispatcher(prnv, __VA_ARGS__)(__VA_ARGS__)
#define prnv1(a)   {std::cout << #a << "=" << (a) << ";" << std::endl;}
#define prnv2(a, b) {std::cout << a << "=" << (b) << ";" << std::endl;}
#define addflag(a) {std::cout << "flags=[flags, " << (a) << "];" << std::endl;}
//! @}

namespace mm {

/// Standard relative error tolerance for testing various geometric inclusions.
static const double EPS = 1e-6;
/// Threshold for what is considers a high conditional number. For conditional numbers
/// that are greater than this, MLS prints a warning
static const double HIGH_COND_NUMBER = 1e15;

/// print function for 2D C Pointer Array
template <class scalar_t>
void printArray(scalar_t arr[], int n, int m = 1) {
    for (int i = 0; i < m; i++) {
        std::cout << std::vector<scalar_t>(arr + i * n, arr + (i + 1) * n) << std::endl;
    }
}
/// cout << overload for PAIR
template <class T, class U>
std::ostream& operator<<(std::ostream& xx, const std::pair<T, U>& par) {
    xx << "(" << par.first << "," << par.second << ")";
    return xx;
}
/// cout << overload for ARRAY with MATLAB like output.
template <class T, int N>
std::ostream& operator<<(std::ostream& xx, const std::array<T, N>& arr) {
    xx << "[";
    for (int i = 0; i < N; ++i) {
        xx << arr[i];
        if (i < N - 1) xx << ",";
    }
    xx << "]";
    return xx;
}
/// cout << overload for VALARRAY with MATLAB like output.
template <class T>
std::ostream& operator<<(std::ostream& xx, const std::valarray<T>& arr) {
    xx << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        xx << arr[i];
        if (i < arr.size() - 1) xx << ",";
    }
    xx << "]";
    return xx;
}
/// cout << overload for VECTOR with MATLAB like output.
template <class T>
std::ostream& operator<<(std::ostream& xx, const std::vector<T>& arr) {
    // do it like the matlab does it.
    xx << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        xx << arr[i];
        if (i < arr.size() - 1) xx << ";";
    }
    xx << "]";  //<< std::endl;
    return xx;
}
/// cout << overload for VECTOR < VECTOR> with MATLAB like output.
template <class T>
std::ostream& operator<<(std::ostream& xx, const std::vector<std::vector<T>>& arr) {
    xx << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        for (size_t j = 0; j < arr[i].size(); ++j) {
            xx << arr[i][j];
            if (j < arr[i].size() - 1) xx << ", ";
        }
        if (i < arr.size() - 1) xx << ";";
    }

    xx << "]";  // << std::endl;
    return xx;
}

/// print formatted for integers -- noop
std::ostream& print_formatted(int x, const std::string&, const std::string&,
                              const std::string&, const std::string&,
                              std::ostream& xx = std::cout);
/// print formatted for doubles -- always fixed
std::ostream& print_formatted(double x, const std::string&, const std::string&,
                              const std::string&, const std::string&,
                              std::ostream& xx = std::cout);
/// print formatted for vectors -- recursive
template <typename T>
std::ostream& print_formatted(const std::vector<T>& v, const std::string& before = "{",
                              const std::string& delimiter = ", ",
                              const std::string& after = "}",
                              const std::string finish = ";", std::ostream& xx = std::cout) {
    bool first = true;
    xx << before;
    for (const T& x : v) {
        if (!first) xx << delimiter;
        first = false;
        print_formatted(x, before, delimiter, after, "", xx);
    }
    return xx << after << finish;
}
/// print formatted for vectors -- recursive
template <typename matrix_t>
std::ostream& print_formatted_matrix(const matrix_t& v, const std::string& before = "{",
                              const std::string& delimiter = ", ",
                              const std::string& after = "}",
                              const std::string finish = ";", std::ostream& xx = std::cout) {
    xx << before;
    int n = v.rows();
    int m = v.cols();
    for (int i = 0; i < n; ++i) {
        if (i > 0) xx << delimiter;
        xx << before;
        for (int j = 0; j < m; ++j) {
            if (j > 0) xx << delimiter;
            xx << v(i, j);
        }
        xx << after;
    }
    return xx << after << finish;
}

/**
 * Prints given text in bold red.
 * @param s text to print.
 */
void print_red(const std::string& s);
/**
 * Prints given text in bold white.
 * @param s text to print.
 */
void print_white(const std::string& s);
/**
 * Prints given text in bold green.
 * @param s text to print.
 */
void print_green(const std::string& s);

/// Signum overload for unsigned types
template <typename T>
inline constexpr int signum(T x, std::false_type) {
    return T(0) < x;
}
/// Signum overload for unsigned types
template <typename T>
inline constexpr int signum(T x, std::true_type) {
    return (T(0) < x) - (x < T(0));
}
/**
 * Signum function -- determines a sign of a number x.
 * @param x A number under inspection.
 * @return 0 if x == 0, -1 if x is negative and +1 if x is positive
 */
template <typename T>
inline constexpr int signum(T x) {
    return signum(x, std::is_signed<T>());
}

/**
 * Return a random seed. The seed is truly random if available, otherwise it is
 * current system time.
 */
unsigned int get_seed();  // NOLINT(*)

/**
 * @brief Our implementation of make_unique for smart pointers
 * @details Intel C Compiler has not yet implemented all c++14 features. One of
 * them includes std::make_unique. Therefore we have created our own
 * (actually we stole it from someone on StackOverflow)
 *
 * @tparam T Type of object that we want to create a pointer for
 * @param args All parameters (including the object we want to point at)
 * @return A unique pointer pointing to the given object
 */
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

/**
 * Simple function to help format memory amounts for printing. Takes in number of bytes
 * and returns a human readable representation.
 */
std::string mem2str(size_t bytes);

/**
 * Returns number of bytes the container uses in memory. The container must support `size()`.
 * This does not count the memory that may be allocated by objects stored in the container.
 * Also STL containers like vector may actually have more memory allocated than their size.
 */
template<typename container_t>
size_t mem_used(const container_t& v) {
    return sizeof(v[0]) * v.size();
}

}  // namespace mm

#endif  // SRC_COMMON_HPP_
