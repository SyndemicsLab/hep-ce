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
#include <DataManagement/DataManager.hpp>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>

namespace event {
    class Screening::ScreeningIMPL {
    private:
        std::shared_ptr<datamanagement::DataManager> dm;
        std::shared_ptr<stats::Decider> decider;

        static int callback(void *storage, int count, char **data,
                            char **columns) {
            std::vector<double> *d = (std::vector<double> *)storage;
            double temp = std::stod(data[0]);
            d->push_back(temp);
            return 0;
        }

        std::string buildSQL(person::PersonBase &person,
                             std::string column) const {
            int age_years = person.GetAge() / 12.0; // intentional truncation
            std::stringstream sql;
            sql << "SELECT " + column;
            sql << " FROM antibody_testing ";
            sql << " INNER JOIN screening_and_linkage ";
            sql << " ON ((antibody_testing.age_years = "
                   "screening_and_linkage.age_years) ";
            sql << " AND (antibody_testing.drug_behavior = "
                   "screening_and_linkage.drug_behavior)) ";
            sql << " WHERE antibody_testing.age_years = '"
                << std::to_string(age_years) << "'";
            sql << " AND screening_and_linkage.gender = '" << person.GetSex()
                << "'";
            sql << " AND screening_and_linkage.drug_behavior = '"
                << person.GetBehavior() << "';";
            return sql.str();
        }

        /// @brief The Background Screening Event Undertaken on a Person
        /// @param person The Person undergoing a background Screening
        void backgroundScreen(person::PersonBase &person) {
            if (!person.GetTimeSinceScreened()) {
                return;
            }
            person.MarkScreened();

            std::string testPrefix = "screening_background_";
            person::HCV infectionStatus = person.GetHCV();
            bool firstTest =
                this->test(infectionStatus, testPrefix + "ab",
                           [&person]() -> int { return person.AddAbScreen(); });
            this->insertScreeningCost(person, "screening_background_ab.cost",
                                      "Background Antibody Screening");

            // if first test is negative, perform a second test
            bool secondTest = false;
            if (!firstTest) {
                secondTest = this->test(
                    infectionStatus, testPrefix + "ab",
                    [&person]() -> int { return person.AddAbScreen(); });
                this->insertScreeningCost(person,
                                          "screening_background_ab.cost",
                                          "Background Antibody Screening");
            }

            if (!firstTest && !secondTest) {
                return; // run two tests and if both are negative do nothing
            }

            // if either is positive then...
            this->insertScreeningCost(person, "screening_background_rna.cost",
                                      "Background RNA Screening");

            if (this->test(
                    infectionStatus, testPrefix + "rna",
                    [&person]() -> int { return person.AddRnaScreen(); })) {
                person.Link(person::LinkageType::BACKGROUND);
                // what else needs to happen during a link?
            }

            person.Unlink();
        }

        bool runABTest(person::PersonBase &person, std::string prefix) {
            bool firstTest =
                this->test(person.GetHCV(), prefix + "_ab",
                           [&person]() -> int { return person.AddAbScreen(); });
            this->insertScreeningCost(person, prefix + "_ab.cost",
                                      "Intervention Antibody Screening");
            return firstTest;
        }

        bool runRNATest(person::PersonBase &person, std::string prefix) {
            bool firstTest = this->test(
                person.GetHCV(), prefix + "_rna",
                [&person]() -> int { return person.AddRnaScreen(); });
            this->insertScreeningCost(person, prefix + "_rna.cost",
                                      "Intervention RNA Screening");
            return firstTest;
        }

        /// @brief The Intervention Screening Event Undertaken on a Person
        /// @param person The Person undergoing an Intervention Screening
        void interventionScreen(person::PersonBase &person) {
            person.MarkScreened();
            if (!person.IsIdentifiedAsInfected()) {
                bool firstTest = runABTest(person, "screening_intervention");
                // if first test is negative, perform a second test
                if (!firstTest) {
                    bool secondTest =
                        runABTest(person, "screening_intervention");
                    if (!secondTest) {
                        return; // two negatives
                    }
                }
            }

            bool rna_test = runRNATest(person, "screening_intervention");
            if (rna_test) {
                person.Link(person::LinkageType::INTERVENTION);
                // what else needs to happen during a link?
            } else {
                person.Unlink();
            }
        }

