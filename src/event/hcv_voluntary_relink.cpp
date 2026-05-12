////////////////////////////////////////////////////////////////////////////////
// File: hcv_voluntary_relink.cpp                                             //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "internals/hcv_voluntary_relink_internals.hpp"

#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>

namespace hepce {
namespace event {

// Factory
std::unique_ptr<Event> VoluntaryRelink::Create(const data::Inputs &inputs,
                                               const std::string &log_name) {
    return std::make_unique<VoluntaryRelink>(inputs, log_name);
}

// Execute
void VoluntaryRelink::Execute(model::Person &person,
                              const model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
    // if linked or never linked OR too long since last linked
    if (Unlinked(person) && RelinkInTime(person) &&
        (person.GetHCVDetails().hcv != data::HCV::kNone) &&
        (sampler.GetDecision({_relink_probability}) == 0)) {
        person.Screen(data::InfectionType::kHcv, data::ScreeningTest::kRna,
                      data::ScreeningType::kBackground);
        AddEventCost(person, _cost);
        person.Link(data::InfectionType::kHcv);
    }
}

void VoluntaryRelink::LoadData() {
    SetCostCategory(model::CostCategory::kScreening);

    _relink_probability = utils::GetDoubleFromConfig(
        "linking.voluntary_relinkage_probability", GetInputs());

    _voluntary_relink_duration = utils::GetDoubleFromConfig(
        "linking.voluntary_relink_duration", GetInputs());

    _cost = utils::GetDoubleFromConfig("screening_background_rna.cost",
                                       GetInputs());
}

} // namespace event
} // namespace hepce
