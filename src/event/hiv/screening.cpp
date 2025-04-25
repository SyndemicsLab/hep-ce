////////////////////////////////////////////////////////////////////////////////
// File: screening.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#include <hepce/event/hiv/screening.hpp>

#include "internals/screening_internals.hpp"
#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
namespace hiv {
ScreeningImpl::ScreeningImpl(
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    const std::string &log_name)
    : ScreeningBase(dm, log_name) {
    SetInterventionType(
        utils::GetStringFromConfig("hiv_screening.intervention_type", dm));
    SetScreeningPeriod(utils::GetIntFromConfig("hiv_screening.period", dm));
}
} // namespace hiv
} // namespace event
} // namespace hepce