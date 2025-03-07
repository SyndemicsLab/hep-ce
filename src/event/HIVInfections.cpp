////////////////////////////////////////////////////////////////////////////////
// File: HIVInfections.cpp                                                    //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-02-28                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVInfections.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
class HIVInfections::HIVInfectionsIMPL {
private:
    // HIV incidence is stratified by age (years), gender, drug use behavior
    // key is a tuple of strata and value is the hiv incidence probability
    using hivincidencemap_t =
        std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                           Utils::key_equal_3i>;
    hivincidencemap_t incidence_data;
    static int callback_hivincidence(void *storage, int count, char **data,
                                     char **columns) {
        Utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((hivincidencemap_t *)storage))[tup] = Utils::stod_positive(data[3]);
        return 0;
    }

    std::string HIVIncidenceSQL() {
        return "SELECT age_years, gender, drug_behavior, incidence FROM "
               "hiv_incidence;";
    }

    std::vector<double> GetInfectionProbability(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        if (incidence_data.empty()) {
            spdlog::get("main")->warn(
                "No result found for HIV Infection Probability!");
            return {0.0};
        }

        int age_years = (int)(person->GetAge() / 12.0);
        int gender = (int)person->GetSex();
        int drug_behavior = (int)person->GetBehavior();
        Utils::tuple_3i tup = std::make_tuple(age_years, gender, drug_behavior);
        double incidence = incidence_data[tup];

        return {incidence};
    }

    int LoadIncidenceData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string error;
        int rc = dm->SelectCustomCallback(HIVIncidenceSQL(),
                                          this->callback_hivincidence,
                                          &incidence_data, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "Error retrieving Infection Probability! Error Message: "
                "{}",
                error);
        }
        if (incidence_data.empty()) {
            spdlog::get("main")->warn(
                "No result found for HIV Infection Probability.");
        }
        return rc;
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // If already infected, exit immediately
        if (person->GetHIV() != person::HIV::NONE) {
            return;
        }

        // Get the probability of infection
        std::vector<double> prob = this->GetInfectionProbability(person, dm);
        // Decide whether person is infected; if value == 0, infect
        if (decider->GetDecision(prob) == 0) {
            person->InfectHIV();
        }
    }

    HIVInfectionsIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        int rc = LoadIncidenceData(dm);
    }
};

HIVInfections::HIVInfections(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<HIVInfectionsIMPL>(dm);
}

HIVInfections::~HIVInfections() = default;
HIVInfections::HIVInfections(HIVInfections &&) noexcept = default;
HIVInfections &HIVInfections::operator=(HIVInfections &&) noexcept = default;

void HIVInfections::DoEvent(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
