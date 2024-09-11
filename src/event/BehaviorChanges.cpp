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
/// This file contains the implementation of the BehaviorChanges Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "BehaviorChanges.hpp"
#include "DataManager.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <sstream>

namespace event {
    class BehaviorChanges::BehaviorChangesIMPL {
    private:
        cost::CostCategory costCategory = cost::CostCategory::BEHAVIOR;

        struct behavior_trans_select {
            double never;
            double fni;
            double fi;
            double ni;
            double in;
        };

        struct behavior_costs_select {
            double cost;
            double util;
        };

        static int callback_trans(void *storage, int count, char **data,
                                  char **columns) {
            std::vector<struct behavior_trans_select> *d =
                (std::vector<struct behavior_trans_select> *)storage;
            struct behavior_trans_select temp;
            temp.never = std::stod(data[0]);
            temp.fni = std::stod(data[1]);
            temp.fi = std::stod(data[2]);
            temp.ni = std::stod(data[3]);
            temp.in = std::stod(data[4]);
            d->push_back(temp);
            return 0;
        }

        static int callback_costs(void *storage, int count, char **data,
                                  char **columns) {
            std::vector<struct behavior_costs_select> *d =
                (std::vector<struct behavior_costs_select> *)storage;
            struct behavior_costs_select temp;
            temp.cost = std::stod(data[0]); // First Column Selected
            temp.util = std::stod(data[1]); // Second Column Selected
            d->push_back(temp);
            return 0;
        }

        std::string buildTransitionSQL(person::PersonBase const person) const {
            std::stringstream sql;
            sql << "SELECT never, former_noninjection, former_injection, "
                   "noninjection, injection ";
            sql << "FROM behavior_transitions ";
            sql << "WHERE age_years = " << std::to_string(person.GetAge());
            sql << "AND gender = " << person.GetSex();
            sql << "AND moud = " << person.GetMoudState();
            sql << "AND drug_behavior = " << person.GetBehavior();

            return sql.str();
        }

        std::string buildCostSQL(person::PersonBase const person) const {
            std::stringstream sql;
            sql << "SELECT cost, utility FROM behavior_costs";
            sql << "INNER JOIN behavior_utilities ON "
                   "((behavior_costs.age_years = "
                   "behavior_utilities.age_years) AND "
                   "(behavior_costs.gender = behavior_utilities.gender) "
                   "AND (behavior_costs.drug_behavior = "
                   "behavior_utilities.drug_behavior)) ";
            sql << "WHERE behavior_costs.age_years = "
                << std::to_string(person.GetAge());
            sql << " AND behavior_costs.gender = " << person.GetSex();
            sql << " AND behavior_costs.drug_behavior = "
                << person.GetBehavior();
            return sql.str();
        }

        void calculateCostAndUtility(
            person::PersonBase &person,
            std::shared_ptr<datamanagement::DataManager> dm) const {
            std::string query = this->buildCostSQL(person);
            std::vector<struct behavior_costs_select> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_costs,
                                              &storage, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "No cost avaliable for Behavior Changes");
            }

            cost::Cost behaviorCost = {this->costCategory, "Drug Behavior",
                                       storage[0].cost};
            person.AddCost(behaviorCost);
            person.SetUtility(storage[0].util);
        }

    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) {

            // Determine person's current behavior classification
            person::Behavior bc = person.GetBehavior();

            // Insert person's behavior cost
            this->calculateCostAndUtility(person, dm);

            // Typical Behavior Change
            // 1. Generate the transition probabilities based on the starting
            // state
            std::string query = this->buildTransitionSQL(person);
            std::vector<struct behavior_trans_select> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_trans,
                                              &storage, error);

            std::vector<double> probs = {storage[0].never, storage[0].fni,
                                         storage[0].fi, storage[0].ni,
                                         storage[0].in};
            // currently using placeholders to test compiling
            // std::vector<double> probs = {0.25, 0.25, 0.25, 0.25};
            // 2. Draw a behavior state to be transitioned to
            int res = decider->GetDecision(probs);
            if (res >= (int)person::Behavior::COUNT) {
                spdlog::get("main")->error(
                    "Behavior Classification Decision returned "
                    "value outside bounds");
                return;
            }
            person::Behavior toBC = (person::Behavior)res;

            // 3. If the drawn state differs from the current state, change the
            // bools in BehaviorState to match
            person.UpdateBehavior(toBC);
        }
    };

    BehaviorChanges::BehaviorChanges(
        std::shared_ptr<stats::Decider> decider,
        std::shared_ptr<datamanagement::DataManager> dm, std::string name)
        : Event(dm, name), decider(decider) {
        impl = std::make_unique<BehaviorChangesIMPL>();
    }

    void BehaviorChanges::doEvent(person::PersonBase &person) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
