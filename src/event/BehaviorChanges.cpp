////////////////////////////////////////////////////////////////////////////////
// File: BehaviorChanges.cpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-09-13                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "BehaviorChanges.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utility.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
class BehaviorChanges::BehaviorChangesIMPL {
private:
    utility::UtilityCategory util_category = utility::UtilityCategory::BEHAVIOR;
    double discount = 0.0;
    struct behavior_transitions {
        double never = 0.0;
        double fni = 0.25;
        double fi = 0.25;
        double ni = 0.25;
        double in = 0.25;
    };

    using behaviormap_t =
        std::unordered_map<Utils::tuple_4i, struct behavior_transitions,
                           Utils::key_hash_4i, Utils::key_equal_4i>;
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
            Utils::stod_positive(data[4]), Utils::stod_positive(data[5]),
            Utils::stod_positive(data[6]), Utils::stod_positive(data[7]),
            Utils::stod_positive(data[8])};
        (*((behaviormap_t *)storage))[tup] = behavior;
        return 0;
    }

    struct cost_util {
        double cost = 0.0;
        double util = 0.0;
    };

    using costmap_t =
        std::unordered_map<Utils::tuple_3i, struct cost_util,
                           Utils::key_hash_3i, Utils::key_equal_3i>;
    costmap_t cost_data;

    std::string CostSQL() const {
        return "SELECT age_years, gender, drug_behavior, cost, utility FROM "
               "behavior_impacts;";
    }

    static int callback_costs(void *storage, int count, char **data,
                              char **columns) {
        Utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        struct cost_util cu = {Utils::stod_positive(data[3]),
                               Utils::stod_positive(data[4])};
        (*((costmap_t *)storage))[tup] = cu;
        return 0;
    }

    void calculateCostAndUtility(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        int age = (int)(person->GetAge() / 12.0);
        int gender = ((int)person->GetSex());
        int behavior = ((int)person->GetBehavior());
        Utils::tuple_3i tup = std::make_tuple(age, gender, behavior);

        double discountAdjustedCost = Event::DiscountEventCost(
            cost_data[tup].cost, discount, person->GetCurrentTimestep());
        person->AddCost(cost_data[tup].cost, discountAdjustedCost,
                        cost::CostCategory::BEHAVIOR);
        person->SetUtility(cost_data[tup].util, util_category);
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

    int LoadBehaviorData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string error;
        int rc = dm->SelectCustomCallback(TransitionSQL(), this->callback_trans,
                                          &behavior_data, error);
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
    void DoEvent(std::shared_ptr<person::PersonBase> person,
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
        std::vector<double> probs = {
            behavior_data[tup].never, behavior_data[tup].fni,
            behavior_data[tup].fi, behavior_data[tup].ni,
            behavior_data[tup].in};

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

    BehaviorChangesIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string discount_data;
        int rc = dm->GetFromConfig("cost.discounting_rate", discount_data);
        if (!discount_data.empty()) {
            this->discount = Utils::stod_positive(discount_data);
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

void BehaviorChanges::DoEvent(
    std::shared_ptr<person::PersonBase> person,
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
