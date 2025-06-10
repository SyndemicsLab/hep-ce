////////////////////////////////////////////////////////////////////////////////
// File: clearance.cpp                                                        //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-10                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/hcv/clearance.hpp>

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>

// Local Includes
#include "internals/clearance_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {

// Factory
std::unique_ptr<hepce::event::Event>
Clearance::Create(datamanagement::ModelData &model_data,
                  const std::string &log_name) {
    return std::make_unique<ClearanceImpl>(model_data, log_name);
}

// Constructor
ClearanceImpl::ClearanceImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name)
    : EventBase(model_data, log_name) {
    LoadData(model_data);
}

// Execute
void ClearanceImpl::Execute(model::Person &person, model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    // if person isn't infected or is chronic, nothing to do
    // Also skip if person is already on treatment since we want this to
    // count as SVR
    if (person.GetHCVDetails().hcv != data::HCV::kAcute ||
        person.GetTreatmentDetails(data::InfectionType::kHcv)
            .initiated_treatment) {
        return;
    }
    if (sampler.GetDecision({_probability, 1 - _probability}) == 0) {
        person.ClearHCV(true);
    }
}

void ClearanceImpl::LoadData(datamanagement::ModelData &model_data) {
    _probability =
        utils::GetDoubleFromConfig("infection.clearance_prob", model_data);

    if (_probability == 0) {
        hepce::utils::LogInfo(
            GetLogName(), "Infection Clearance Probability is not found or "
                          "0, setting to default value of 25\% over 6 months");
        _probability =
            utils::RateToProbability(utils::ProbabilityToRate(0.25, 6));
    }
}
} // namespace hcv
} // namespace event
} // namespace hepce