        bool test(person::HCV infectionStatus, std::string configKey,
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
            // probability is the chance of false positive or false negative
            int value = decider->GetDecision({probability});
            testFunc();
            return value;
        }

        std::vector<double>
        getBackgroundScreeningProbability(person::PersonBase &person) {
            std::string query =
                this->buildSQL(person, "background_screen_probability");
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback, &storage,
                                              error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting background_screen_probability "
                    "Screening Data from antibody_testing and "
                    "screening_and_linkage! Error Message: {}",
                    error);
                return {};
            }
            double prob = storage[0];

            std::string configStr =
                "screening.seropositive_multiplier_not_boomer";

            if (person.IsBoomer()) {
                configStr = "screening.seropositive_multiplier_boomer";
            }

            std::string data;
            dm->GetFromConfig(configStr, data);
            double multiplier = (data.empty()) ? 1.0 : std::stod(data);
            prob *= multiplier;
            std::vector<double> result = {prob, 1 - prob};
            return result;
        }

        std::vector<double>
        getInterventionScreeningProbability(person::PersonBase &person) {
            std::string query =
                this->buildSQL(person, "intervention_screen_probability");
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback, &storage,
                                              error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting intervention_screen_probability "
                    "Screening Data from antibody_testing and "
                    "screening_and_linkage! Error Message: {}",
                    error);
                return {};
            }
            std::vector<double> result;
            if (storage.empty()) {
                spdlog::get("main")->warn(
                    "Callback Function Returned Empty Dataset From Query: "
                    "{}",
                    query);
                result = {0.0, 0.0};
            } else {
                result = {storage[0], 1 - storage[0]};
            }
            return result;
        }

        void interventionDecision(person::PersonBase &person) {
            std::vector<double> interventionProbability =
                this->getInterventionScreeningProbability(person);
            if (decider->GetDecision(interventionProbability) == 0) {
                this->interventionScreen(person);
            }
        }

        /// @brief Insert cost for screening of type \code{type}
        /// @param person The person who is accruing cost
        /// @param type The screening type, used to discern the cost to add
        void insertScreeningCost(person::PersonBase &person,
                                 std::string configKey,
                                 std::string screeningName) {
            double screeningCost;
            std::string data;
            dm->GetFromConfig(configKey, data);
            screeningCost = std::stod(data);

            cost::Cost cost = {cost::CostCategory::SCREENING, screeningName,
                               screeningCost};
            person.AddCost(cost);
        }

    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) {
            this->dm = dm;
            this->decider = decider;
            std::string interventionType;
            dm->GetFromConfig("screening.intervention_type", interventionType);
            int interventionPeriod = 0;
            if (interventionType == "periodic") {
                std::string temp;
                dm->GetFromConfig("screening.period", temp);
                interventionPeriod = std::stoi(temp);
                if (person.GetTimeSinceScreened() >= interventionPeriod ||
                    person.GetTimeOfLastScreening() == -1) {
                    this->interventionDecision(person);
                }
            }
            if (interventionType == "one-time" &&
                person.GetCurrentTimestep() == 1) {
                this->interventionDecision(person);
            } else if (interventionType == "periodic") {
                if (person.GetTimeSinceScreened() > interventionPeriod) {
                    this->interventionDecision(person);
                } else {
                    return; // Do not screen this time
                }

            } else {
                spdlog::get("main")->warn(
                    "Intervention Type not valid During Screening "
                    "Event");
                return;
            }

            // [screen, don't screen]
            std::vector<double> backgroundProbability =
                this->getBackgroundScreeningProbability(person);
            int choice = decider->GetDecision(backgroundProbability);
            if (choice == 0) {
                this->backgroundScreen(person);
            }
        }
    };

    void Screening::doEvent(person::PersonBase &person) {
        impl->doEvent(person, dm, decider);
    }
    Screening::Screening(std::shared_ptr<stats::Decider> decider,
                         std::shared_ptr<datamanagement::DataManager> dm,
                         std::string name)
        : Event(dm, name), decider(decider) {
        impl = std::make_unique<ScreeningIMPL>();
    }
} // namespace event
