////////////////////////////////////////////////////////////////////////////////
// File: screening.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
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
Screening::Create(datamanagement::ModelData &model_data,
                  const std::string &log_name) {
    return std::make_unique<ScreeningImpl>(model_data, log_name);
}

ScreeningImpl::ScreeningImpl(datamanagement::ModelData &model_data,
                             const std::string &log_name)
    : ScreeningBase(model_data, log_name) {
    SetDiscount(
        utils::GetDoubleFromConfig("cost.discounting_rate", model_data));
    SetCostCategory(model::CostCategory::kScreening);

    ScreeningData temp;

    // Background RNA
    temp.acute_sensitivity = utils::GetDoubleFromConfig(
        "screening_background_rna.acute_sensitivity", model_data);
    temp.chronic_sensitivity = utils::GetDoubleFromConfig(
        "screening_background_rna.chronic_sensitivity", model_data);
    temp.specificity = utils::GetDoubleFromConfig(
        "screening_background_rna.specificity", model_data);
    temp.cost =
        utils::GetDoubleFromConfig("screening_background_rna.cost", model_data);
    SetBackgroundRnaData(temp);

    // Background AB
    temp.acute_sensitivity = utils::GetDoubleFromConfig(
        "screening_background_ab.acute_sensitivity", model_data);
    temp.chronic_sensitivity = utils::GetDoubleFromConfig(
        "screening_background_ab.chronic_sensitivity", model_data);
    temp.specificity = utils::GetDoubleFromConfig(
        "screening_background_ab.specificity", model_data);
    temp.cost =
        utils::GetDoubleFromConfig("screening_background_ab.cost", model_data);
    SetBackgroundAbData(temp);

    // Intervention RNA
    temp.acute_sensitivity = utils::GetDoubleFromConfig(
        "screening_intervention_rna.acute_sensitivity", model_data);
    temp.chronic_sensitivity = utils::GetDoubleFromConfig(
        "screening_intervention_rna.chronic_sensitivity", model_data);
    temp.specificity = utils::GetDoubleFromConfig(
        "screening_intervention_rna.specificity", model_data);
    temp.cost = utils::GetDoubleFromConfig("screening_intervention_rna.cost",
                                           model_data);
    SetInterventionRnaData(temp);

    // Intervention AB
    temp.acute_sensitivity = utils::GetDoubleFromConfig(
        "screening_intervention_ab.acute_sensitivity", model_data);
    temp.chronic_sensitivity = utils::GetDoubleFromConfig(
        "screening_intervention_ab.chronic_sensitivity", model_data);
    temp.specificity = utils::GetDoubleFromConfig(
        "screening_intervention_ab.specificity", model_data);
    temp.cost = utils::GetDoubleFromConfig("screening_intervention_ab.cost",
                                           model_data);
    SetInterventionAbData(temp);

    // Other Config Gets
    double t = utils::GetDoubleFromConfig(
        "screening.seropositivity_multiplier_boomer", model_data);
    SetSeropositivityBoomerMultiplier((t == 0) ? 1.0 : t);
    SetScreeningPeriod(utils::GetIntFromConfig("screening.period", model_data));
    SetInterventionType(
        utils::GetStringFromConfig("screening.intervention_type", model_data));
}
} // namespace hcv
} // namespace event
} // namespace hepce