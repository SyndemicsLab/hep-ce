////////////////////////////////////////////////////////////////////////////////
// File: screening.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
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
    const std::string &log_name)
    : ScreeningBase(dm, log_name) {
    SetDiscount(utils::GetDoubleFromConfig("cost.discounting_rate", dm));
    SetCostCategory(model::CostCategory::kScreening);

    std::string data;

    ScreeningData temp;

    // Background RNA
    dm->GetFromConfig("screening_background_rna.acute_sensitivity", data);
    temp.acute_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_rna.chronic_sensitivity", data);
    temp.chronic_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_rna.specificity", data);
    temp.specificity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_rna.cost", data);
    temp.cost = utils::SToDPositive(data);
    SetBackgroundRnaData(temp);

    // Background AB
    dm->GetFromConfig("screening_background_ab.acute_sensitivity", data);
    temp.acute_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_ab.chronic_sensitivity", data);
    temp.chronic_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_ab.specificity", data);
    temp.specificity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_background_ab.cost", data);
    temp.cost = utils::SToDPositive(data);
    SetBackgroundAbData(temp);

    // Intervention RNA
    dm->GetFromConfig("screening_intervention_rna.acute_sensitivity", data);
    temp.acute_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_rna.chronic_sensitivity", data);
    temp.chronic_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_rna.specificity", data);
    temp.specificity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_rna.cost", data);
    temp.cost = utils::SToDPositive(data);
    SetInterventionRnaData(temp);

    // Intervention AB
    dm->GetFromConfig("screening_intervention_ab.acute_sensitivity", data);
    temp.acute_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_ab.chronic_sensitivity", data);
    temp.chronic_sensitivity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_ab.specificity", data);
    temp.specificity = utils::SToDPositive(data);
    dm->GetFromConfig("screening_intervention_ab.cost", data);
    temp.cost = utils::SToDPositive(data);
    SetInterventionAbData(temp);

    // Other Config Gets
    dm->GetFromConfig("screening.seropositivity_multiplier_boomer", data);

    SetSeropositivityBoomerMultiplier(
        (data.empty()) ? 1.0 : utils::SToDPositive(data));
    dm->GetFromConfig("screening.period", data);
    SetScreeningPeriod((data.empty()) ? 0 : std::stoi(data));
    dm->GetFromConfig("screening.intervention_type", data);
    SetInterventionType(data);
}
} // namespace hcv
} // namespace event
} // namespace hepce