//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Utilities Helper Functions.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef UTILS_UTILS_HPP_
#define UTILS_UTILS_HPP_

#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>

/// @brief Namespace containing Utility Helper Functions
namespace Utils {
    /// @brief Convert Probability to Rate
    /// @param prob Probability to Convert
    /// @return Converted Rate
    double probabilityToRate(double prob);

    /// @brief Convert Rate to Probability
    /// @param rate Rate to Convert
    /// @return Converted Probability
    double rateToProbability(double rate);

    /// @brief Calculate Discount to Provide
    /// @param valueToDiscount Value that the Discount will be applied to
    /// @param discountRate The Discount Rate
    /// @param timestep The Timestep to Discount During
    /// @return
    double discount(double valueToDiscount, double discountRate,
                    double timestep);

    /// @brief
    /// @param string
    /// @return
    std::string toLower(std::string string);

    /// @brief
    /// @param string
    /// @return
    bool stobool(std::string string);
} // namespace Utils

#endif
