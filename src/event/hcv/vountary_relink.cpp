////////////////////////////////////////////////////////////////////////////////
// File: vountary_relink.cpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
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
std::unique_ptr<hepce::event::Event>
VoluntaryRelink::Create(datamanagement::ModelData &model_data,
                        const std::string &log_name) {
    return std::make_unique<VoluntaryRelinkImpl>(model_data, log_name);
}

VoluntaryRelinkImpl::VoluntaryRelinkImpl(datamanagement::ModelData &model_data,
                                         const std::string &log_name) {
    SetDiscount(
        utils::GetDoubleFromConfig("cost.discounting_rate", model_data));
    SetCostCategory(model::CostCategory::kScreening);

    _relink_probability = utils::GetDoubleFromConfig(
        "linking.voluntary_relinkage_probability", model_data);

    _voluntary_relink_duration = utils::GetDoubleFromConfig(
        "linking.voluntary_relink_duration", model_data);

    _cost =
        utils::GetDoubleFromConfig("screening_background_rna.cost", model_data);
}

int VoluntaryRelinkImpl::Execute(model::Person &person,
                                 model::Sampler &sampler) {
    // if linked or never linked OR too long since last linked
    if ((person.GetLinkState() == data::LinkageState::UNLINKED) &&
        ((person.GetTimeSinceLinkChange()) < _voluntary_relink_duration) &&
        (sampler.GetDecision({_relink_probability}) == 0) &&
        (person.GetHCV() != data::HCV::NONE)) {
        AddRNATest(person);
        person.Link(data::LinkageType::BACKGROUND);
    }
}

} // namespace hcv
} // namespace event
} // namespace hepce