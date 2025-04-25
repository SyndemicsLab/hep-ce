////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
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
    LinkingImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                const std::string &log_name = "console") {
        SetCostCategory(model::CostCategory::kHiv);
        SetLinkingStratifiedByPregnancy(CheckForPregnancyEvent(dm));
        LoadLinkingData(dm);

        SetInterventionCost(
            utils::GetDoubleFromConfig("hiv_linking.intervention_cost", dm));
        SetFalsePositiveCost(utils::GetDoubleFromConfig(
            "hiv_linking.false_positive_test_cost", dm));
        SetRecentScreenMultiplier(utils::GetDoubleFromConfig(
            "hiv_linking.recent_screen_multiplier", dm));
        SetRecentScreenCutoff(
            utils::GetIntFromConfig("hiv_linking.recent_screen_cutoff", dm));
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