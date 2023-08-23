#ifndef UTILS_UTILS_HPP_
#define UTILS_UTILS_HPP_

#include <cmath>
#include <stdexcept>

namespace Utils {
    double probabilityToRate(double prob);

    double rateToProbability(double rate);

    double discount(double valueToDiscount, double discountRate,
                    double timestep);
} // namespace Utils

#endif