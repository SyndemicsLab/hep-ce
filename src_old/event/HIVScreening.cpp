////////////////////////////////////////////////////////////////////////////////
// File: HIVScreening.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-06                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVScreening.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <datamanagement/datamanagement.hpp>
#include <string>
#include <unordered_map>

namespace event {
class HIVScreening::HIVScreeningIMPL {
private:
    // constants
    // two types of screening - background and intervention
    /// @brief
    enum class SCREEN_TYPE : int {
        kBackground = 0,
        kIntervention = 1,
        kCount = 2
    };
    /// @brief
    const person::InfectionType INF_TYPE = person:: ::kHiv;
    /// @brief
    const cost::CostCategory COST_CATEGORY = cost::CostCategory::HIV;
    /// @brief
    const std::unordered_map<SCREEN_TYPE, std::string> TYPE_COLUMNS = {
        {SCREEN_TYPE::kBackground, "hiv_background_screen_probability"},
        {SCREEN_TYPE::kIntervention, "hiv_intervention_screen_probability"}};

    // user-provided values
    double discount = 0.0;

    // intervention HIV screening
    int screening_period;
    std::string intervention_type;

    // details of each test
    struct TestCharacteristics {
        double ab_sensitivity;
        double ab_specificity;
        double ab_cost;
        double rna_sensitivity;
        double rna_specificity;
        double rna_cost;
    };
    std::unordered_map<SCREEN_TYPE, TestCharacteristics> test_data;

    // data for test acceptance
    using hivscreenmap_t =
        std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                           Utils::key_equal_3i>;
    hivscreenmap_t hiv_background_screen_data;
    hivscreenmap_t hiv_intervention_screen_data;

    /// @brief
    /// @param
    /// @return
    static int callback_hivscreen(void *storage, int count, char **data,
                                  char **columns) {
        Utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((hivscreenmap_t *)storage))[tup] = Utils::stod_positive(data[3]);
        return 0;
    }

    /// @brief
    /// @param
    /// @return
    std::string HIVScreenSQL(std::string column) const {
        return "SELECT at.age_years, sl.gender, sl.drug_behavior, " + column +
               " FROM antibody_testing AS at INNER JOIN "
               "screening_and_linkage AS sl ON ((at.age_years = "
               "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";
    }

    /// @brief
    /// @param
    /// @return A reference to the map used to store data for the screen type
    hivscreenmap_t *PickMap(SCREEN_TYPE type) {
        switch (type) {
        case SCREEN_TYPE::kBackground:
            return &hiv_background_screen_data;
        case SCREEN_TYPE::kIntervention:
            return &hiv_intervention_screen_data;
        default:
            break;
        }
        return nullptr;
    }

    /// @brief
    /// @param
    /// @return
    double GetScreeningProbability(std::shared_ptr<person::PersonBase> person,
                                   SCREEN_TYPE type) {
        int age_years = static_cast<int>(person->GetAge() / 12.0);
        int gender = static_cast<int>(person->GetSex());
        int drug_behavior = static_cast<int>(person->GetBehavior());
        Utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);

