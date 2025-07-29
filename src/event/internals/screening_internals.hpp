////////////////////////////////////////////////////////////////////////////////
// File: screening_internals.hpp                                              //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-25                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_SCREENINGINTERNALS_HPP_
#define HEPCE_EVENT_SCREENINGINTERNALS_HPP_

// STL Includes
#include <functional>

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>
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
        : EventBase(model_data, log_name) {}

    void Execute(model::Person &person, model::Sampler &sampler) override {
        if (!ValidExecute(person)) {
            return;
        }
        // if a person is already linked, skip screening
        if (person.GetLinkageDetails(GetInfectionType()).link_state ==
            data::LinkageState::kLinked) {
            return;
        }

        bool do_one_time_screen = (GetInterventionType() == "one-time") &&
                                  (person.GetCurrentTimestep() == 1);

        bool do_periodic_screen = IsPeriodicScreen(person);

        // If it is time to do a one-time intervention screen or periodic
        // screen, run an intervention screen
        if (do_one_time_screen || do_periodic_screen) {
            InterventionScreen(person, sampler);
        } else {
            BackgroundScreen(person, sampler);
        }
    }

protected:
    virtual data::InfectionType GetInfectionType() const = 0;

    inline void LoadScreeningData(datamanagement::ModelData &model_data) {
        std::any storage = screenmap_t{};
        try {
            model_data.GetDBSource("inputs").Select(ScreenSQL(),
                                                    CallbackScreening, storage);
        } catch (std::exception &e) {
            std::stringstream msg;
            msg << "Error getting " << GetInfectionType()
                << " Screening Data: " << e.what();
            hepce::utils::LogError(GetLogName(), msg.str());
            return;
        }
        _probability = std::any_cast<screenmap_t>(storage);
        if (_probability.empty()) {
            std::stringstream s;
            s << GetInfectionType() << " Linking Data is Empty...";
            hepce::utils::LogWarning(GetLogName(), s.str());
#ifdef EXIT_ON_WARNING
            std::exit(EXIT_FAILURE);
#endif
        }
    }

    inline void SetBackgroundRnaData(const ScreeningData &d) {
        _background_rna_data = d;
    }
    inline void SetBackgroundAbData(const ScreeningData &d) {
        _background_ab_data = d;
    }
    inline void SetInterventionRnaData(const ScreeningData &d) {
        _intervention_rna_data = d;
    }
    inline void SetInterventionAbData(const ScreeningData &d) {
        _intervention_ab_data = d;
    }
    inline void SetSeropositivityBoomerMultiplier(const double &multiplier) {
        _seropositivity_boomer_multiplier = multiplier;
    }
    inline void SetScreeningPeriod(const int &period) {
        _screening_period = period;
    }
    inline void SetInterventionType(const std::string &type) {
        _intervention_type = type;
    }

    inline ScreeningData GetBackgroundRnaData() const {
        return _background_rna_data;
    }
    inline ScreeningData GetBackgroundAbData() const {
        return _background_ab_data;
    }
    inline ScreeningData GetInterventionRnaData() const {
        return _intervention_rna_data;
    }
    inline ScreeningData GetInterventionAbData() const {
        return _intervention_ab_data;
    }
    inline screenmap_t GetScreeningProbabilities() const {
        return _probability;
    }
    inline double GetSeropositivityBoomerMultiplier() const {
        return _seropositivity_boomer_multiplier;
    }
    inline int GetScreeningPeriod() const { return _screening_period; }
    inline std::string GetInterventionType() const {
        return _intervention_type;
    }

    static void CallbackScreening(std::any &storage,
                                  const SQLite::Statement &stmt) {
        screenmap_t *temp = std::any_cast<screenmap_t>(&storage);
        utils::tuple_3i tup = std::make_tuple(stmt.getColumn(0).getInt(),
                                              stmt.getColumn(1).getInt(),
                                              stmt.getColumn(2).getInt());
        (*temp)[tup] = {stmt.getColumn(3).getDouble(),
                        stmt.getColumn(4).getDouble()};
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

    /// @brief Insert cost for screening of type \code{type}
    /// @param person The person who is accruing cost
    /// @param type The screening type, used to discern the cost to add
    inline void InsertScreeningCost(model::Person &person,
                                    const data::ScreeningType &type,
                                    const data::ScreeningTest &test) {
        if (type == data::ScreeningType::kBackground &&
            test == data::ScreeningTest::kRna) {
            AddEventCost(person, _background_rna_data.cost);
        } else if (type == data::ScreeningType::kBackground &&
                   test == data::ScreeningTest::kAb) {
            AddEventCost(person, _background_ab_data.cost);
        } else if (type == data::ScreeningType::kIntervention &&
                   test == data::ScreeningTest::kRna) {
            AddEventCost(person, _intervention_rna_data.cost);
        } else if (type == data::ScreeningType::kIntervention &&
                   test == data::ScreeningTest::kAb) {
            AddEventCost(person, _intervention_ab_data.cost);
        }
    }

    inline double GetScreeningProbability(model::Person &person,
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
            // there is no need to scale the probability up if it is already 1.
            // it is also outside the domain of ln(1-prob), needed by
            // `ProbabilityToRate()`
            if (probability < 1.0) {
                probability = utils::RateToProbability(
                    utils::ProbabilityToRate(probability) *
                    _seropositivity_boomer_multiplier);
            }
        }
        return probability;
    }

    inline int InterventionScreen(model::Person &person,
                                  model::Sampler &sampler) {
        double interventionProbability =
            GetScreeningProbability(person, "intervention_screen_probability");
        int decision = sampler.GetDecision({interventionProbability});
        if (decision == 0) {
            Screen(data::ScreeningType::kIntervention, person, sampler);
        }
        return decision;
    }

    inline int BackgroundScreen(model::Person &person,
                                model::Sampler &sampler) {
        double backgroundProbability =
            GetScreeningProbability(person, "background_screen_probability");
        int decision = sampler.GetDecision({backgroundProbability});
        if (decision == 0) {
            Screen(data::ScreeningType::kBackground, person, sampler);
        }
        return decision;
    }

    /// @brief Determine whether periodic screening should happen
    /// when periodic screening, first attempt to screen people on the first
    /// timestep rather than waiting until a full period has passed before
    /// the first screen: "if intervention is periodic AND (the period has
    /// been reached OR it is the first timestep), intervention screen"
    /// @param person The person to be checked that the conditions are met
    /// @return Whether intervention screening should happen this timestep
    inline bool IsPeriodicScreen(model::Person &person) {
        bool is_periodic = (GetInterventionType() == "periodic");
        if (!is_periodic) {
            return false;
        }
        bool period_reached =
            (GetTimeSince(person, person.GetScreeningDetails(GetInfectionType())
                                      .time_of_last_screening) >=
             GetScreeningPeriod());
        bool is_first_timestep = (person.GetCurrentTimestep() == 1);
        return (period_reached || is_first_timestep);
    }

    inline const std::string ScreenSQL() const {
        return "SELECT at.age_years, sl.gender, sl.drug_behavior, "
               "background_screen_probability, intervention_screen_probability "
               "FROM antibody_testing AS at INNER JOIN "
               "screening_and_linkage AS sl ON ((at.age_years = "
               "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";
    }

    inline bool RunTest(model::Person &person, const data::ScreeningType &type,
                        const data::ScreeningTest &test,
                        model::Sampler &sampler) {
        double probability = GetScreeningTypeSensitivitySpecificity(
            person.GetHCVDetails().hcv, test, type);
        person.Screen(GetInfectionType(), test, type);
        bool result = (sampler.GetDecision({probability}) == 0) ? true : false;

        InsertScreeningCost(person, type, test);
        return result;
    }

    inline double
    GetScreeningTypeSensitivitySpecificity(const data::HCV &hcv_status,
                                           const data::ScreeningTest &test,
                                           const data::ScreeningType &type) {
        if (test == data::ScreeningTest::kRna &&
            type == data::ScreeningType::kBackground) {
            if (hcv_status == data::HCV::kAcute) {
                return _background_rna_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::kChronic) {
                return _background_rna_data.chronic_sensitivity;
            } else {
                return 1 - _background_rna_data.specificity;
            }
        } else if (test == data::ScreeningTest::kAb &&
                   type == data::ScreeningType::kBackground) {
            if (hcv_status == data::HCV::kAcute) {
                return _background_ab_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::kChronic) {
                return _background_ab_data.chronic_sensitivity;
            } else {
                return 1 - _background_ab_data.specificity;
            }
        } else if (test == data::ScreeningTest::kRna &&
                   type == data::ScreeningType::kIntervention) {
            if (hcv_status == data::HCV::kAcute) {
                return _intervention_rna_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::kChronic) {
                return _intervention_rna_data.chronic_sensitivity;
            } else {
                return 1 - _intervention_rna_data.specificity;
            }
        } else if (test == data::ScreeningTest::kAb &&
                   type == data::ScreeningType::kIntervention) {
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

    /// @brief The Intervention Screening Event Undertaken on a Person
    /// @param person The Person undergoing an Intervention Screening
    inline void Screen(data::ScreeningType type, model::Person &person,
                       model::Sampler &sampler) {
        bool identified =
            person.GetScreeningDetails(GetInfectionType()).identified;
        // if person is intervention screening and not identified OR background
        // screened
        bool valid_screen =
            ((type == data::ScreeningType::kIntervention && !identified) ||
             type == data::ScreeningType::kBackground);

        if (!valid_screen) {
            return;
        }

        if (!person.GetScreeningDetails(GetInfectionType()).ab_positive) {
            if (!RunTest(person, type, data::ScreeningTest::kAb, sampler) &&
                person.GetHCVDetails().hcv != data::HCV::kNone) {
                person.AddFalseNegative(GetInfectionType());
                person.ClearDiagnosis(GetInfectionType());
                return;
            }
        }

        if (RunTest(person, type, data::ScreeningTest::kRna, sampler)) {
            if (!identified) {
                person.Diagnose(GetInfectionType());
            }
        } else if (person.GetHCVDetails().hcv != data::HCV::kNone) {
            person.AddFalseNegative(GetInfectionType());
            person.ClearDiagnosis(GetInfectionType());
        }
    }
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_SCREENINGINTERNALS_HPP_
