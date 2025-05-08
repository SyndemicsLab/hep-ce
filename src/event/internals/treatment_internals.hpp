////////////////////////////////////////////////////////////////////////////////
// File: treatment_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_TREATMENTINTERNALS_HPP_
#define HEPCE_EVENT_TREATMENTINTERNALS_HPP_

// STL Includes
#include <string>

// Library Includes
#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>

// Local Includes
#include "event_internals.hpp"

namespace hepce {
namespace event {
class TreatmentBase : public EventBase {
public:
    struct Eligibilities {
        std::vector<std::string> behavior_states = {};
        std::vector<std::string> fibrosis_states = {};
        std::vector<std::string> pregnancy_states = {};
        int time_since_linked = -2;
        int time_since_last_use = -2;
    };
    struct TreatmentCosts {
        double treatment = 0.0;
        double salvage = 0.0;
        double toxicity = 0.0;
    };
    struct TreatmentUtilities {
        double treatment = 1.0;
        double toxicity = 1.0;
    };
    struct TreatmentProbabilities {
        double loss_to_follow_up = 0.0;
        double initialization = 0.0;
    };

    TreatmentBase(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console")
        : EventBase(model_data, log_name) {
        SetEventCostCategory(model::CostCategory::kTreatment);
        SetEventUtilityCategory(model::UtilityCategory::kTreatment);
        LoadEligibilityData(model_data);
        SetTreatmentLimit(
            utils::GetIntFromConfig("treatment.treatment_limit", model_data));

        _probabilities.loss_to_follow_up = utils::GetDoubleFromConfig(
            "treatment.ltfu_probability", model_data);
        _costs.treatment =
            utils::GetDoubleFromConfig("treatment.treatment_cost", model_data);
        _utilities.treatment = utils::GetDoubleFromConfig(
            "treatment.treatment_utility", model_data);
        _costs.salvage =
            utils::GetDoubleFromConfig("treatment.salvage_cost", model_data);
        _probabilities.initialization = utils::GetDoubleFromConfig(
            "treatment.treatment_initiation", model_data);
        _costs.toxicity =
            utils::GetDoubleFromConfig("treatment.tox_cost", model_data);
        _utilities.toxicity =
            utils::GetDoubleFromConfig("treatment.tox_utility", model_data);
    }

protected:
    inline void SetTreatmentLimit(const int &r) { _treatment_limit = r; }
    inline void SetTreatmentUtilities(const TreatmentUtilities &u) {
        _utilities = u;
    }
    inline void SetTreatmentCosts(const TreatmentCosts &c) { _costs = c; }
    inline void SetTreatmentProbabilities(const TreatmentProbabilities &p) {
        _probabilities = p;
    }
    inline void SetEligibilities(const Eligibilities &e) { _eligibilities = e; }

    inline bool GetTreatmentLimit() const { return _treatment_limit; }
    inline TreatmentUtilities GetTreatmentUtilities() const {
        return _utilities;
    }
    inline TreatmentCosts GetTreatmentCosts() const { return _costs; }
    inline TreatmentProbabilities GetTreatmentProbabilities() const {
        return _probabilities;
    }

    void LoadEligibilityData(datamanagement::ModelData &model_data) {
        _eligibilities.behavior_states = LoadEligibilityVectors(
            "eligibility.ineligible_drug_use", model_data);
        _eligibilities.fibrosis_states = LoadEligibilityVectors(
            "eligibility.ineligible_fibrosis_stages", model_data);
        _eligibilities.pregnancy_states = LoadEligibilityVectors(
            "eligibility.ineligible_pregnancy_states", model_data);
        _eligibilities.time_since_linked = utils::GetIntFromConfig(
            "eligibility.ineligible_time_since_linked", model_data);
        _eligibilities.time_since_last_use = utils::GetIntFromConfig(
            "eligibility.ineligible_time_former_threshold", model_data);
    }

    /// @brief
    /// @return
    virtual data::InfectionType GetInfectionType() const = 0;
    /// @brief
    /// @param
    virtual void ResetUtility(model::Person &person) const = 0;
    /// @brief
    /// @param
    inline void QuitEngagement(model::Person &person) {
        person.EndTreatment(GetInfectionType());
        person.Unlink(GetInfectionType());
        ResetUtility(person);
    }
    /// @brief
    /// @param
    /// @return
    inline bool LostToFollowUp(model::Person &person, model::Sampler &sampler) {
        // If the person is already on treatment, they can't be lost to
        // follow up
        if (!person.GetTreatmentDetails(GetInfectionType())
                 .initiated_treatment &&
            (sampler.GetDecision({_probabilities.loss_to_follow_up}) == 0)) {
            QuitEngagement(person);
            return true;
        }
        return false;
    }
    /// @brief
    /// @param
    /// @return
    std::vector<std::string>
    LoadEligibilityVectors(const std::string &config_key,
                           datamanagement::ModelData &model_data) {
        std::string data = utils::GetStringFromConfig(config_key, model_data);
        if (data.empty()) {
            // Warn empty
            return {};
        }
        return utils::SplitToVecT<std::string>(data, ',');
    }
    /// @brief
    /// This checks eligibility based on a set of conditions:
    /// 1. If they can start salvage or haven't started treatment before
    /// 2. If they have an eligible fibrosis state
    /// 3. If they have an eligible behavior
    /// 4. If they have an eligible pregnancy state
    /// 5. If they've been in the current behavior state long enough
    /// 6. If they've been linked long enough
    /// @param
    /// @return
    bool IsEligible(const model::Person &person) const {
        auto treatment_ready =
            ((_treatment_limit >
              person.GetTreatmentDetails(GetInfectionType()).num_starts) ||
             _treatment_limit < 0) &&
            (!person.GetTreatmentDetails(GetInfectionType())
                  .initiated_treatment);
        return (treatment_ready && IsEligibleFibrosisStage(person) &&
                IsEligibleBehavior(person) && IsEligiblePregnancy(person) &&
                IsEligibleTimeLastActive(person) &&
                IsEligibleTimeSinceLinked(person))
                   ? true
                   : false;
    }

private:
    int _treatment_limit = 0;
    TreatmentUtilities _utilities;
    TreatmentCosts _costs;
    TreatmentProbabilities _probabilities;
    Eligibilities _eligibilities;

    /// @brief
    /// @param
    /// @return
    bool IsEligibleFibrosisStage(const model::Person &person) const {
        auto fibrosis_state = person.GetHCVDetails().fibrosis_state;
        for (std::string state : _eligibilities.fibrosis_states) {
            data::FibrosisState temp;
            temp << state;
            if (fibrosis_state == temp) {
                return false;
            }
        }
        return true;
    }

    bool IsEligibleTimeLastActive(const model::Person &person) const {
        auto time = person.GetBehaviorDetails().time_last_active;
        if (time == -1 || time > _eligibilities.time_since_last_use) {
            return true;
        }
        return false;
    }

    bool IsEligibleTimeSinceLinked(const model::Person &person) const {
        return (
            GetTimeSince(
                person,
                person.GetLinkageDetails(GetInfectionType()).time_link_change) >
            _eligibilities.time_since_linked);
    }

    /// These Eligibility Checks Smell like a Template Function Use Case?

    /// @brief
    /// @param
    /// @return
    bool IsEligibleBehavior(const model::Person &person) const {
        auto behavior = person.GetBehaviorDetails().behavior;
        for (std::string state : _eligibilities.behavior_states) {
            data::Behavior temp;
            temp << state;
            if (behavior == temp) {
                return false;
            }
        }
        return true;
    }
    /// @brief
    /// @param
    /// @return
    bool IsEligiblePregnancy(const model::Person &person) const {
        auto pregnancy_state = person.GetPregnancyDetails().pregnancy_state;
        if (pregnancy_state == data::PregnancyState::kNa) {
            return true; // short circuit for not running pregnancy event
        }
        for (std::string state : _eligibilities.pregnancy_states) {
            data::PregnancyState temp;
            temp << state;
            if (pregnancy_state == temp) {
                return false;
            }
        }
        return true;
    }
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_TREATMENTINTERNALS_HPP_