////////////////////////////////////////////////////////////////////////////////
// File: screening_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_SCREENINGINTERNALS_HPP_
#define HEPCE_EVENT_SCREENINGINTERNALS_HPP_

// STL Includes
#include <functional>

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "event_internals.hpp"

namespace hepce {
namespace event {
class ScreeningBase : public EventBase {
public:
    struct ScreeningData {
        double acute_sensitivity = 0.0;
        double chronic_sensitivity = 0.0;
        double specificity = 0.0;
        double cost = 0.0;
    };

    struct ScreeningProbabilities {
        double background = 0.0;
        double intervention = 0.0;
    };

    using screenmap_t =
        std::unordered_map<utils::tuple_3i, struct ScreeningProbabilities,
                           utils::key_hash_3i, utils::key_equal_3i>;

    ScreeningBase(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console")
        : EventBase(model_data, log_name) {
        std::any storage = _probability;
        model_data.GetDBSource("inputs").Select(ScreenSQL(), CallbackScreening,
                                                storage);
        _probability = std::any_cast<screenmap_t>(storage);
        if (_probability.empty()) {
            // Warn Empty
        }
    }

    void Execute(model::Person &person, model::Sampler &sampler) override {
        // if a person is already linked, skip screening
        if (person.GetLinkageDetails(GetInfectionType()).link_state ==
            data::LinkageState::kLinked) {
            return;
        }

        bool do_one_time_screen = (GetInterventionType() == "one-time") &&
                                  (person.GetCurrentTimestep() == 1);

        bool do_periodic_screen =
            (GetInterventionType() == "periodic") &&
            (GetTimeSince(person, person.GetScreeningDetails(GetInfectionType())
                                      .time_of_last_screening) >=
             GetScreeningPeriod());

        // If it is time to do a one-time intervention screen or periodic
        // screen, run an intervention screen
        if (do_one_time_screen || do_periodic_screen) {
            this->InterventionScreen(person, sampler);
        } else {
            this->BackgroundScreen(person, sampler);
        }
    }

protected:
    virtual data::InfectionType GetInfectionType() const = 0;
    void SetBackgroundRnaData(const ScreeningData &d) {
        _background_rna_data = d;
    }
    void SetBackgroundAbData(const ScreeningData &d) {
        _background_ab_data = d;
    }
    void SetInterventionRnaData(const ScreeningData &d) {
        _intervention_rna_data = d;
    }
    void SetInterventionAbData(const ScreeningData &d) {
        _intervention_ab_data = d;
    }
    void SetSeropositivityBoomerMultiplier(const double &multiplier) {
        _seropositivity_boomer_multiplier = multiplier;
    }
    void SetScreeningPeriod(const int &period) { _screening_period = period; }
    void SetInterventionType(const std::string &type) {
        _intervention_type = type;
    }

    ScreeningData GetBackgroundRnaData() const { return _background_rna_data; }
    ScreeningData GetBackgroundAbData() const { return _background_ab_data; }
    ScreeningData GetInterventionRnaData() const {
        return _intervention_rna_data;
    }
    ScreeningData GetInterventionAbData() const {
        return _intervention_ab_data;
    }
    screenmap_t GetScreeningProbabilities() const { return _probability; }
    double GetSeropositivityBoomerMultiplier() const {
        return _seropositivity_boomer_multiplier;
    }
    int GetScreeningPeriod() const { return _screening_period; }
    std::string GetInterventionType() const { return _intervention_type; }

    static void CallbackScreening(std::any &storage,
                                  const SQLite::Statement &stmt) {
        utils::tuple_3i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt(),
                                              stmt.getColumn(2).getInt());
        std::any_cast<screenmap_t>(storage)[tup] = {
            stmt.getColumn(3).getDouble(), stmt.getColumn(4).getDouble()};
    }

    inline const std::string ScreenSQL() const {
        return "SELECT at.age_years, sl.gender, sl.drug_behavior, "
               "background_screen_probability, intervention_screen_probability "
               "FROM antibody_testing AS at INNER JOIN "
               "screening_and_linkage AS sl ON ((at.age_years = "
               "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";
    }

    bool RunAbTest(model::Person &person, const std::string &prefix,
                   model::Sampler &sampler) {
        std::string key = prefix + "_ab";
        bool test = RunTest(person.GetHCVDetails().hcv, key, sampler,
                            [this, &person]() -> void {
                                return person.AddAbScreen(GetInfectionType());
                            });
        InsertScreeningCost(person, key);
        return test;
    }

    bool RunRnaTest(model::Person &person, const std::string &prefix,
                    model::Sampler &sampler) {
        std::string key = prefix + "_rna";
        bool test = RunTest(person.GetHCVDetails().hcv, key, sampler,
                            [this, &person]() -> void {
                                return person.AddRnaScreen(GetInfectionType());
                            });
        InsertScreeningCost(person, key);
        return test;
    }

