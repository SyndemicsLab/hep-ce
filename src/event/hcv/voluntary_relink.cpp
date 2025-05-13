////////////////////////////////////////////////////////////////////////////////
// File: voluntary_relink.cpp                                                 //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/hcv/voluntary_relink.hpp>

#include "internals/voluntary_relink_internals.hpp"
#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>

namespace hepce {
namespace event {
namespace hcv {
// Factory
std::unique_ptr<hepce::event::Event>
VoluntaryRelink::Create(datamanagement::ModelData &model_data,
                        const std::string &log_name) {
    return std::make_unique<VoluntaryRelinkImpl>(model_data, log_name);
}

// Constructor
VoluntaryRelinkImpl::VoluntaryRelinkImpl(datamanagement::ModelData &model_data,
                                         const std::string &log_name)
    : EventBase(model_data, log_name) {
    LoadData(model_data);
}

// Execute
void VoluntaryRelinkImpl::Execute(model::Person &person,
                                  model::Sampler &sampler) {
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

void VoluntaryRelinkImpl::LoadData(datamanagement::ModelData &model_data) {
    SetEventCostCategory(model::CostCategory::kScreening);

    _relink_probability = utils::GetDoubleFromConfig(
        "linking.voluntary_relinkage_probability", model_data);

    _voluntary_relink_duration = utils::GetDoubleFromConfig(
        "linking.voluntary_relink_duration", model_data);

    _cost =
        utils::GetDoubleFromConfig("screening_background_rna.cost", model_data);
}

} // namespace hcv
} // namespace event
} // namespace hepce