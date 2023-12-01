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
/// This file contains the implementation of the Utility Helper Functions.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Utils.hpp"

namespace Utils {

    double probabilityToRate(double probability) {
        if (probability < 0 || probability >= 1) {
            throw std::domain_error("Out of probability value range");
        }
        return -log(1 - probability);
    }

    double rateToProbability(double rate) {
        if (rate < 0) {
            throw std::domain_error("Out of rate value range");
        }
        return 1 - exp(-rate);
    }

    double discount(double valueToDiscount, double discountRate,
                    double timestep) {
        if (discountRate < 0 || timestep < 0) {
            throw std::domain_error("Out of DiscountRate or Timestep Range");
        }
        return valueToDiscount / pow(1 + discountRate, timestep);
    }

    std::string toLower(std::string string) {
        std::string temp = string;
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        return temp;
    }

    bool stobool(std::string string) {
        std::string temp = toLower(string);
        std::istringstream is(temp);
        bool b;
        is >> std::boolalpha >> b;
        return b;
    }
} // namespace Utils