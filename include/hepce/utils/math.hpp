////////////////////////////////////////////////////////////////////////////////
// File: math.hpp                                                             //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_MATH_HPP_
#define HEPCE_UTILS_MATH_HPP_

#include <cmath>
#include <stdexcept>
namespace hepce {
namespace utils {
/// @brief Convert Probability to Rate
/// @param prob Probability to Convert
/// @return Converted Rate
inline double ProbabilityToRate(double probability, int time = 1) {
    if (probability < 0 || probability >= 1) {
        throw std::domain_error("Out of probability value range");
    }
    return -log(1 - probability) / time;
}

/// @brief Convert Rate to Probability
/// @param rate Rate to Convert
/// @return Converted Probability
inline double RateToProbability(double rate, int time = 1) {
    if (rate < 0) {
        throw std::domain_error("Out of rate value range");
    }
    return 1 - exp(-rate * time);
}

/// @brief Calculate Discount to Provide
/// @param value Value that the Discount will be applied to
/// @param discount_rate The Discount Rate
/// @param timestep The Timestep to Discount During
/// @return
inline double Discount(double value, double discount_rate, double timestep,
                       bool annual = false) {
    if (discount_rate < 0 || timestep < 0) {
        throw std::domain_error("Out of discount rate or Timestep Range");
    }
    discount_rate = annual ? discount_rate / 12 : discount_rate;
    double denominator = std::pow(1 + discount_rate, timestep);
    return value / denominator;
}
} // namespace utils
} // namespace hepce

#endif // HEPCE_UTILS_MATH_HPP_