        double probability = 0.0;
        switch (type) {
        case SCREEN_TYPE::kBackground:
            probability = hiv_background_screen_data[tup];
            break;
        case SCREEN_TYPE::kIntervention:
            probability = hiv_intervention_screen_data[tup];
            break;
        default:
            break;
        }
        return probability;
    }

    void AddScreeningCost(std::shared_ptr<person::PersonBase> person,
                          double base_cost) {
        double discounted_cost = DiscountEventCost(
            base_cost, discount, person->GetCurrentTimestep());
        person->AddCost(base_cost, discounted_cost, COST_CATEGORY);
    }

    /// @brief
    /// @param
    /// @return
    bool RunABScreen(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<stats::DeciderBase> decider,
                     SCREEN_TYPE type) {
        person->AddAbScreen(INF_TYPE);
        AddScreeningCost(person, test_data[type].ab_cost);
        // if person is HIV infected, use sensitivity for this probability.
        // otherwise, use (1 - sensitivity), i.e. rate of false positive
        double prob_positive = (person->GetHIV() != person::HIV::kNone)
                                   ? test_data[type].ab_sensitivity
                                   : 1 - test_data[type].ab_specificity;

        // determine the test outcome -- if decider returns 0, positive
        bool test_outcome =
            (decider->GetDecision({prob_positive}) == 0) ? true : false;
        return test_outcome;
    }

    /// @brief
    /// @param
    /// @return
    bool RunRNAScreen(std::shared_ptr<person::PersonBase> person,
                      std::shared_ptr<stats::DeciderBase> decider,
                      SCREEN_TYPE type) {
        // increment rna screening for person
        person->AddRnaScreen(INF_TYPE);
        // accumulate the cost of screening
        AddScreeningCost(person, test_data[type].rna_cost);
        // if person is HIV infected, use sensitivity for this probability.
        // otherwise, use (1 - sensitivity), i.e. rate of false positive
        double prob_positive = (person->GetHIV() != person::HIV::kNone)
                                   ? test_data[type].ab_sensitivity
                                   : 1 - test_data[type].ab_specificity;
        // determine the test outcome -- if decider returns 0, true
        bool test_outcome =
            (decider->GetDecision({prob_positive}) == 0) ? true : false;
        return test_outcome;
    }

    /// @brief
    /// @param
    void AttemptScreen(std::shared_ptr<person::PersonBase> person,
                       std::shared_ptr<stats::DeciderBase> decider,
                       SCREEN_TYPE type = SCREEN_TYPE::kBackground) {
        // get the chance that a person will be screened
        double screen_probability = this->GetScreeningProbability(person, type);
        // do not screen if the decision does not evaluate to 0
        if (decider->GetDecision({screen_probability}) != 0) {
            return;
        }

        // mark that person has been HIV screened on the current timestep
        person->MarkScreened(INF_TYPE);

        // if not already tested positive for antibody, HIV antibody screen
        if (!person->CheckAntibodyPositive(INF_TYPE)) {
            // stop screening if person isn't found antibody positive
            if (!RunABScreen(person, decider, type)) {
                return;
            }
            // mark positive otherwise
            person->SetAntibodyPositive(true, INF_TYPE);
        }

        // if person reaches this part, they are ab positive. rna screen.
        if (!RunRNAScreen(person, decider, type)) {
            return;
        }
        // if rna positive, mark hiv identified
        person->Diagnose(INF_TYPE);
        // if this was an intervention screen, set linkage type to
        // intervention
        if (type == SCREEN_TYPE::kIntervention) {
            person->SetLinkageType(person::LinkageType::kIntervention,
                                   INF_TYPE);
            return;
        }
        person->SetLinkageType(person::LinkageType::kBackground, INF_TYPE);
    }

    /// @brief
    /// @param
    void LoadScreeningData(SCREEN_TYPE type,
                           datamanagement::ModelData &model_data) {
        hivscreenmap_t *chosen_screenmap = PickMap(type);
        std::string column = TYPE_COLUMNS.at(type);
        std::string error;
        int rc = dm->SelectCustomCallback(HIVScreenSQL(column),
                                          this->callback_hivscreen,
                                          chosen_screenmap, error);
        if (rc != 0) {
            spdlog::get("main")->error("Error retrieving HIV Screening values "
                                       "for column `{}'! Error Message: {}",
                                       column, error);
        }
        if ((*chosen_screenmap).empty()) {
            spdlog::get("main")->warn("No `" + column + "' found.");
        }
    }

    /// @brief
    /// @param
    /// @return
    void MakeTestCharacteristics(SCREEN_TYPE type,
                                 datamanagement::ModelData &model_data) {
        std::string prefix = "hiv_screening_";
        if (type == SCREEN_TYPE::kBackground) {
            prefix += "background";
        } else if (type == SCREEN_TYPE::kIntervention) {
            prefix += "intervention";
        } else {
            // error
        }

        // define test characteristics
        TestCharacteristics temp = {
            Utils::GetDoubleFromConfig(prefix + ".ab_sensitivity", dm),
            Utils::GetDoubleFromConfig(prefix + ".ab_specificity", dm),
            Utils::GetDoubleFromConfig(prefix + ".ab_cost", dm),
            Utils::GetDoubleFromConfig(prefix + ".rna_sensitivity", dm),
            Utils::GetDoubleFromConfig(prefix + ".rna_specificity", dm),
            Utils::GetDoubleFromConfig(prefix + ".rna_cost", dm)};
        test_data[type] = temp;
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // if Person already linked, skip screening
        if (person->GetLinkState(INF_TYPE) == person::LinkageState::kLinked) {
            return;
        }

        bool one_time_screen = (intervention_type == "one-time") &&
                               (person->GetCurrentTimestep() == 1);
        bool periodic_screen =
            (intervention_type == "periodic") &&
            (person->GetTimeSinceLastScreening(INF_TYPE) >= screening_period);

        if (one_time_screen || periodic_screen) {
            this->AttemptScreen(person, decider, SCREEN_TYPE::kIntervention);
        } else {
            this->AttemptScreen(person, decider);
        }
    }

    HIVScreeningIMPL(datamanagement::ModelData &model_data) {
        discount = Utils::GetDoubleFromConfig("cost.discounting_rate", dm);

        intervention_type =
            Utils::GetStringFromConfig("hiv_screening.intervention_type", dm);
        screening_period = Utils::GetIntFromConfig("hiv_screening.period", dm);

        // iterate through screening types, defining screening data
        for (int screen_type = 0;
             screen_type < static_cast<int>(SCREEN_TYPE::kCount);
             ++screen_type) {
            SCREEN_TYPE type = static_cast<SCREEN_TYPE>(screen_type);
            MakeTestCharacteristics(type, dm);
            LoadScreeningData(type, dm);
        }
    }
};

HIVScreening::HIVScreening(datamanagement::ModelData &model_data) {
    impl = std::make_unique<HIVScreeningIMPL>(dm);
}

HIVScreening::~HIVScreening() = default;
HIVScreening::HIVScreening(HIVScreening &&) noexcept = default;
HIVScreening &HIVScreening::operator=(HIVScreening &&) noexcept = default;

void HIVScreening::DoEvent(std::shared_ptr<person::PersonBase> person,
                           datamanagement::ModelData &model_data,
                           std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
