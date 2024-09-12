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
#include "DataManager.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
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
            std::stringstream sql;
            sql << "SELECT " + column;
            sql << " FROM antibody_testing ";
            sql << "INNER JOIN screening_and_linkage ";
            sql << "ON ((antibody_testing.age_years = "
                   "screening_and_linkage.age_years) ";
            sql << "AND (antibody_testing.drug_behavior = "
                   "screening_and_linkage.drug_behavior)) ";
            sql << "WHERE age_years = " << std::to_string(person.GetAge());
            sql << " AND behavior_costs.gender = " << person.GetSex();
            sql << " AND behavior_costs.drug_behavior = "
                << person.GetBehavior();
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
                           [&person]() -> int { person.AddAbScreen(); });
            this->insertScreeningCost(person, "screening_background_ab.cost",
                                      "Background Antibody Screening");

            // if first test is negative, perform a second test
            bool secondTest = false;
            if (!firstTest) {
                secondTest =
                    this->test(infectionStatus, testPrefix + "ab",
                               [&person]() -> int { person.AddAbScreen(); });
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

            if (this->test(infectionStatus, testPrefix + "rna",
                           [&person]() -> int { person.AddRnaScreen(); })) {
                person.Link(person::LinkageType::BACKGROUND);
                // what else needs to happen during a link?
            }

            person.Unlink();
        }

        /// @brief The Intervention Screening Event Undertaken on a Person
        /// @param person The Person undergoing an Intervention Screening
        void interventionScreen(person::PersonBase &person) {
            person.MarkScreened();

            std::string testPrefix = "screening_intervention_";
            person::HCV infectionStatus = person.GetHCV();
            bool firstTest =
                this->test(infectionStatus, testPrefix + "ab",
                           [&person]() -> int { person.AddAbScreen(); });
            this->insertScreeningCost(person, "screening_intervention_ab.cost",
                                      "Intervention Antibody Screening");

            // if first test is negative, perform a second test
            bool secondTest = false;
            if (!firstTest) {
                infectionStatus = person.GetHCV();
                secondTest =
                    this->test(infectionStatus, testPrefix + "ab",
                               [&person]() -> int { person.AddAbScreen(); });
            }

            this->insertScreeningCost(person, "screening_intervention_rna.cost",
                                      "Intervention RNA Screening");
            infectionStatus = person.GetHCV();
            if (this->test(infectionStatus, testPrefix + "rna",
                           [&person]() -> int { person.AddRnaScreen(); })) {
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
            std::vector<double> result = {storage[0], 1 - storage[0]};
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
                if (interventionPeriod <= 0) {
                    // log error
                }
            }
            if (interventionType == "one-time" &&
                person.GetCurrentTimestep() == 1) {
                this->interventionDecision(person);
            } else if (interventionType == "periodic" &&
                       (person.GetTimeSinceScreened() > interventionPeriod)) {
                { this->interventionDecision(person); }
            } else {
                spdlog::get("main")->error(
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
} // namespace event
