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
/// This file contains the implementation of the Infections Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
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
        typedef std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                                   Utils::key_equal_3i>
            incidencemap_t;
        incidencemap_t incidence_data;
        static int callback_incidence(void *storage, int count, char **data,
                                      char **columns) {
            Utils::tuple_3i tup = std::make_tuple(
                std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
            (*((incidencemap_t *)storage))[tup] = std::stod(data[3]);
            return 0;
        }

        std::string IncidenceSQL() {
            return "SELECT age_years, gender, drug_behavior, incidence FROM "
                   "incidence;";
        }

        std::vector<double>
        getInfectProb(std::shared_ptr<person::PersonBase> person,
                      std::shared_ptr<datamanagement::DataManagerBase> dm) {
            if (incidence_data.empty()) {
                spdlog::get("main")->warn(
                    "No result found for Infection Probability!");
                return {0.0, 1.0};
            }

            int age_years = (int)(person->GetAge() / 12.0);
            int gender = (int)person->GetSex();
            int drug_behavior = (int)person->GetBehavior();
            Utils::tuple_3i tup =
                std::make_tuple(age_years, gender, drug_behavior);
            double incidence = incidence_data[tup];

            return {incidence, 1 - incidence};
        }

        int
        LoadIncidenceData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string error;
            int rc = dm->SelectCustomCallback(IncidenceSQL(),
                                              this->callback_incidence,
                                              &incidence_data, error);
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
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            // only those who aren't infected go to the rest of the event.
            // those who are infected transition from acute to chronic after 6
            // months.
            if (person->GetHCV() != person::HCV::NONE) {
                if (person->GetHCV() == person::HCV::ACUTE &&
                    person->GetTimeSinceHCVChanged() >= 6) {
                    person->SetHCV(person::HCV::CHRONIC);
                }
                return;
            }

            // draw new infection probability
            std::vector<double> prob = this->getInfectProb(person, dm);
            // decide whether person is infected; if value == 0, infect
            if (decider->GetDecision(prob) != 0) {
                return;
            }
            person->InfectHCV();
        }

        InfectionsIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            int rc = LoadIncidenceData(dm);
        }
    };
    Infections::Infections(
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<InfectionsIMPL>(dm);
    }

    Infections::~Infections() = default;
    Infections::Infections(Infections &&) noexcept = default;
    Infections &Infections::operator=(Infections &&) noexcept = default;

    void
    Infections::doEvent(std::shared_ptr<person::PersonBase> person,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }

} // namespace event
