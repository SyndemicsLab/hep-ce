////////////////////////////////////////////////////////////////////////////////
// File: hiv_screening.cpp                                                    //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "internals/hiv_screening_internals.hpp"

#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
// Factory
std::unique_ptr<Event> HIVScreening::Create(const data::Inputs &inputs,
                                            const std::string &log_name) {
    return std::make_unique<HIVScreening>(inputs, log_name);
}

void HIVScreening::LoadData() {
    SetInterventionType(utils::GetStringFromConfig(
        "hiv_screening.intervention_type", GetInputs()));
    SetScreeningPeriod(
        utils::GetIntFromConfig("hiv_screening.period", GetInputs()));
    LoadScreeningData();
}
} // namespace event
} // namespace hepce