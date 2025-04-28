////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_LINKINGINTERNALS_HPP_
#define HEPCE_EVENT_HIV_LINKINGINTERNALS_HPP_

#include <hepce/event/hiv/linking.hpp>

#include "internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
class LinkingImpl : public virtual hiv::Linking, public event::LinkingBase {
public:
    LinkingImpl(datamanagement::ModelData &model_data,
                const std::string &log_name = "console") {
        SetCostCategory(model::CostCategory::kHiv);
        SetLinkingStratifiedByPregnancy(CheckForPregnancyEvent(model_data));
        LoadLinkingData(model_data);

        SetInterventionCost(utils::GetDoubleFromConfig(
            "hiv_linking.intervention_cost", model_data));
        SetFalsePositiveCost(utils::GetDoubleFromConfig(
            "hiv_linking.false_positive_test_cost", model_data));
        SetRecentScreenMultiplier(utils::GetDoubleFromConfig(
            "hiv_linking.recent_screen_multiplier", model_data));
        SetRecentScreenCutoff(utils::GetIntFromConfig(
            "hiv_linking.recent_screen_cutoff", model_data));
    }

    ~LinkingImpl() = default;

private:
    inline bool FalsePositive(model::Person &person) override {
        if (person.GetHIV() != data::HIV::NONE) {
            return false;
        }
        person.ClearDiagnosis(data::InfectionType::HCV);
        AddFalsePositiveCost(person, GetCostCategory());
        return true;
    }
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif