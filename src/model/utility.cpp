////////////////////////////////////////////////////////////////////////////////
// File: utility.cpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-30                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-23                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/model/utility.hpp>

// Local Includes
#include "internals/utility_internals.hpp"

namespace hepce {
namespace model {
std::ostream &operator<<(std::ostream &os, const UtilityCategory &uc) {
    switch (uc) {
    case UtilityCategory::kBehavior:
        os << "kBehavior";
        break;
    case UtilityCategory::kLiver:
        os << "kLiver";
        break;
    case UtilityCategory::kTreatment:
        os << "kTreatment";
        break;
    case UtilityCategory::kBackground:
        os << "kBackground";
        break;
    case UtilityCategory::kHiv:
        os << "kHiv";
        break;
    default:
        os << "kNa";
        break;
    }
    return os;
}
} // namespace model
} // namespace hepce
