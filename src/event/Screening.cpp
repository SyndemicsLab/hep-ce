////////////////////////////////////////////////////////////////////////////////
// File: Screening.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Screening.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace event {
class Screening::ScreeningIMPL {
private:
    double discount = 0.0;

    double background_rna_acute_sensitivity;
    double background_rna_chronic_sensitivity;
    double background_rna_specificity;
    double background_rna_cost;

    double background_ab_acute_sensitivity;
    double background_ab_chronic_sensitivity;
    double background_ab_specificity;
    double background_ab_cost;

    double intervention_rna_acute_sensitivity;
    double intervention_rna_chronic_sensitivity;
    double intervention_rna_specificity;
    double intervention_rna_cost;

    double intervention_ab_acute_sensitivity;
    double intervention_ab_chronic_sensitivity;
    double intervention_ab_specificity;
    double intervention_ab_cost;

    double seropositivity_boomer_multiplier;
    int screening_period;
    std::string intervention_type;

    using screenmap_t =
        std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                           Utils::key_equal_3i>;
    screenmap_t intervention_screen_data;
    screenmap_t background_screen_data;

    static int callback_screen(void *storage, int count, char **data,
                               char **columns) {
        Utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((screenmap_t *)storage))[tup] = Utils::stod_positive(data[3]);
        return 0;
    }

    std::string ScreenSQL(std::string column) const {
        return "SELECT at.age_years, sl.gender, sl.drug_behavior, " + column +
               " FROM antibody_testing AS at INNER JOIN "
               "screening_and_linkage AS sl ON ((at.age_years = "
               "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";
    }

    bool runABTest(std::shared_ptr<person::PersonBase> person,
                   std::shared_ptr<datamanagement::DataManagerBase> dm,
                   std::string prefix,
                   std::shared_ptr<stats::DeciderBase> decider) {
        bool test =
            this->test(person->GetHCV(), dm, prefix + "_ab", decider,
                       [&person]() -> int { return person->AddAbScreen(); });
        this->InsertScreeningCost(person, dm, prefix + "_ab.cost",
                                  "Antibody Screening");
        return test;
    }

    bool runRNATest(std::shared_ptr<person::PersonBase> person,
                    std::shared_ptr<datamanagement::DataManagerBase> dm,
                    std::string prefix,
                    std::shared_ptr<stats::DeciderBase> decider) {
        bool test =
            this->test(person->GetHCV(), dm, prefix + "_rna", decider,
                       [&person]() -> int { return person->AddRnaScreen(); });
        this->InsertScreeningCost(person, dm, prefix + "_rna.cost",
                                  "RNA Screening");
        return test;
    }

    double GetScreeningTypeSensitivitySpecificity(person::HCV infectionStatus,
                                                  std::string configKey) {
        if (configKey == "screening_background_rna") {
            if (infectionStatus == person::HCV::ACUTE) {
                return background_rna_acute_sensitivity;
            } else if (infectionStatus == person::HCV::CHRONIC) {
                return background_rna_chronic_sensitivity;
            } else {
                return 1 - background_rna_specificity;
            }
        } else if (configKey == "screening_background_ab") {
            if (infectionStatus == person::HCV::ACUTE) {
                return background_ab_acute_sensitivity;
            } else if (infectionStatus == person::HCV::CHRONIC) {
                return background_ab_chronic_sensitivity;
            } else {
                return 1 - background_ab_specificity;
            }
        } else if (configKey == "screening_intervention_rna") {
            if (infectionStatus == person::HCV::ACUTE) {
                return intervention_rna_acute_sensitivity;
            } else if (infectionStatus == person::HCV::CHRONIC) {
                return intervention_rna_chronic_sensitivity;
            } else {
                return 1 - intervention_rna_specificity;
            }
        } else if (configKey == "screening_intervention_ab") {
            if (infectionStatus == person::HCV::ACUTE) {
                return intervention_ab_acute_sensitivity;
            } else if (infectionStatus == person::HCV::CHRONIC) {
                return intervention_ab_chronic_sensitivity;
            } else {
                return 1 - intervention_ab_specificity;
            }
        } else {
            return 0.0;
        }
    }

    bool test(person::HCV infectionStatus,
              std::shared_ptr<datamanagement::DataManagerBase> dm,
              std::string configKey,
              std::shared_ptr<stats::DeciderBase> decider,
              std::function<int(void)> testFunc) {
        double probability =
            GetScreeningTypeSensitivitySpecificity(infectionStatus, configKey);

        testFunc();
        // probability is the chance of false positive or false negative
        bool rValue = (decider->GetDecision({probability}) == 0) ? true : false;
        return rValue;
    }

    /// @brief The Intervention Screening Event Undertaken on a Person
    /// @param person The Person undergoing an Intervention Screening
    void Screen(std::string screenkey,
                std::shared_ptr<person::PersonBase> person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                std::shared_ptr<stats::DeciderBase> decider) {
        person->MarkScreened();
        // if person has had a history of HCV Infections
        if (((screenkey == "screening_intervention" &&
              !person->IsIdentifiedAsHCVInfected()) ||
             screenkey == "screening_background") &&
            (!person->HistoryOfHCVInfection())) {
            if (!runABTest(person, dm, screenkey, decider)) {
                return;
            }
        }

        if (runRNATest(person, dm, screenkey, decider)) {
            person::LinkageType type = (screenkey == "screening_background")
                                           ? person::LinkageType::BACKGROUND
                                           : person::LinkageType::INTERVENTION;
            person->SetLinkageType(type);
            person->DiagnoseHCV();
        }
    }

    double GetScreeningProbability(
        std::string colname, std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        int age_years = (int)(person->GetAge() / 12.0);
        int gender = (int)person->GetSex();
        int drug_behavior = (int)person->GetBehavior();
        Utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);

        double probability = 0.0;
        if (colname == "background_screen_probability") {
            probability = background_screen_data[tup];
        } else if (colname == "intervention_screen_probability") {
            probability = intervention_screen_data[tup];
        }
        if (person->IsBoomer()) {
            probability *= seropositivity_boomer_multiplier;
        }
        return probability;
    }

    int InterventionScreen(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::shared_ptr<stats::DeciderBase> decider) {
        double interventionProbability = this->GetScreeningProbability(
            "intervention_screen_probability", person, dm);
        int decision = decider->GetDecision({interventionProbability});
        if (decision == 0) {
            this->Screen("screening_intervention", person, dm, decider);
        }
        return decision;
    }

    int BackgroundScreen(std::shared_ptr<person::PersonBase> person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         std::shared_ptr<stats::DeciderBase> decider) {
        double backgroundProbability = this->GetScreeningProbability(
            "background_screen_probability", person, dm);
        int decision = decider->GetDecision({backgroundProbability});
        if (decision == 0) {
            this->Screen("screening_background", person, dm, decider);
        }
        return decision;
    }

    /// @brief Insert cost for screening of type \code{type}
    /// @param person The person who is accruing cost
    /// @param type The screening type, used to discern the cost to add
    void
    InsertScreeningCost(std::shared_ptr<person::PersonBase> person,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        std::string configKey, std::string screeningName) {
        double screeningCost;
        if (configKey == "screening_background_rna") {
            screeningCost = background_rna_cost;
        } else if (configKey == "screening_background_ab") {
            screeningCost = background_ab_cost;
        } else if (configKey == "screening_intervention_rna") {
            screeningCost = intervention_rna_cost;
        } else if (configKey == "screening_intervention_ab") {
            screeningCost = intervention_ab_cost;
        } else {
            screeningCost = 0.0;
        }

        double discountAdjustedCost = Event::DiscountEventCost(
            screeningCost, discount, person->GetCurrentTimestep());
        person->AddCost(screeningCost, discountAdjustedCost,
                        cost::CostCategory::SCREENING);
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // if a person is already linked, skip screening
        if (person->GetLinkState() == person::LinkageState::LINKED) {
            return;
        }

        bool do_one_time_screen = (intervention_type == "one-time") &&
                                  (person->GetCurrentTimestep() == 1);

        bool do_periodic_screen =
            (intervention_type == "periodic") &&
            (person->GetTimeSinceLastScreening() >= screening_period);

        // If it is time to do a one-time intervention screen or periodic
        // screen, run an intervention screen
        if (do_one_time_screen || do_periodic_screen) {
            this->InterventionScreen(person, dm, decider);
        } else {
            this->BackgroundScreen(person, dm, decider);
        }
    }
    ScreeningIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string discount_data;
        int rc = dm->GetFromConfig("cost.discounting_rate", discount_data);
        if (!discount_data.empty()) {
            this->discount = Utils::stod_positive(discount_data);
        }
        std::string data;

        // Background RNA
        dm->GetFromConfig("screening_background_rna.acute_sensitivity", data);
        background_rna_acute_sensitivity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_background_rna.chronic_sensitivity", data);
        background_rna_chronic_sensitivity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_background_rna.specificity", data);
        background_rna_specificity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_background_rna.cost", data);
        background_rna_cost = Utils::stod_positive(data);

        // Background AB
        dm->GetFromConfig("screening_background_ab.acute_sensitivity", data);
        background_ab_acute_sensitivity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_background_ab.chronic_sensitivity", data);
        background_ab_chronic_sensitivity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_background_ab.specificity", data);
        background_ab_specificity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_background_ab.cost", data);
        background_ab_cost = Utils::stod_positive(data);

        // Intervention RNA
        dm->GetFromConfig("screening_intervention_rna.acute_sensitivity", data);
        intervention_rna_acute_sensitivity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_intervention_rna.chronic_sensitivity",
                          data);
        intervention_rna_chronic_sensitivity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_intervention_rna.specificity", data);
        intervention_rna_specificity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_intervention_rna.cost", data);
        intervention_rna_cost = Utils::stod_positive(data);

        // Intervention AB
        dm->GetFromConfig("screening_intervention_ab.acute_sensitivity", data);
        intervention_ab_acute_sensitivity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_intervention_ab.chronic_sensitivity",
                          data);
        intervention_ab_chronic_sensitivity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_intervention_ab.specificity", data);
        intervention_ab_specificity = Utils::stod_positive(data);
        dm->GetFromConfig("screening_intervention_ab.cost", data);
        intervention_ab_cost = Utils::stod_positive(data);

        // Other Config Gets
        dm->GetFromConfig("screening.seropositivity_multiplier_boomer", data);
        seropositivity_boomer_multiplier =
            (data.empty()) ? 1.0 : Utils::stod_positive(data);
        dm->GetFromConfig("screening.period", data);
        screening_period = (data.empty()) ? 0 : std::stoi(data);
        dm->GetFromConfig("screening.intervention_type", intervention_type);

        std::string error;
        rc = dm->SelectCustomCallback(
            ScreenSQL("background_screen_probability"), this->callback_screen,
            &background_screen_data, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "Error extracting background_screen_probability "
                "Screening Data from antibody_testing and "
                "screening_and_linkage! Error Message: {}",
                error);
        }

        if (background_screen_data.empty()) {
            spdlog::get("main")->warn(
                "No Background Screening Probabilities Found.");
        }

        rc = dm->SelectCustomCallback(
            ScreenSQL("intervention_screen_probability"), this->callback_screen,
            &intervention_screen_data, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "Error extracting intervention_screen_probability "
                "Screening Data from antibody_testing and "
                "screening_and_linkage! Error Message: {}",
                error);
        }

        if (intervention_screen_data.empty()) {
            spdlog::get("main")->warn(
                "No Intervention Screening Probabilities Found.");
        }
    }
};

Screening::Screening(std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<ScreeningIMPL>(dm);
}

Screening::~Screening() = default;
Screening::Screening(Screening &&) noexcept = default;
Screening &Screening::operator=(Screening &&) noexcept = default;

void Screening::DoEvent(std::shared_ptr<person::PersonBase> person,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
