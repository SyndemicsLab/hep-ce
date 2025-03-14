////////////////////////////////////////////////////////////////////////////////
// File: HIVScreening.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-06                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-14                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVScreening.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <string>
#include <unordered_map>

namespace event {
class HIVScreening::HIVScreeningIMPL {
private:
    // constants
    // two types of screening - background and intervention
    const int TYPE_COUNT = 2;
    enum class SCREEN_TYPE { BACKGROUND = 0, INTERVENTION = 1 };
    person::InfectionType it = person::InfectionType::HIV;
    const std::unordered_map<SCREEN_TYPE, std::string> TYPE_COLUMNS = {
        {SCREEN_TYPE::BACKGROUND, "hiv_background_screen_probability"},
        {SCREEN_TYPE::INTERVENTION, "hiv_intervention_screen_probability"}};

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
        std::unordered_map<Utils::tuple3i, double, Utils::key_hash_3i,
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
    /// @return
    double
    GetDoubleFromConfig(std::string config_key,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        bool positive = true) {
        std::string config_data;
        dm->GetFromConfig(config_key, config_data);
        if (config_data.empty()) {
            spdlog::get("main")->warn("No {} Found!", config_key);
            config_data = "0.0";
        }
        if (positive) {
            return Utils::stod_positive(config_data);
        }
        return static_cast<double>(config_data);
    }

    std::string
    GetStringFromConfig(std::string config_key,
                        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string config_data;
        dm->GetFromConfig(config_key, config_data);
        if (config_data.empty()) {
            spdlog::get("main")->warn("No {} Found!", config_key);
        }
        return config_data;
    }

    /// @brief
    /// @param
    /// @return A reference to the map used to store data for the screen type
    hivscreenmap_t &PickMap(SCREEN_TYPE type) {
        switch (type) {
        case SCREEN_TYPE::BACKGROUND:
            return &hiv_background_screen_data;
        case SCREEN_TYPE::INTERVENTION:
            return &hiv_intervention_screen_data;
        }
    }

    /// @brief
    /// @param
    void
    LoadScreeningData(SCREEN_TYPE type,
                      std::shared_ptr<datamanagement::DataManagerBase> dm) {
        hivscreenmap_t &chosen_screenmap = PickMap(type);
        std::string column = TYPE_COLUMNS.at(type);
        std::string error;
        int rc = dm->SelectCustomCallback(HIVScreenSQL(column),
                                          this->callback_hivscreen,
                                          chosen_screenmap, error);
        if (rc != 0) {
            std::string message =
                "Error retrieving Screening values for column `" + column
                "'! Error Message: {}";
            spdlog::get("main")->error(message, error);
        }
        if ((*chosen_screenmap).empty()) {
            spdlog::get("main")->warn("No " + column + " found.");
        }
    }

    /// @brief
    /// @param
    /// @return
    double
    GetScreeningProbability(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            SCREEN_TYPE type) {
        int age_years = static_cast<int>(person->GetAge() / 12.0);
        int gender = static_cast<int>(person->GetSex());
        int drug_behavior = static_cast<int>(person->GetBehavior());
        Utils::tuple_3i tup = std::make_tupe(age_years, gender, drug_behavior);

        double probability = 0.0;
        switch (type) {
        case SCREEN_TYPE::BACKGROUND:
            probability = hiv_background_screen_data[tup];
            break;
        case SCREEN_TYPE::INTERVENTION:
            probability = hiv_intervention_screen_data[tup];
            break;
        }
        return probability;
    }

    /// @brief
    /// @param
    bool RunABScreen(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider,
                     SCREEN_TYPE type) {}

    /// @brief
    /// @param
    void AttemptScreen(std::shared_ptr<person::PersonBase> person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       std::shared_ptr<stats::DeciderBase> decider,
                       SCREEN_TYPE type = SCREEN_TYPE::BACKGROUND) {
        double screen_probability =
            this->GetScreeningProbability(person, dm, type);
        // do not screen if the decision does not evaluate to 0
        if (decider->GetDecision({screen_probability}) != 0) {
            return;
        }

        // mark that person has been HIV screened on the current timestep
        person->MarkScreened(it);

        // if not already tested positive for antibody, HIV antibody screen
        if (!person->CheckAntibodyPositive(it)) {
            // add antibody screening to person
            person->AddAbScreen(it);
            // stop screening if person isn't found antibody positive
            if (!RunABScreen(person, dm, decider, type)) {
                return;
            }
            // mark positive otherwise
            person->SetAntibodyPositive(true);
        }

        // if positive for HIV antibody, HIV rna screen
        if (person->CheckAntibodyPositive(it)) {
            if (!RunRNAScreen(person, dm, decider, type)) {
                return;
            }
            // if rna positive, mark hiv identified
            // STILL IN PROGRESS
            // person->Diagnose(it);
        }
    }

    void MakeTestCharacteristics(SCREEN_TYPE type) {
        std::string prefix = "hiv_screening_";
        if (type == SCREEN_TYPE::BACKGROUND) {
            prefix += "background";
        } else if {
            prefix += "intervention";
        } else {
            // error
        }

        // define test characteristics
        TestCharacteristics temp = {
            GetDoubleFromConfig(prefix + ".ab_sensitivity", dm),
            GetDoubleFromConfig(prefix + ".ab_specificity", dm),
            GetDoubleFromConfig(prefix + ".ab_cost", dm),
            GetDoubleFromConfig(prefix + ".rna_sensitivity", dm),
            GetDoubleFromConfig(prefix + ".rna_specificity", dm),
            GetDoubleFromConfig(prefix + ".rna_cost", dm)};
        test_data[type] = temp;
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // if Person already linked, skip screening
        if (person->GetLinkState(it) == person::LinkageState::LINKED) {
            return;
        }

        bool one_time_screen = (intervention_type == "one-time") &&
                               (person->GetCurrentTimestep() == 1);
        bool periodic_screen =
            (intervention_type == "periodic") &&
            (person->GetTimeSinceLastScreening() >= screening_period);
        if (one_time_screen || periodic_screen) {
            this->AttemptScreen(person, dm, decider, SCREEN_TYPE::INTERVENTION);
        } else {
            this->AttemptScreen(person, dm, decider);
        }
    }

    HIVScreeningIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        discount = GetDoubleFromConfig("cost.discounting_rate", dm);

        intervention_type =
            GetStringFromConfig("hiv_screening.intervention_type");
        std::string temp = GetStringFromConfig("hiv_screening.period");
        period = temp.empty() ? 0 : std::stoi(temp);

        // iterate through screening types, defining screening data
        for (int screen_type = 0; i < TYPE_COUNT; ++screen_type) {
            SCREEN_TYPE type = static_cast<SCREEN_TYPE>(screen_type);
            MakeTestCharacteristics(type);
            LoadScreeningData(type);
        }
    }
};
} // namespace event
