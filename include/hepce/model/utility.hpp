////////////////////////////////////////////////////////////////////////////////
// File: utility.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_UTILITY_HPP_
#define HEPCE_MODEL_UTILITY_HPP_

#include <ostream>

namespace hepce {
namespace model {
enum class UtilityCategory : int {
    kBehavior = 0,
    kLiver = 1,
    kTreatment = 2,
    kBackground = 3,
    kHiv = 4,
    kCount = 5
};
std::ostream &operator<<(std::ostream &os, const UtilityCategory &uc);
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_UTILITY_HPP_