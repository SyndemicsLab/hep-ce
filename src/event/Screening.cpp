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
/// This file contains the implementation of the Screening Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Screening.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace event {
    class Screening::ScreeningIMPL {
    private:
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            std::vector<double> *d = (std::vector<double> *)storage;
            double temp = std::stod(data[0]);
            d->push_back(temp);
            return 0;
        }

        std::string buildSQL(std::shared_ptr<person::PersonBase> person,
                             std::string column) const {
            int age_years = person->GetAge() / 12.0; // intentional truncation
            std::stringstream sql;
            sql << "SELECT " + column;
            sql << " FROM antibody_testing ";
            sql << " INNER JOIN screening_and_linkage ";
            sql << " ON ((antibody_testing.age_years = "
                   "screening_and_linkage.age_years) ";
            sql << " AND (antibody_testing.drug_behavior = "
                   "screening_and_linkage.drug_behavior)) ";
            sql << " WHERE antibody_testing.age_years = " << age_years;
            sql << " AND screening_and_linkage.gender = "
                << ((int)person->GetSex());
            sql << " AND screening_and_linkage.drug_behavior = "
                << ((int)person->GetBehavior()) << ";";
            return sql.str();
        }

        bool runABTest(std::shared_ptr<person::PersonBase> person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       std::string prefix,
                       std::shared_ptr<stats::DeciderBase> decider) {
            bool test = this->test(
                person->GetHCV(), dm, prefix + "_ab", decider,
                [&person]() -> int { return person->AddAbScreen(); });
            this->insertScreeningCost(person, dm, prefix + "_ab.cost",
                                      "Antibody Screening");
            return test;
        }

        bool runRNATest(std::shared_ptr<person::PersonBase> person,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        std::string prefix,
                        std::shared_ptr<stats::DeciderBase> decider) {
            bool test = this->test(
                person->GetHCV(), dm, prefix + "_rna", decider,
                [&person]() -> int { return person->AddRnaScreen(); });
            this->insertScreeningCost(person, dm, prefix + "_rna.cost",
                                      "RNA Screening");
            return test;
        }

        bool test(person::HCV infectionStatus,
                  std::shared_ptr<datamanagement::DataManagerBase> dm,
                  std::string configKey,
                  std::shared_ptr<stats::DeciderBase> decider,
                  std::function<int(void)> testFunc) {
            double probability = 0.5;
            std::string data;
            if (infectionStatus == person::HCV::ACUTE) {
                dm->GetFromConfig(configKey + ".acute_sensitivity", data);
                probability = 1 - std::stod(data);
            } else if (infectionStatus == person::HCV::CHRONIC) {
                dm->GetFromConfig(configKey + ".chronic_sensitivity", data);
                probability = 1 - std::stod(data);
            } else {
                dm->GetFromConfig(configKey + ".specificity", data);
                probability = std::stod(data);
            }
            testFunc();
            // probability is the chance of false positive or false negative
            bool rValue =
                (decider->GetDecision({probability, 1 - probability}) == 0)
                    ? true
                    : false;
            return rValue;
        }

        /// @brief The Intervention Screening Event Undertaken on a Person
        /// @param person The Person undergoing an Intervention Screening
        void Screen(std::string screenkey,
                    std::shared_ptr<person::PersonBase> person,
                    std::shared_ptr<datamanagement::DataManagerBase> dm,
                    std::shared_ptr<stats::DeciderBase> decider) {
            person->MarkScreened();
            if ((screenkey == "screening_intervention" &&
                 !person->IsIdentifiedAsHCVInfected()) ||
                screenkey == "screening_background") {
                // if first test is negative, perform a second test
                if (!runABTest(person, dm, screenkey, decider)) {
                    if (!runABTest(person, dm, screenkey, decider)) {
                        return; // two negatives
                    }
                }
            }

            if (runRNATest(person, dm, screenkey, decider)) {
                person::LinkageType type =
                    (screenkey == "screening_background")
                        ? person::LinkageType::BACKGROUND
                        : person::LinkageType::INTERVENTION;
                person->Link(type);
            } else {
                person->Unlink();
            }
        }

        std::vector<double> GetScreeningProbability(
            std::string colname, std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string query = this->buildSQL(person, colname);
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback, &storage,
                                              error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting {} "
                    "Screening Data from antibody_testing and "
                    "screening_and_linkage! Error Message: {}",
                    colname, error);
                return {};
            }
            std::vector<double> probs;
            if (storage.empty()) {
                spdlog::get("main")->warn(
                    "Callback Function Returned Empty Dataset From Query: "
                    "{}",
                    query);
                probs = {0.0};
            } else {
                probs = {storage[0]};
            }

            std::string configStr =
                "screening.seropositive_multiplier_not_boomer";

            if (person->IsBoomer()) {
                configStr = "screening.seropositive_multiplier_boomer";
            }

            std::string data;
            dm->GetFromConfig(configStr, data);
            double multiplier = (data.empty()) ? 1.0 : std::stod(data);
            probs[0] *= multiplier;
            std::vector<double> result = {probs[0], 1 - probs[0]};
            return result;
        }

        void interventionDecision(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm,
            std::shared_ptr<stats::DeciderBase> decider) {
            std::vector<double> interventionProbability =
                this->GetScreeningProbability("intervention_screen_probability",
                                              person, dm);
            if (decider->GetDecision(interventionProbability) == 0) {
                this->Screen("screening_intervention", person, dm, decider);
            }
        }

        /// @brief Insert cost for screening of type \code{type}
        /// @param person The person who is accruing cost
        /// @param type The screening type, used to discern the cost to add
        void
        insertScreeningCost(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::string configKey, std::string screeningName) {
            double screeningCost;
            std::string data;
            dm->GetFromConfig(configKey, data);
            screeningCost = std::stod(data);

            cost::Cost cost = {cost::CostCategory::SCREENING, screeningName,
                               screeningCost};
            person->AddCost(cost);
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            std::string temp;
            dm->GetFromConfig("screening.period", temp);
            int interventionPeriod = (temp.empty()) ? 0 : std::stoi(temp);

            std::string interventionType;
            dm->GetFromConfig("screening.intervention_type", interventionType);
            /// Intervention Screening Conditions:
            /// 1. Have InterventionType is One-Time and CurrentTimestep
            ///         is 1
            /// 2. Have Intervention Type is Periodic and the person
            ///         hasn't screened
            ///         or the period has been reached since their last
            ///         screening
            if ((interventionType == "one-time" &&
                 person->GetCurrentTimestep() == 1) ||
                (interventionType == "periodic" &&
                 (person->GetTimeSinceLastScreening() >= interventionPeriod ||
                  person->GetTimeOfLastScreening() == -1))) {
                this->interventionDecision(person, dm, decider);
            }

            // Background Screening:
            std::vector<double> backgroundProbability =
                this->GetScreeningProbability("background_screen_probability",
                                              person, dm);
            if (decider->GetDecision(backgroundProbability) == 0 &&
                person->GetTimeSinceLastScreening() != 0) {
                this->Screen("screening_background", person, dm, decider);
            }
        }
    };

    Screening::Screening() { impl = std::make_unique<ScreeningIMPL>(); }

    Screening::~Screening() = default;
    Screening::Screening(Screening &&) noexcept = default;
    Screening &Screening::operator=(Screening &&) noexcept = default;

    void Screening::doEvent(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }

} // namespace event
