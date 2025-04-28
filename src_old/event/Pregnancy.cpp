////////////////////////////////////////////////////////////////////////////////
// File: Pregnancy.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2024-06-13                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2024-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Pregnancy.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <datamanagement/datamanagement.hpp>
#include <sstream>

namespace event {
class Pregnancy::PregnancyIMPL {
private:
    double multiple_delivery_probability;
    double infant_hcv_tested_probability;
    double vertical_hcv_transition_probability;

    static int callback(void *storage, int count, char **data, char **columns) {
        double *d = (double *)storage;
        *d = Utils::stod_positive(data[0]); // First Column Selected
        return 0;
    }
    std::string buildSQL(std::shared_ptr<person::PersonBase> person,
                         std::string column) {
        std::stringstream sql;
        int age_years = (int)(person->GetAge() / 12.0);
        sql << "SELECT " + column + " FROM pregnancy";
        sql << " WHERE age_years = " << age_years << ";";
        return sql.str();
    }

    bool CheckMiscarriage(std::shared_ptr<person::PersonBase> person,
                          datamanagement::ModelData &model_data,
                          std::shared_ptr<stats::DeciderBase> decider) {
        std::vector<double> prob =
            this->GetSingleProbability(person, dm, "miscarriage");
        return (!decider->GetDecision(prob)) ? true : false;
    }

    std::vector<double>
    GetSingleProbability(std::shared_ptr<person::PersonBase> person,
                         datamanagement::ModelData &model_data,
                         std::string column) {
        std::string query = this->buildSQL(person, column);
        double storage = 0.0;
        std::string error;
        int rc = dm->SelectCustomCallback(query, callback, &storage, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "No probabilities avaliable for Pregnancy!");
            return {};
        }
        std::vector<double> result = {storage, 1 - storage};
        return result;
    }

    int GetNumberOfBirths(std::shared_ptr<person::PersonBase> person,
                          datamanagement::ModelData &model_data,
                          std::shared_ptr<stats::DeciderBase> decider) {

        std::vector<double> result = {multiple_delivery_probability,
                                      1 - multiple_delivery_probability};
        // Currently only deciding between single birth or twins
        return (decider->GetDecision(result) == 0) ? 2 : 1;
    }

    bool DoChildrenGetTested(datamanagement::ModelData &model_data,
                             std::shared_ptr<stats::DeciderBase> decider) {

        std::vector<double> result = {infant_hcv_tested_probability,
                                      1 - infant_hcv_tested_probability};
        return (decider->GetDecision(result) == 0) ? true : false;
    }

    bool DrawChildInfection(datamanagement::ModelData &model_data,
                            std::shared_ptr<stats::DeciderBase> decider) {
        std::vector<double> result = {vertical_hcv_transition_probability,
                                      1 - vertical_hcv_transition_probability};
        return (decider->GetDecision(result) == 0) ? true : false;
    }

    void AttemptHaveChild(std::shared_ptr<person::PersonBase> person,
                          datamanagement::ModelData &model_data,
                          std::shared_ptr<stats::DeciderBase> decider) {
        if (CheckMiscarriage(person, dm, decider)) {
            person->Stillbirth();
            return;
        }

        int numberOfBirths = GetNumberOfBirths(person, dm, decider);

        if (person->GetHCV() != person::HCV::kChronic) {
            for (int child = 0; child < numberOfBirths; ++child) {
                person->AddChild(person::HCV::kNone, false);
            }
            person->SetPregnancyState(person::PregnancyState::POSTPARTUM);
            return;
        }

        bool tested = DoChildrenGetTested(dm, decider);
        for (int child = 0; child < numberOfBirths; ++child) {
            person->AddInfantExposure();
            if (DrawChildInfection(dm, decider)) {
                person->AddChild(person::HCV::kChronic, tested);
            } else {
                person->AddChild(person::HCV::kNone, tested);
            }
        }
        // after giving birth, set postpartum
        person->SetPregnancyState(person::PregnancyState::POSTPARTUM);
    }

    void AttemptHealthyMonth(std::shared_ptr<person::PersonBase> person,
                             datamanagement::ModelData &model_data,
                             std::shared_ptr<stats::DeciderBase> decider) {
        if (CheckMiscarriage(person, dm, decider)) {
            person->Miscarry();
        }
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) {

        if (person->GetSex() == person::Sex::kMale || person->GetAge() < 180 ||
            person->GetAge() > 540 ||
            (person->GetPregnancyState() ==
                 person::PregnancyState::kPostpartum &&
             person->GetTimeSincePregnancyChange() < 3)) {
            return;
        }

        if (person->GetPregnancyState() ==
            person::PregnancyState::kPostpartum) {
            person->EndPostpartum();
        }

        if (person->GetPregnancyState() == person::PregnancyState::kPregnant) {
            if (person->GetTimeSincePregnancyChange() >= 9) {
                AttemptHaveChild(person, dm, decider);
            } else {
                AttemptHealthyMonth(person, dm, decider);
            }
        } else {
            std::vector<double> prob =
                this->GetSingleProbability(person, dm, "pregnancy_probability");
            // if you are pregnant (i.e. getDecision returns 0)
            if (decider->GetDecision(prob) == 0) {
                person->Impregnate();
            }
        }
        return;
    }

    PregnancyIMPL(datamanagement::ModelData &model_data) {
        std::string storage;
        dm->GetFromConfig("pregnancy.multiple_delivery_probability", storage);
        if (storage.empty()) {
            spdlog::get("main")->warn(
                "No Multiple Delivery Probability Found! Assuming 0.");
            storage = "0";
        }
        multiple_delivery_probability = Utils::stod_positive(storage);

        storage.clear();
        dm->GetFromConfig("pregnancy.infant_hcv_tested_probability", storage);
        if (storage.empty()) {
            spdlog::get("main")->warn(
                "No Infant HCV Testing Probability Found! Assuming 0.");
            storage = "0";
        }
        infant_hcv_tested_probability = Utils::stod_positive(storage);

        storage.clear();
        dm->GetFromConfig("pregnancy.vertical_hcv_transition_probability",
                          storage);
        if (storage.empty()) {
            spdlog::get("main")->warn("No Infant HCV Vertical Transmission "
                                      "Probability Found! Assuming 0.");
            storage = "0";
        }
        vertical_hcv_transition_probability = Utils::stod_positive(storage);
    }
};

Pregnancy::Pregnancy(datamanagement::ModelData &model_data) {
    impl = std::make_unique<PregnancyIMPL>(dm);
}

Pregnancy::~Pregnancy() = default;
Pregnancy::Pregnancy(Pregnancy &&) noexcept = default;
Pregnancy &Pregnancy::operator=(Pregnancy &&) noexcept = default;

void Pregnancy::DoEvent(std::shared_ptr<person::PersonBase> person,
                        datamanagement::ModelData &model_data,
                        std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}

} // namespace event
