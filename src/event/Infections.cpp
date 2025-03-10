////////////////////////////////////////////////////////////////////////////////
// File: Infections.cpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Infections.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>
namespace event {
class Infections::InfectionsIMPL {
private:
    double gt3_prob = 0;

    using incidencemap_t =
        std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                           Utils::key_equal_3i>;
    incidencemap_t incidence_data;
    static int callback_incidence(void *storage, int count, char **data,
                                  char **columns) {
        Utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((incidencemap_t *)storage))[tup] = Utils::stod_positive(data[3]);
        return 0;
    }

    std::string IncidenceSQL() {
        return "SELECT age_years, gender, drug_behavior, incidence FROM "
               "incidence;";
    }

    std::vector<double> GetInfectionProbability(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        if (incidence_data.empty()) {
            spdlog::get("main")->warn(
                "No result found for Infection Probability!");
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
        int rc = dm->SelectCustomCallback(
            IncidenceSQL(), this->callback_incidence, &incidence_data, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "Error retrieving Infection Probability! Error Message: "
                "{}",
                error);
        }
        if (incidence_data.empty()) {
            spdlog::get("main")->warn(
                "No result found for Infection Probability.");
        }
        return rc;
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // Acute cases progress to chronic after 6 consecutive months of
        // infection
        if (person->GetHCV() == person::HCV::ACUTE &&
            person->GetTimeSinceHCVChanged() == 6) {
            person->SetHCV(person::HCV::CHRONIC);
        }

        // If already infected, skip
        if (person->GetHCV() != person::HCV::NONE) {
            return;
        }

        // draw new infection probability
        std::vector<double> prob = this->GetInfectionProbability(person, dm);
        // decide whether person is infected; if value == 0, infect
        if (decider->GetDecision(prob) == 0) {
            person->InfectHCV();
            // decide whether hcv is genotype three
            if (decider->GetDecision({gt3_prob}) == 0) {
                person->SetGenotypeThree(true);
            }
        }
    }

    InfectionsIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        int rc = LoadIncidenceData(dm);
        std::string data;
        dm->GetFromConfig("infection.genotype_three_prob", data);
        if (data.empty()) {
            spdlog::get("main")->warn(
                "[Infections] No probability of genotype three infection "
                "found. Using default value of {}",
                gt3_prob);
        } else {
            this->gt3_prob = Utils::stod_positive(data);
        }
    }
};
Infections::Infections(std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<InfectionsIMPL>(dm);
}

Infections::~Infections() = default;
Infections::Infections(Infections &&) noexcept = default;
Infections &Infections::operator=(Infections &&) noexcept = default;

void Infections::DoEvent(std::shared_ptr<person::PersonBase> person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}

} // namespace event
