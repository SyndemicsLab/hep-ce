////////////////////////////////////////////////////////////////////////////////
// File: Treatment.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-16                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_TREATMENT_HPP_
#define EVENT_TREATMENT_HPP_

#include "Cost.hpp"
#include "Event.hpp"
#include "Person.hpp"
#include "Utility.hpp"
#include <unordered_map>

namespace event {
class TreatmentIMPL {
protected:
    // event-specific variables
    utility::UtilityCategory UTIL_CATEGORY =
        utility::UtilityCategory::kTreatment;
    cost::CostCategory COST_CATEGORY = cost::CostCategory::kTreatment;
    // default infection type is HCV
    person::InfectionType INF_TYPE = person:: ::kHcv;

    // user-provided values
    bool allow_retreatment = true;
    double discount = 0.0;
    double lost_to_follow_up_probability;
    // costs of visit for treatment
    double treatment_cost;
    // cost in the case of a toxicity event during treatment
    double toxicity_cost;
    // utility in the case of a toxicity event during treatment
    double toxicity_utility;
    double treatment_init_probability;
    // including all ineligibility variables because it's unclear if any would
    // make a person ineligible for HIV treatment - erring on the side of
    // caution
    std::vector<std::string> ineligible_behaviors = {};
    std::vector<std::string> ineligible_fibrosis = {};
    std::vector<std::string> ineligible_pregnancy = {};
    int ineligible_time_since_linked = -2;
    int ineligible_time_since_last_use = -2;

    /// @brief
    /// @param
    /// @return
    bool isEligibleFibrosisStage(person::FibrosisState fibrosis_state) const;
    /// @brief
    /// @param
    /// @return
    bool isEligibleBehavior(person::Behavior behavior) const;
    /// @brief
    /// @param
    /// @return
    bool isEligiblePregnancy(person::PregnancyState pregnancy_state) const;
    /// @brief
    /// @param
    void ResetUtility(std::shared_ptr<person::PersonBase> person);
    /// @brief
    /// @param
    void QuitEngagement(std::shared_ptr<person::PersonBase> person);
    /// @brief
    /// @param
    /// @return
    bool LostToFollowUp(std::shared_ptr<person::PersonBase> person,
                        std::shared_ptr<stats::DeciderBase> decider);
    /// @brief
    /// @param
    void ChargeCost(std::shared_ptr<person::PersonBase> person, double cost);
    /// @brief
    /// @param
    /// @return
    std::vector<std::string>
    LoadEligibilityVectors(std::string config_key,
                           datamanagement::ModelData &model_data);
    /// @brief
    /// @param
    /// @return
    bool IsEligible(std::shared_ptr<person::PersonBase> person) const;

public:
    TreatmentIMPL(datamanagement::ModelData &model_data);
};
} // namespace event
#endif