    double GetScreeningTypeSensitivitySpecificity(const data::HCV &hcv_status,
                                                  const std::string &key) {
        if (key == "screening_background_rna") {
            if (hcv_status == data::HCV::kAcute) {
                return _background_rna_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::kChronic) {
                return _background_rna_data.chronic_sensitivity;
            } else {
                return 1 - _background_rna_data.specificity;
            }
        } else if (key == "screening_background_ab") {
            if (hcv_status == data::HCV::kAcute) {
                return _background_ab_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::kChronic) {
                return _background_ab_data.chronic_sensitivity;
            } else {
                return 1 - _background_ab_data.specificity;
            }
        } else if (key == "screening_intervention_rna") {
            if (hcv_status == data::HCV::kAcute) {
                return _intervention_rna_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::kChronic) {
                return _intervention_rna_data.chronic_sensitivity;
            } else {
                return 1 - _intervention_rna_data.specificity;
            }
        } else if (key == "screening_intervention_ab") {
            if (hcv_status == data::HCV::kAcute) {
                return _intervention_ab_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::kChronic) {
                return _intervention_ab_data.chronic_sensitivity;
            } else {
                return 1 - _intervention_ab_data.specificity;
            }
        } else {
            return 0.0;
        }
    }

    bool RunTest(const data::HCV &hcv_status, const std::string &key,
                 model::Sampler &sampler, std::function<void(void)> testFunc) {
        double probability =
            GetScreeningTypeSensitivitySpecificity(hcv_status, key);

        testFunc();
        // probability is the chance of false positive or false negative
        bool rValue = (sampler.GetDecision({probability}) == 0) ? true : false;
        return rValue;
    }

    /// @brief The Intervention Screening Event Undertaken on a Person
    /// @param person The Person undergoing an Intervention Screening
    void Screen(std::string screenkey, model::Person &person,
                model::Sampler &sampler) {
        person.MarkScreened(GetInfectionType());
        // if person has had a history of HCV Infections
        if (((screenkey == "screening_intervention" &&
              !person.GetScreeningDetails(data::InfectionType::kHiv)
                   .identified) ||
             screenkey == "screening_background") &&
            (!person.GetScreeningDetails(data::InfectionType::kHiv)
                  .ab_positive)) {
            if (!RunAbTest(person, screenkey, sampler)) {
                return;
            }
        }

        if (RunRnaTest(person, screenkey, sampler)) {
            data::LinkageType type = (screenkey == "screening_background")
                                         ? data::LinkageType::kBackground
                                         : data::LinkageType::kIntervention;
            person.SetLinkageType(type, GetInfectionType());
            person.Diagnose(GetInfectionType());
        }
    }

    double GetScreeningProbability(model::Person &person,
                                   const std::string &colname) {
        int age_years = static_cast<int>(person.GetAge() / 12.0);
        int gender = static_cast<int>(person.GetSex());
        int drug_behavior =
            static_cast<int>(person.GetBehaviorDetails().behavior);
        utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);

        double probability = 0.0;
        if (colname == "background_screen_probability") {
            probability = _probability[tup].background;
        } else if (colname == "intervention_screen_probability") {
            probability = _probability[tup].intervention;
        }
        if (person.IsBoomer()) {
            probability *= _seropositivity_boomer_multiplier;
        }
        return probability;
    }

    int InterventionScreen(model::Person &person, model::Sampler &sampler) {
        double interventionProbability =
            GetScreeningProbability(person, "intervention_screen_probability");
        int decision = sampler.GetDecision({interventionProbability});
        if (decision == 0) {
            Screen("screening_intervention", person, sampler);
        }
        return decision;
    }

    int BackgroundScreen(model::Person &person, model::Sampler &sampler) {
        double backgroundProbability =
            GetScreeningProbability(person, "background_screen_probability");
        int decision = sampler.GetDecision({backgroundProbability});
        if (decision == 0) {
            Screen("screening_background", person, sampler);
        }
        return decision;
    }

    /// @brief Insert cost for screening of type \code{type}
    /// @param person The person who is accruing cost
    /// @param type The screening type, used to discern the cost to add
    void InsertScreeningCost(model::Person &person, const std::string &key) {
        if (key == "screening_background_rna") {
            AddEventCost(person, _background_rna_data.cost);
        } else if (key == "screening_background_ab") {
            AddEventCost(person, _background_ab_data.cost);
        } else if (key == "screening_intervention_rna") {
            AddEventCost(person, _intervention_rna_data.cost);
        } else if (key == "screening_intervention_ab") {
            AddEventCost(person, _intervention_ab_data.cost);
        }
    }

private:
    ScreeningData _background_rna_data;
    ScreeningData _background_ab_data;
    ScreeningData _intervention_rna_data;
    ScreeningData _intervention_ab_data;
    screenmap_t _probability;

    double _seropositivity_boomer_multiplier;
    int _screening_period;
    std::string _intervention_type;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_SCREENINGINTERNALS_HPP_