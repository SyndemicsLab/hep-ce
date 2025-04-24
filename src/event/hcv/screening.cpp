////////////////////////////////////////////////////////////////////////////////
// File: screening.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/hcv/screening.hpp>

#include "internals/screening_internals.hpp"
#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {
namespace hcv {
std::unique_ptr<hepce::event::Event>
Screening::Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name) {
    return std::make_unique<ScreeningImpl>(dm, log_name);
}

ScreeningImpl::ScreeningImpl(
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    const std::string &log_name) {
    SetDiscount(utils::GetDoubleFromConfig("cost.discounting_rate", dm));
    SetCostCategory(model::CostCategory::kScreening);

    std::string data;

    // Background RNA
    dm->GetFromConfig("screening_background_rna.acute_sensitivity", data);
    _background_rna_data.acute_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_rna.chronic_sensitivity", data);
    _background_rna_data.chronic_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_rna.specificity", data);
    _background_rna_data.specificity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_rna.cost", data);
    _background_rna_data.cost = utils::SToDPositive(data);

    // Background AB
    dm->GetFromConfig("screening_background_ab.acute_sensitivity", data);
    _background_ab_data.acute_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_ab.chronic_sensitivity", data);
    _background_ab_data.chronic_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_ab.specificity", data);
    _background_ab_data.specificity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_ab.cost", data);
    _background_ab_data.cost = utils::SToDPositive(data);

    // Intervention RNA
    dm->GetFromConfig("screening_intervention_rna.acute_sensitivity", data);
    _intervention_rna_data.acute_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_rna.chronic_sensitivity", data);
    _intervention_rna_data.chronic_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_rna.specificity", data);
    _intervention_rna_data.specificity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_rna.cost", data);
    _intervention_rna_data.cost = utils::SToDPositive(data);

    // Intervention AB
    dm->GetFromConfig("screening_intervention_ab.acute_sensitivity", data);
    _intervention_ab_data.acute_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_ab.chronic_sensitivity", data);
    _intervention_ab_data.chronic_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_ab.specificity", data);
    _intervention_ab_data.specificity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_ab.cost", data);
    _intervention_ab_data.cost = utils::SToDPositive(data);

    // Other Config Gets
    dm->GetFromConfig("screening.seropositivity_multiplier_boomer", data);
    _seropositivity_boomer_multiplier =
        (data.empty()) ? 1.0 : utils::SToDPositive(data);
    dm->GetFromConfig("screening.period", data);
    _screening_period = (data.empty()) ? 0 : std::stoi(data);
    dm->GetFromConfig("screening.intervention_type", _intervention_type);

    std::string error;
    int rc = dm->SelectCustomCallback(ScreenSQL(), CallbackScreening,
                                      &_probability, error);
    if (rc != 0) {
        // Log Error
    }

    if (_probability.empty()) {
        // Warn Empty
    }
}

int ScreeningImpl::Execute(model::Person &person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           model::Sampler &sampler) {
    // if a person is already linked, skip screening
    if (person.GetLinkState() == data::LinkageState::LINKED) {
        return;
    }

    bool do_one_time_screen = (_intervention_type == "one-time") &&
                              (person.GetCurrentTimestep() == 1);

    bool do_periodic_screen =
        (_intervention_type == "periodic") &&
        (person.GetTimeSinceLastScreening() >= _screening_period);

    // If it is time to do a one-time intervention screen or periodic
    // screen, run an intervention screen
    if (do_one_time_screen || do_periodic_screen) {
        this->InterventionScreen(person, dm, sampler);
    } else {
        this->BackgroundScreen(person, dm, sampler);
    }
}
} // namespace hcv
} // namespace event
} // namespace hepce