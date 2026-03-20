////////////////////////////////////////////////////////////////////////////////
// File: hcv_screening.cpp                                                    //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "internals/hcv_screening_internals.hpp"

#include <hepce/utils/config.hpp>

namespace hepce {
namespace event {

// Factory
// Factory
std::unique_ptr<Event> HCVScreening::Create(const data::Inputs &inputs,
                                            const std::string &log_name) {
    return std::make_unique<HCVScreening>(inputs, log_name);
}

void HCVScreening::LoadData() {
    SetCostCategory(model::CostCategory::kScreening);

    ScreeningData temp;

    // Background RNA
    temp.acute_sensitivity = utils::GetDoubleFromConfig(
        "screening_background_rna.acute_sensitivity", GetInputs());
    temp.chronic_sensitivity = utils::GetDoubleFromConfig(
        "screening_background_rna.chronic_sensitivity", GetInputs());
    temp.specificity = utils::GetDoubleFromConfig(
        "screening_background_rna.specificity", GetInputs());
    temp.cost = utils::GetDoubleFromConfig("screening_background_rna.cost",
                                           GetInputs());
    SetBackgroundRnaData(temp);

    // Background AB
    temp.acute_sensitivity = utils::GetDoubleFromConfig(
        "screening_background_ab.acute_sensitivity", GetInputs());
    temp.chronic_sensitivity = utils::GetDoubleFromConfig(
        "screening_background_ab.chronic_sensitivity", GetInputs());
    temp.specificity = utils::GetDoubleFromConfig(
        "screening_background_ab.specificity", GetInputs());
    temp.cost =
        utils::GetDoubleFromConfig("screening_background_ab.cost", GetInputs());
    SetBackgroundAbData(temp);

    // Intervention RNA
    temp.acute_sensitivity = utils::GetDoubleFromConfig(
        "screening_intervention_rna.acute_sensitivity", GetInputs());
    temp.chronic_sensitivity = utils::GetDoubleFromConfig(
        "screening_intervention_rna.chronic_sensitivity", GetInputs());
    temp.specificity = utils::GetDoubleFromConfig(
        "screening_intervention_rna.specificity", GetInputs());
    temp.cost = utils::GetDoubleFromConfig("screening_intervention_rna.cost",
                                           GetInputs());
    SetInterventionRnaData(temp);

    // Intervention AB
    temp.acute_sensitivity = utils::GetDoubleFromConfig(
        "screening_intervention_ab.acute_sensitivity", GetInputs());
    temp.chronic_sensitivity = utils::GetDoubleFromConfig(
        "screening_intervention_ab.chronic_sensitivity", GetInputs());
    temp.specificity = utils::GetDoubleFromConfig(
        "screening_intervention_ab.specificity", GetInputs());
    temp.cost = utils::GetDoubleFromConfig("screening_intervention_ab.cost",
                                           GetInputs());
    SetInterventionAbData(temp);

    // Other Config Gets
    double t = utils::GetDoubleFromConfig(
        "screening.seropositivity_multiplier_boomer", GetInputs());
    SetSeropositivityBoomerMultiplier((t == 0) ? 1.0 : t);
    SetScreeningPeriod(
        utils::GetIntFromConfig("screening.period", GetInputs()));
    SetInterventionType(
        utils::GetStringFromConfig("screening.intervention_type", GetInputs()));
    LoadScreeningData();
}
} // namespace event
} // namespace hepce