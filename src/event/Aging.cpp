////////////////////////////////////////////////////////////////////////////////
// File: Aging.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-31                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-25                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Aging.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utility.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>
#include <tuple>
#include <unordered_map>

namespace event {
class Aging::AgingIMPL {
private:
    utility::UtilityCategory util_category =
        utility::UtilityCategory::BACKGROUND;
    double discount = 0.0;
    struct cost_util {
        double cost;
        double util;
    };

    using agemap_t =
        std::unordered_map<Utils::tuple_3i, struct cost_util,
                           Utils::key_hash_3i, Utils::key_equal_3i>;
    agemap_t data;

    static int callback(void *storage, int count, char **data, char **columns) {
        Utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        struct cost_util cu = {Utils::stod_positive(data[3]),
                               Utils::stod_positive(data[4])};
        (*((agemap_t *)storage))[tup] = cu;
        return 0;
    }

    std::string buildSQL() const {
        return "SELECT age_years, gender, drug_behavior, cost, utility "
               "FROM background_impacts;";
    }

    std::pair<double, double> DiscountUtility(std::pair<double, double> util,
                                              int timestep) {
        std::pair<double, double> discounted = {
            Utils::discount(util.first, discount, timestep),
            Utils::discount(util.second, discount, timestep)};
        return discounted;
    }

    /// @brief Adds person's background cost
    /// @param person The person to whom cost will be added
    void addBackgroundCostAndUtility(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        int age_years = (int)(person->GetAge() / 12.0);
        int gender = ((int)person->GetSex());
        int behavior = ((int)person->GetBehavior());
        Utils::tuple_3i tup = std::make_tuple(age_years, gender, behavior);

        double discountAdjustedCost = Event::DiscountEventCost(
            data[tup].cost, discount, person->GetCurrentTimestep());
        person->AddCost(data[tup].cost, discountAdjustedCost,
                        cost::CostCategory::BACKGROUND);

        person->SetUtility(data[tup].util, util_category);
        std::pair<double, double> utilities = person->GetUtility();
        std::pair<double, double> discount_utilities =
            DiscountUtility(utilities, person->GetCurrentTimestep());
        person->AccumulateTotalUtility(utilities, discount_utilities);
    }

    int LoadData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string query = this->buildSQL();
        std::string error;
        int rc =
            dm->SelectCustomCallback(query, this->callback, &this->data, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "Error extracting Aging Data from background costs and "
                "background behaviors! Error Message: {}",
                error);
        }
        if (this->data.empty()) {
            spdlog::get("main")->warn("No Background Cost found for Aging!");
        }
        return rc;
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm) {
        person->Grow();
        this->addBackgroundCostAndUtility(person, dm);

        // Add life span
        person->AddDiscountedLifeSpan(
            Utils::discount(1, discount, person->GetCurrentTimestep()));
    }
    AgingIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        this->discount = GetDoubleFromConfig("cost.discounting_rate", dm);
        this->data.clear();
        LoadData(dm);
    }
};

Aging::Aging(std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<AgingIMPL>(dm);
}

Aging::~Aging() = default;
Aging::Aging(Aging &&) noexcept = default;
Aging &Aging::operator=(Aging &&) noexcept = default;

void Aging::DoEvent(std::shared_ptr<person::PersonBase> person,
                    std::shared_ptr<datamanagement::DataManagerBase> dm,
                    std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm);
}

} // namespace event
