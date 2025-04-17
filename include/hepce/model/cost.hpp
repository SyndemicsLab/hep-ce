////////////////////////////////////////////////////////////////////////////////
// File: cost.hpp                                                             //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_COST_HPP_
#define HEPCE_MODEL_COST_HPP_

#include <ostream>

namespace hepce {
namespace model {
enum class CostCategory : int {
    kMisc = 0,
    kBehavior = 1,
    kScreening = 2,
    kLinking = 3,
    kStaging = 4,
    kLiver = 5,
    kTreatment = 6,
    kBackground = 7,
    kHiv = 8,
    kCount = 9
};

std::ostream &operator<<(std::ostream &os, const CostCategory &inst);

} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_COST_HPP_