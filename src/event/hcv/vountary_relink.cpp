////////////////////////////////////////////////////////////////////////////////
// File: vountary_relink.cpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
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
VoluntaryRelink::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
                        const std::string &log_name) {
    return std::make_unique<VoluntaryRelinkImpl>(dm, log_name);
}

VoluntaryRelinkImpl::VoluntaryRelinkImpl(
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    const std::string &log_name) {
    SetDiscount(utils::GetDoubleFromConfig("cost.discounting_rate", dm));
    SetCostCategory(model::CostCategory::kScreening);

    std::string data;
    dm->GetFromConfig("linking.voluntary_relinkage_probability", data);
    if (data.empty()) {
        // Warn Empty
        data = "0.0";
    }
    _relink_probability = utils::SToDPositive(data);

    dm->GetFromConfig("linking.voluntary_relink_duration", data);
    if (data.empty()) {
        // Warn Empty
        data = "0.0";
    }
    _voluntary_relink_duration = std::stoi(data);

    dm->GetFromConfig("screening_background_rna.cost", data);
    _cost = utils::SToDPositive(data);
}

int VoluntaryRelinkImpl::Execute(
    model::Person &person, std::shared_ptr<datamanagement::DataManagerBase> dm,
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