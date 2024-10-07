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
/// This file contains the implementation of the Treatment Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Treatment.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class Treatment::TreatmentIMPL {
    private:
        double lost_to_follow_up_probability;
        double treatment_cost;
        double treatment_utility;
        double toxicity_cost;
        double toxicity_utility;
        double treatment_init_probability;

        struct cost_svr_select {
            int time;
            double svr;
        };

        static int callback_double(void *storage, int count, char **data,
                                   char **columns) {
            std::vector<double> *d = (std::vector<double> *)storage;
            double temp = std::stod(data[0]);
            d->push_back(temp);
            return 0;
        }

        static int callback_struct(void *storage, int count, char **data,
                                   char **columns) {
            std::vector<cost_svr_select> *d =
                (std::vector<cost_svr_select> *)storage;
            cost_svr_select temp;
            temp.time = std::stoi(data[0]);
            temp.svr = std::stod(data[1]);
            d->push_back(temp);
            return 0;
        }

        std::string buildSQL(std::shared_ptr<person::PersonBase> person,
                             std::string column) {
            int geno3 = (person->IsGenotypeThree()) ? 1 : 0;
            int cirr = (person->IsCirrhotic()) ? 1 : 0;
            std::stringstream sql;

            sql << "SELECT " << column << " FROM treatments ";
            sql << "WHERE genotype_three = " << geno3;
            sql << " AND cirrhotic = " << cirr << ";";

            return sql.str();
        }

        bool isEligible(std::shared_ptr<person::PersonBase> person) const {
            person::FibrosisState fibrosisState =
                person->GetTrueFibrosisState();
            person::Behavior behavior = person->GetBehavior();
            int timeBehaviorChange = person->GetTimeBehaviorChange();
            person::PregnancyState pregnancyState = person->GetPregnancyState();
            if (!isEligibleFibrosisStage(fibrosisState) ||
                ((person->GetTimeSinceLinkChange()) > -1) ||
                (behavior == person::Behavior::INJECTION) ||
                (behavior == person::Behavior::FORMER_INJECTION &&
                 timeBehaviorChange < -1) ||
                (pregnancyState == person::PregnancyState::PREGNANT ||
                 pregnancyState == person::PregnancyState::POSTPARTUM)) {
                return false;
            } else {
                return true;
            }
        }

        bool
        isEligibleFibrosisStage(person::FibrosisState fibrosisState) const {
            return (fibrosisState < person::FibrosisState::NONE) ? true : false;
        }

        void
        addTreatmentCostAndUtility(std::shared_ptr<person::PersonBase> person,
                                   double cost, double util) {
            cost::Cost treatmentCost = {cost::CostCategory::TREATMENT,
                                        "Treatment Cost", cost};
            person->AddCost(treatmentCost);
            person->SetUtility(util);
        }

        bool LostToFollowUp(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {
            if (person->HasInitiatedTreatment() || isEligible(person)) {
                return false;
            }
            if (decider->GetDecision({lost_to_follow_up_probability,
                                      1 - lost_to_follow_up_probability}) !=
                0) {
                return false;
            }
            this->quitEngagement(person);
            return true;
        }

        void
        chargeCostOfVisit(std::shared_ptr<person::PersonBase> person,
                          std::shared_ptr<datamanagement::DataManagerBase> dm) {
            cost::Cost visitCost = {cost::CostCategory::TREATMENT,
                                    "Cost of Treatment Visit", treatment_cost};
            person->AddCost(visitCost);
        }

        void chargeCostOfCourse(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string query = buildSQL(person, "treatment_cost");
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_double,
                                              &storage, error);
            if (rc != 0) {
                spdlog::get("main")->error("Error extracting Treatment Data "
                                           "from treatments! Error Message: {}",
                                           error);
                return;
            }
            double cost = (!storage.empty()) ? storage[0] : 0.0;
            cost::Cost courseCost = {cost::CostCategory::TREATMENT,
                                     "Cost of Treatment Course", cost};
            person->AddCost(courseCost);
            person->SetUtility(treatment_utility);
        }

        bool Withdraws(std::shared_ptr<person::PersonBase> person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       std::shared_ptr<stats::DeciderBase> decider) {
            std::string query = buildSQL(person, "withdrawal_probability");

            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_double,
                                              &storage, error);

            if (storage.empty()) {
                spdlog::get("main")->warn("Withdrawal Probability not Found!");
                storage.push_back(0.0);
            }

            if (decider->GetDecision({storage[0], 1 - storage[0]}) == 0) {
                person->AddWithdrawal();
                this->quitEngagement(person);
                return true;
            }
            return false;
        }

        bool
        ExperienceToxicity(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::shared_ptr<stats::DeciderBase> decider) {
            std::string query = buildSQL(person, "toxicity");
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_double,
                                              &storage, error);
            double probability = 0.0;
            if (storage.empty()) {
                spdlog::get("main")->warn("No Toxicity Probability Found!");
            } else {
                probability = storage[0];
            }
            if (decider->GetDecision({probability, 1 - probability}) == 1) {
                return false;
            }
            person->AddToxicReaction();
            this->quitEngagement(person);

            cost::Cost toxicityCost = {cost::CostCategory::TREATMENT,
                                       "Toxicity Cost", toxicity_cost};
            person->AddCost(toxicityCost);
            person->SetUtility(toxicity_utility);
            return true;
        }

        bool
        InitiateTreatment(std::shared_ptr<person::PersonBase> person,
                          std::shared_ptr<datamanagement::DataManagerBase> dm,
                          std::shared_ptr<stats::DeciderBase> decider) {
            // if person hasn't initialized draw, if they have, continue
            // treatment
            if (person->HasInitiatedTreatment()) {
                return true;
            }
            if (decider->GetDecision({treatment_init_probability,
                                      1 - treatment_init_probability}) != 0) {
                this->quitEngagement(person);
                return false;
            }

            // person initiates treatment -- set treatment initiation values
            person->InitiateTreatment();
            return true;
        }

        void quitEngagement(std::shared_ptr<person::PersonBase> person) {
            // unlink from care
            person->Unlink();
            // reset utility
            person->SetUtility(1.0);
        }

        double ParseDoublesFromConfig(
            std::string configKey,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string data;
            dm->GetFromConfig(configKey, data);
            if (data.empty()) {
                spdlog::get("main")->warn("No {} Found!", configKey);
                data = "0.0";
            }
            return std::stod(data);
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            // 1. Check if the Person is Lost To Follow Up (LTFU)
            if (LostToFollowUp(person, dm, decider)) {
                return;
            }

            // 2. Charge the Cost of the Visit (varies if this is retreatment)
            chargeCostOfVisit(person, dm);

            // 3. Determine if the Person Engages and Initiates Treatment (i.e.
            // picks up medicine)
            if (!InitiateTreatment(person, dm, decider)) {
                return;
            }

            // 5. Charge the person for the Course they are on
            chargeCostOfCourse(person, dm);

            // 6. Determine if the person withdraws from the treatment
            if (Withdraws(person, dm, decider)) {
                return;
            }

            // 7. Determine if the person has a toxic reaction
            if (ExperienceToxicity(person, dm, decider)) {
                return;
            }

            // 8. Determine if the person has been treated long enough, if they
            // achieve SVR
            std::string query = buildSQL(person, "time, svr");

            std::vector<struct cost_svr_select> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_struct,
                                              &storage, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error Selecting Treatment! Error Message: {}", error);
                return;
            }
            struct cost_svr_select data = {0, 0.0};
            if (storage.empty()) {
                spdlog::get("main")->warn("No SVR Probability Found!");
            } else {
                data.svr = storage[0].svr;
                data.time = storage[0].time;
            }

            if (decider->GetDecision({data.svr, 1 - data.svr}) == 0) {
                person->AddSVR();
                person->ClearHCV();
            }
            if (person->GetTimeSinceTreatmentInitiation() >= data.time) {
                person->AddCompletedTreatment();
                this->quitEngagement(person);
            }
        }
        TreatmentIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            lost_to_follow_up_probability =
                ParseDoublesFromConfig("treatment.ltfu_probability", dm);
            treatment_cost =
                ParseDoublesFromConfig("treatment.treatment_cost", dm);
            treatment_utility =
                ParseDoublesFromConfig("treatment.treatment_utility", dm);
            toxicity_cost = ParseDoublesFromConfig("treatment.tox_cost", dm);
            toxicity_utility =
                ParseDoublesFromConfig("treatment.tox_utility", dm);
            treatment_init_probability = ParseDoublesFromConfig(
                "treatment.treatment_initialization", dm);
        }
    };
    Treatment::Treatment(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<TreatmentIMPL>(dm);
    }

    Treatment::~Treatment() = default;
    Treatment::Treatment(Treatment &&) noexcept = default;
    Treatment &Treatment::operator=(Treatment &&) noexcept = default;

    void Treatment::doEvent(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }

} // namespace event
