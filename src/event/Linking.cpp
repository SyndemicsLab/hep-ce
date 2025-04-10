////////////////////////////////////////////////////////////////////////////////
// File: Linking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-02                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-09                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Linking.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <string>

namespace event {
int LinkingIMPL::callback_link(void *storage, int count, char **data,
                               char **columns) {
    Utils::tuple_4i tup =
        std::make_tuple(std::stoi(data[0]), std::stoi(data[1]),
                        std::stoi(data[2]), std::stoi(data[3]));
    (*((linkmap_t *)storage))[tup] = Utils::stod_positive(data[4]);
    return 0;
}

double
LinkingIMPL::GetLinkProbability(std::shared_ptr<person::PersonBase> person,
                                person::InfectionType it) {
    int age_years = (int)(person->GetAge() / 12.0);
    int gender = (int)person->GetSex();
    int drug_behavior = (int)person->GetBehavior();
    int pregnancy = (int)person->GetPregnancyState();
    const person::LinkageType &type = person->GetLinkageType(it);
    Utils::tuple_4i tup =
        std::make_tuple(age_years, gender, drug_behavior, pregnancy);
    if (type == person::LinkageType::BACKGROUND) {
        return this->background_link_data[tup];
    } else if (type == person::LinkageType::INTERVENTION) {
        return this->intervention_link_data[tup];
    }
    return 0.0;
}

void LinkingIMPL::AddLinkingCost(std::shared_ptr<person::PersonBase> person,
                                 LINK_COST type) {
    double cost;
    if (type == LINK_COST::INTERVENTION) {
        cost = this->intervention_cost;
    } else if (type == LINK_COST::FALSE_POSITIVE) {
        cost = this->false_positive_test_cost;
    } else {
        return;
    }

    double discountAdjustedCost =
        Event::DiscountEventCost(cost, discount, person->GetCurrentTimestep());
    person->AddCost(cost, discountAdjustedCost, cost::CostCategory::LINKING);
}

double LinkingIMPL::ApplyMultiplier(double prob, double mult) {
    return Utils::rateToProbability(Utils::probabilityToRate(prob) * mult);
}

bool LinkingIMPL::CheckForPregnancyEvent(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::vector<std::string> event_list = Utils::split2vecT<std::string>(
        Event::GetStringFromConfig("simulation.events", dm), ',');

    return (std::find(event_list.begin(), event_list.end(), "pregnancy") !=
            event_list.end());
}

LinkingIMPL::LinkingIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
    this->discount = Event::GetDoubleFromConfig("cost.discounting_rate", dm);
}
} // namespace event
