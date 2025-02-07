//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of the Pregnancy Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Pregnancy.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class Pregnancy::PregnancyIMPL {
    private:
        double multiple_delivery_probability;
        double infant_hcv_tested_probability;
        double vertical_hcv_transition_probability;

        static int callback(void *storage, int count, char **data,
                            char **columns) {
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

        bool
        CheckMiscarriage(std::shared_ptr<person::PersonBase> person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         std::shared_ptr<stats::DeciderBase> decider) {
            std::vector<double> prob =
                this->GetSingleProbability(person, dm, "miscarriage");
            return (!decider->GetDecision(prob)) ? true : false;
        }

        std::vector<double> GetSingleProbability(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm,
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

        int
        GetNumberOfBirths(std::shared_ptr<person::PersonBase> person,
                          std::shared_ptr<datamanagement::DataManagerBase> dm,
                          std::shared_ptr<stats::DeciderBase> decider) {

            std::vector<double> result = {multiple_delivery_probability,
                                          1 - multiple_delivery_probability};
            // Currently only deciding between single birth or twins
            return (decider->GetDecision(result) == 0) ? 2 : 1;
        }

        bool
        DoChildrenGetTested(std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {

            std::vector<double> result = {infant_hcv_tested_probability,
                                          1 - infant_hcv_tested_probability};
            return (decider->GetDecision(result) == 0) ? true : false;
        }

        bool
        DrawChildInfection(std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::shared_ptr<stats::DeciderBase> decider) {
            std::vector<double> result = {
                vertical_hcv_transition_probability,
                1 - vertical_hcv_transition_probability};
            return (decider->GetDecision(result) == 0) ? true : false;
        }

        void
        AttemptHaveChild(std::shared_ptr<person::PersonBase> person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         std::shared_ptr<stats::DeciderBase> decider) {
            if (CheckMiscarriage(person, dm, decider)) {
                person->Stillbirth();
                return;
            }

            int numberOfBirths = GetNumberOfBirths(person, dm, decider);

            if (person->GetHCV() != person::HCV::CHRONIC) {
                for (int child = 0; child < numberOfBirths; ++child) {
                    person->AddChild(person::HCV::NONE, false);
                }
                return;
            }

            bool tested = DoChildrenGetTested(dm, decider);
            for (int child = 0; child < numberOfBirths; ++child) {
                if (DrawChildInfection(dm, decider)) {
                    person->AddChild(person::HCV::CHRONIC, tested);
                } else {
                    person->AddChild(person::HCV::NONE, tested);
                }
            }
        }

        void
        AttemptHealthyMonth(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {
            if (CheckMiscarriage(person, dm, decider)) {
                person->Miscarry();
            }
        }

    public:
        void DoEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {

            if (person->GetSex() == person::Sex::MALE ||
                person->GetAge() < 180 || person->GetAge() > 540 ||
                (person->GetPregnancyState() ==
                     person::PregnancyState::POSTPARTUM &&
                 person->GetTimeSincePregnancyChange() < 3)) {
                return;
            }

            if (person->GetPregnancyState() ==
                person::PregnancyState::POSTPARTUM) {
                person->EndPostpartum();
            }

            if (person->GetPregnancyState() ==
                person::PregnancyState::PREGNANT) {
                if (person->GetTimeSincePregnancyChange() >= 9) {
                    AttemptHaveChild(person, dm, decider);
                } else {
                    AttemptHealthyMonth(person, dm, decider);
                }
            } else {
                std::vector<double> prob = this->GetSingleProbability(
                    person, dm, "pregnancy_probability");
                // if you are pregnant (i.e. getDecision returns 0)
                if (decider->GetDecision(prob) == 0) {
                    person->Impregnate();
                }
            }
            return;
        }

        PregnancyIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string storage;
            dm->GetFromConfig("pregnancy.multiple_delivery_probability",
                              storage);
            if (storage.empty()) {
                spdlog::get("main")->warn(
                    "No Multiple Delivery Probability Found! Assuming 0.");
                storage = "0";
            }
            multiple_delivery_probability = Utils::stod_positive(storage);

            storage.clear();
            dm->GetFromConfig("pregnancy.infant_hcv_tested_probability",
                              storage);
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

    Pregnancy::Pregnancy(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<PregnancyIMPL>(dm);
    }

    Pregnancy::~Pregnancy() = default;
    Pregnancy::Pregnancy(Pregnancy &&) noexcept = default;
    Pregnancy &Pregnancy::operator=(Pregnancy &&) noexcept = default;

    void Pregnancy::DoEvent(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {
        impl->DoEvent(person, dm, decider);
    }

} // namespace event
