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
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class BehaviorChanges::BehaviorChangesIMPL {
    private:
        double discount = 0.0;
        struct behavior_transitions {
            double never = 0.0;
            double fni = 0.25;
            double fi = 0.25;
            double ni = 0.25;
            double in = 0.25;
        };

        typedef std::unordered_map<Utils::tuple_4i, struct behavior_transitions,
                                   Utils::key_hash_4i, Utils::key_equal_4i>
            behaviormap_t;
        behaviormap_t behavior_data;

        std::string TransitionSQL() const {
            return "SELECT age_years, gender, moud, drug_behavior, never, "
                   "former_noninjection, former_injection, "
                   "noninjection, injection FROM behavior_transitions;";
        }

        static int callback_trans(void *storage, int count, char **data,
                                  char **columns) {
            Utils::tuple_4i tup =
                std::make_tuple(std::stoi(data[0]), std::stoi(data[1]),
                                std::stoi(data[2]), std::stoi(data[3]));
            struct behavior_transitions behavior = {
                std::stod(data[4]), std::stod(data[5]), std::stod(data[6]),
                std::stod(data[7]), std::stod(data[8])};
            (*((behaviormap_t *)storage))[tup] = behavior;
            return 0;
        }

        struct cost_util {
            double cost = 0.0;
            double util = 0.0;
        };

        typedef std::unordered_map<Utils::tuple_2i, struct cost_util,
                                   Utils::key_hash_2i, Utils::key_equal_2i>
            costmap_t;
        costmap_t cost_data;

        std::string CostSQL() const {
            return "SELECT gender, drug_behavior, cost, utility FROM "
                   "behavior_impacts;";
        }

        static int callback_costs(void *storage, int count, char **data,
                                  char **columns) {
            Utils::tuple_2i tup =
                std::make_tuple(std::stoi(data[0]), std::stoi(data[1]));
            struct cost_util cu = {std::stod(data[0]), std::stod(data[1])};
            (*((costmap_t *)storage))[tup] = cu;
            return 0;
        }

        void calculateCostAndUtility(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            int gender = ((int)person->GetSex());
            int behavior = ((int)person->GetBehavior());
            Utils::tuple_2i tup = std::make_tuple(gender, behavior);

            double discountAdjustedCost = Event::DiscountEventCost(
                cost_data[tup].cost, discount, person->GetCurrentTimestep());
            person->AddCost(discountAdjustedCost, cost::CostCategory::BEHAVIOR);
            person->SetUtility(cost_data[tup].util);
        }

        int LoadCostData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string error;
            int rc = dm->SelectCustomCallback(CostSQL(), this->callback_costs,
                                              &cost_data, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Behavior Change Data from background "
                    "costs and "
                    "background behaviors! Error Message: {}",
                    error);
            }
            if (cost_data.empty()) {
                spdlog::get("main")->warn(
                    "No Background Cost found for Behavior Changes!");
            }
            return rc;
        }

        int
        LoadBehaviorData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string error;
            int rc = dm->SelectCustomCallback(
                TransitionSQL(), this->callback_trans, &behavior_data, error);
            if (rc != 0) {
                spdlog::get("main")->error("Error extracting Behavior Change "
                                           "Transition Data! Error Message: {}",
                                           error);
            }
            if (behavior_data.empty()) {
                spdlog::get("main")->warn(
                    "No Transition Data found for Behavior Changes!");
            }
            return rc;
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {

            // Typical Behavior Change
            // 1. Generate the transition probabilities based on the starting
            // state
            int age_years = (int)(person->GetAge() / 12.0);
            int gender = ((int)person->GetSex());
            int moud = ((int)person->GetMoudState());
            int behavior = ((int)person->GetBehavior());
            Utils::tuple_4i tup =
                std::make_tuple(age_years, gender, moud, behavior);
            struct behavior_transitions temp = behavior_data[tup];
            std::vector<double> probs = {temp.never, temp.fni, temp.fi, temp.ni,
                                         temp.in};

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
            person->SetBehavior(toBC);

            // Insert person's behavior cost
            this->calculateCostAndUtility(person, dm);
        }

        BehaviorChangesIMPL(
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string discount_data;
            int rc = dm->GetFromConfig("cost.discounting_rate", discount_data);
            if (discount_data.empty()) {
                this->discount = std::stod(discount_data);
            }
            if (dm == nullptr) {
                spdlog::get("main")->warn(
                    "No Data Manager Provided during Construction. No Data "
                    "Loaded to Behavior Changes.");
                return;
            }
            rc = LoadCostData(dm);
            rc = LoadBehaviorData(dm);
        }
    };

    BehaviorChanges::BehaviorChanges(
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<BehaviorChangesIMPL>(dm);
    }

    BehaviorChanges::~BehaviorChanges() = default;
    BehaviorChanges::BehaviorChanges(BehaviorChanges &&) noexcept = default;
    BehaviorChanges &
    BehaviorChanges::operator=(BehaviorChanges &&) noexcept = default;

    void BehaviorChanges::doEvent(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm,
        std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
