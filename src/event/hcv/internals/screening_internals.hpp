////////////////////////////////////////////////////////////////////////////////
// File: screening_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_SCREENINGINTERNALS_HPP_
#define HEPCE_EVENT_HCV_SCREENINGINTERNALS_HPP_

#include <hepce/event/hcv/screening.hpp>

#include <functional>

#include "internals/event_internals.hpp"
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

namespace hepce {
namespace event {
namespace hcv {
class ScreeningImpl : public virtual Screening, public EventBase {
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

    ScreeningImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name = "console");

    ~ScreeningImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    ScreeningData _background_rna_data;
    ScreeningData _background_ab_data;
    ScreeningData _intervention_rna_data;
    ScreeningData _intervention_ab_data;
    screenmap_t _probability;

    double _seropositivity_boomer_multiplier;
    int _screening_period;
    std::string _intervention_type;

    static int CallbackScreening(void *storage, int count, char **data,
                                 char **columns) {
        utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((screenmap_t *)storage))[tup] = {utils::SToDPositive(data[3]),
                                            utils::SToDPositive(data[4])};
        return 0;
    }

    inline const std::string ScreenSQL() const {
        return "SELECT at.age_years, sl.gender, sl.drug_behavior, "
               "background_screen_probability, intervention_screen_probability "
               "FROM antibody_testing AS at INNER JOIN "
               "screening_and_linkage AS sl ON ((at.age_years = "
               "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";
    }

    bool RunAbTest(model::Person &person,
                   std::shared_ptr<datamanagement::DataManagerBase> dm,
                   const std::string &prefix, model::Sampler &sampler) {
        std::string key = prefix + "_ab";
        bool test = RunTest(person.GetHCV(), key, sampler, [&person]() -> int {
            return person.AddAbScreen();
        });
        InsertScreeningCost(person, key);
        return test;
    }

    bool RunRnaTest(model::Person &person,
                    std::shared_ptr<datamanagement::DataManagerBase> dm,
                    const std::string &prefix, model::Sampler &sampler) {
        std::string key = prefix + "_rna";
        bool test = RunTest(person.GetHCV(), key, sampler, [&person]() -> int {
            return person.AddRnaScreen();
        });
        InsertScreeningCost(person, key);
        return test;
    }

    double GetScreeningTypeSensitivitySpecificity(const data::HCV &hcv_status,
                                                  const std::string &key) {
        if (key == "screening_background_rna") {
            if (hcv_status == data::HCV::ACUTE) {
                return _background_rna_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::CHRONIC) {
                return _background_rna_data.chronic_sensitivity;
            } else {
                return 1 - _background_rna_data.specificity;
            }
        } else if (key == "screening_background_ab") {
            if (hcv_status == data::HCV::ACUTE) {
                return _background_ab_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::CHRONIC) {
                return _background_ab_data.chronic_sensitivity;
            } else {
                return 1 - _background_ab_data.specificity;
            }
        } else if (key == "screening_intervention_rna") {
            if (hcv_status == data::HCV::ACUTE) {
                return _intervention_rna_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::CHRONIC) {
                return _intervention_rna_data.chronic_sensitivity;
            } else {
                return 1 - _intervention_rna_data.specificity;
            }
        } else if (key == "screening_intervention_ab") {
            if (hcv_status == data::HCV::ACUTE) {
                return _intervention_ab_data.acute_sensitivity;
            } else if (hcv_status == data::HCV::CHRONIC) {
                return _intervention_ab_data.chronic_sensitivity;
            } else {
                return 1 - _intervention_ab_data.specificity;
            }
        } else {
            return 0.0;
        }
    }

    bool RunTest(const data::HCV &hcv_status, const std::string &key,
                 model::Sampler &sampler, std::function<int(void)> testFunc) {
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
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) {
        person.MarkScreened();
        // if person has had a history of HCV Infections
        if (((screenkey == "screening_intervention" &&
              !person.IsIdentifiedAsInfected()) ||
             screenkey == "screening_background") &&
            (!person.CheckAntibodyPositive())) {
            if (!RunAbTest(person, dm, screenkey, sampler)) {
                return;
            }
        }

        if (RunRnaTest(person, dm, screenkey, sampler)) {
            data::LinkageType type = (screenkey == "screening_background")
                                         ? data::LinkageType::BACKGROUND
                                         : data::LinkageType::INTERVENTION;
            person.SetLinkageType(type);
            person.Diagnose();
        }
    }

    double GetScreeningProbability(model::Person &person,
                                   const std::string &colname) {
        int age_years = static_cast<int>(person.GetAge() / 12.0);
        int gender = static_cast<int>(person.GetSex());
        int drug_behavior = static_cast<int>(person.GetBehavior());
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

    int InterventionScreen(model::Person &person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           model::Sampler &sampler) {
        double interventionProbability =
            GetScreeningProbability(person, "intervention_screen_probability");
        int decision = sampler.GetDecision({interventionProbability});
        if (decision == 0) {
            Screen("screening_intervention", person, dm, sampler);
        }
        return decision;
    }

    int BackgroundScreen(model::Person &person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         model::Sampler &sampler) {
        double backgroundProbability =
            GetScreeningProbability(person, "background_screen_probability");
        int decision = sampler.GetDecision({backgroundProbability});
        if (decision == 0) {
            Screen("screening_background", person, dm, sampler);
        }
        return decision;
    }

    /// @brief Insert cost for screening of type \code{type}
    /// @param person The person who is accruing cost
    /// @param type The screening type, used to discern the cost to add
    void InsertScreeningCost(model::Person &person, const std::string &key) {
        if (key == "screening_background_rna") {
            SetCost(_background_rna_data.cost);
        } else if (key == "screening_background_ab") {
            SetCost(_background_ab_data.cost);
        } else if (key == "screening_intervention_rna") {
            SetCost(_intervention_rna_data.cost);
        } else if (key == "screening_intervention_ab") {
            SetCost(_intervention_ab_data.cost);
        }

        AddEventCost(person);
    }
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_SCREENINGINTERNALS_HPP_