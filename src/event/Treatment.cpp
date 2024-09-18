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
#include <DataManagement/DataManager.hpp>
#include <sstream>

namespace event {
    class Treatment::TreatmentIMPL {
    private:
        std::shared_ptr<datamanagement::DataManager> dm;
        std::shared_ptr<stats::Decider> decider;

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

        std::string buildSQL(person::PersonBase &person, std::string column) {
            std::string geno3 = (person.IsGenotypeThree()) ? "TRUE" : "FALSE";
            std::string cirr = (person.IsCirrhotic()) ? "TRUE" : "FALSE";
            std::stringstream sql;

            sql << "SELECT " << column << " FROM treatments ";
            sql << "WHERE is_genotype3 = '" << geno3 << "'";
            sql << " AND is_cirrhotic = '" << cirr << "';";

            return sql.str();
        }

        bool isEligible(person::PersonBase const &person) const {
            person::FibrosisState fibrosisState = person.GetFibrosisState();
            person::Behavior behavior = person.GetBehavior();
            int timeBehaviorChange = person.GetTimeBehaviorChange();
            person::PregnancyState pregnancyState = person.GetPregnancyState();
            if (!isEligibleFibrosisStage(fibrosisState) ||
                ((person.GetTimeSinceLinkChange()) > -1) ||
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

        void addTreatmentCostAndUtility(person::PersonBase &person, double cost,
                                        double util) {
            cost::Cost treatmentCost = {cost::CostCategory::TREATMENT,
                                        "Treatment Cost", cost};
            person.AddCost(treatmentCost);
            person.SetUtility(util);
        }

        bool isLostToFollowUp(person::PersonBase &person) {
            if (person.HasInitiatedTreatment() || isEligible(person)) {
                return false;
            }
            std::string data;
            dm->GetFromConfig("treatment.ltfu_probability", data);
            double ltfuProb = std::stod(data);
            int ltfu = decider->GetDecision({ltfuProb});
            if (ltfu != 0) {
                return false;
            }
            person.Unlink();
            return true;
        }

        void chargeCostOfVisit(person::PersonBase &person) {
            std::string data;
            dm->GetFromConfig("treatment.treatment_cost", data);
            double cost = std::stod(data);
            cost::Cost visitCost = {cost::CostCategory::TREATMENT,
                                    "Cost of Treatment Visit", cost};
            person.AddCost(visitCost);
        }

        void chargeCostOfCourse(person::PersonBase &person) {
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
            cost::Cost courseCost = {cost::CostCategory::TREATMENT,
                                     "Cost of Treatment Course", storage[0]};
            person.AddCost(courseCost);

            std::string data;
            dm->GetFromConfig("treatment.treatment_utility", data);
            double util = std::stod(data);
            person.SetUtility(util);
        }

        bool doesWithdraw(person::PersonBase &person) {
            std::string query = buildSQL(person, "withdrawal_probability");

            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_double,
                                              &storage, error);

            int withdraw = decider->GetDecision({storage[0]});
            if (withdraw == 1) {
                return true;
            }
            return false;
        }

        bool experiencedToxicity(person::PersonBase &person) {
            std::string query = buildSQL(person, "toxicity");
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_double,
                                              &storage, error);
            int toxicity = decider->GetDecision({storage[0]});
            return (toxicity == 1) ? true : false;
        }

        bool initiatesTreatment(person::PersonBase &person) {
            // if person hasn't initialized draw, if they have, continue
            // treatment
            if (person.HasInitiatedTreatment()) {
                return true;
            }
            std::string data;
            dm->GetFromConfig("treatment.treatment_initialization", data);
            double initProb = std::stod(data);
            int initiation = decider->GetDecision({initProb});
            // if the randomly-drawn value from getDecision is 0, person
            // does not initiate treatment
            if (initiation == 0) {

                return false;
            }
            // person initiates treatment -- set treatment initiation values
            person.SetInitiatedTreatment(true);
            return true;
        }

        void quitEngagement(person::PersonBase &person) {
            // unlink from care
            person.Unlink();
            // reset utility
            person.SetUtility(1.0);
        }

    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) {
            this->dm = dm;
            this->decider = decider;

            // 1. Check if the Person is Lost To Follow Up (LTFU)
            if (this->isLostToFollowUp(person)) {
                this->quitEngagement(person);
                return;
            }

            // 2. Charge the Cost of the Visit (varies if this is retreatment)
            chargeCostOfVisit(person);

            // 3. Determine if the Person Engages and Initiates Treatment (i.e.
            // picks up medicine)
            if (!this->initiatesTreatment(person)) {
                this->quitEngagement(person);
                return;
            }

            // 5. Charge the person for the Course they are on
            chargeCostOfCourse(person);

            // 6. Determine if the person withdraws from the treatment
            if (this->doesWithdraw(person)) {
                this->quitEngagement(person);
                return;
            }

            // 7. Determine if the person has a toxic reaction
            if (this->experiencedToxicity(person)) {
                this->quitEngagement(person);
                std::string data;
                dm->GetFromConfig("treatment.tox_cost", data);
                double cost = std::stod(data);
                cost::Cost toxicityCost = {cost::CostCategory::TREATMENT,
                                           "Toxicity Cost", cost};
                person.AddCost(toxicityCost);

                data.clear();
                dm->GetFromConfig("treatment.tox_utility", data);
                double utility = std::stod(data);
                person.SetUtility(utility);
            }

            // 8. Determine if the person has been treated long enough, if they
            // achieve SVR
            int timeSinceInitiation = person.GetTimeSinceTreatmentInitiation();

            std::string query = buildSQL(person, "time, svr");

            std::vector<struct cost_svr_select> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_struct,
                                              &storage, error);

            if (decider->GetDecision({storage[0].svr}) == 1) {
                person.ClearHCV();
            }
            if (timeSinceInitiation >= storage[0].time) {
                this->quitEngagement(person);
            }
        }
    };
    Treatment::Treatment(std::shared_ptr<stats::Decider> decider,
                         std::shared_ptr<datamanagement::DataManager> dm,
                         std::string name)
        : Event(dm, name), decider(decider) {
        impl = std::make_unique<TreatmentIMPL>();
    }

    void Treatment::doEvent(person::PersonBase &person) {
        impl->doEvent(person, dm, decider);
    }

} // namespace event
