/**
 * @file
 * @brief Implementation of non templated functions defined in the header common.hpp.
 */

#include "common.hpp"


/**
 * A namespace wrapping everything in this library. Name mm stands for Meshless Machine.
 */
namespace mm {

std::ostream& print_formatted(int x, const std::string&, const std::string&,
                              const std::string&, const std::string&, std::ostream& xx) {
    return xx << x;
}
std::ostream& print_formatted(double x, const std::string&, const std::string&,
                              const std::string&, const std::string&, std::ostream& xx) {
    return xx << std::fixed << std::setprecision(16) << x;
}

void print_red(const std::string& s) { std::cout << "\x1b[31;1m" << s << "\x1b[37;0m"; }

void print_white(const std::string& s) { std::cout << "\x1b[37;1m" << s << "\x1b[37;0m"; }

void print_green(const std::string& s) { std::cout << "\x1b[32;1m" << s << "\x1b[37;0m"; }

unsigned int get_seed() {
    try {
        std::random_device rd;
        return rd();
    } catch (std::exception& e) {
        return std::chrono::system_clock::now().time_since_epoch().count();
    }
}

std::string mem2str(size_t bytes) {
    double amount = bytes;
    std::vector<std::string> suffix = {"B", "kB", "MB", "GB"};
    for (int i = 0; i < 4; ++i) {
        if (amount < 100) {
            std::stringstream ss;
            ss << static_cast<int>(amount*10) / 10.0 << " " << suffix[i];
            return ss.str();
        }
        amount /= 1000;
    }
    return "More than your mom.";
}

}  // namespace mm
