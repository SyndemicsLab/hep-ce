////////////////////////////////////////////////////////////////////////////////
// File: Utils.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Utils.hpp"
#include <algorithm>

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

double discount(double valueToDiscount, double discountRate, double timestep) {
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
