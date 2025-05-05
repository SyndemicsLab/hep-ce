////////////////////////////////////////////////////////////////////////////////
// File: screening.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-05                                                  //
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
// Factory
std::unique_ptr<hepce::event::Event>
Screening::Create(datamanagement::ModelData &model_data,
                  const std::string &log_name) {
    return std::make_unique<ScreeningImpl>(model_data, log_name);
}

ScreeningImpl::ScreeningImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name)
    : ScreeningBase(model_data, log_name) {
    LoadData(model_data);
}

void ScreeningImpl::LoadData(datamanagement::ModelData &model_data) {
    SetInterventionType(utils::GetStringFromConfig(
        "hiv_screening.intervention_type", model_data));
    SetScreeningPeriod(
        utils::GetIntFromConfig("hiv_screening.period", model_data));
}
} // namespace hiv
} // namespace event
} // namespace hepce