////////////////////////////////////////////////////////////////////////////////
// File: math.hpp                                                             //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-08-05                                                  //
// Modified By: Andrew Clark                                                  //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_MATH_HPP_
#define HEPCE_UTILS_MATH_HPP_

#include <chrono>
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

/// @brief Retrieves the current time since epoch in milliseconds as an integer.
/// @return The current time in milliseconds
inline int GetCurrentTimeInMilliseconds() {
    const std::chrono::time_point<std::chrono::steady_clock> current_time =
        std::chrono::steady_clock::now();
    return static_cast<int>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            current_time.time_since_epoch())
            .count());
}

/// @brief Sigmoidal Decay Function
/// @param timestep The timestep to adjust for
/// @param decay_start The timestep at which decay is steepest
/// @param scaling_coeff Determines the steepness of the decay
/// @return Decayed value
inline double SigmoidalDecay(double value, int time, int cutoff,
                             double scaling_coeff) {
    return value / (1 + exp(scaling_coeff * (time - cutoff)));
}
} // namespace utils
} // namespace hepce

#endif // HEPCE_UTILS_MATH_HPP_
