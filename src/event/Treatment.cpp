////////////////////////////////////////////////////////////////////////////////
// File: Treatment.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-16                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-21                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Treatment.hpp"
#include "Decider.hpp"

namespace event {
bool TreatmentIMPL::isEligibleFibrosisStage(
    person::FibrosisState fibrosisState) const {
    for (std::string state : this->ineligible_fibrosis) {
        person::FibrosisState temp;
        temp << state;
        if (fibrosisState == temp) {
            return false;
        }
    }
    return true;
}

bool TreatmentIMPL::isEligibleBehavior(person::Behavior behavior) const {
    for (std::string state : this->ineligible_behaviors) {
        person::Behavior temp;
        temp << state;
        if (behavior == temp) {
            return false;
        }
    }
    return true;
}

bool TreatmentIMPL::isEligiblePregnancy(
    person::PregnancyState pregnancy_state) const {
    for (std::string state : this->ineligible_pregnancy) {
        person::PregnancyState temp;
        temp << state;
        if (pregnancy_state == temp) {
            return false;
        }
    }
    return true;
}

void TreatmentIMPL::QuitEngagement(std::shared_ptr<person::PersonBase> person) {
    person->EndTreatment();
    person->Unlink();
    // reset utility
    person->SetUtility(1.0, this->UTIL_CATEGORY);
}

bool TreatmentIMPL::LostToFollowUp(
    std::shared_ptr<person::PersonBase> person,
    std::shared_ptr<stats::DeciderBase> decider) {
    // If the person is already on treatment, they can't be lost to
    // follow up
    if (person->HasInitiatedTreatment(this->INF_TYPE)) {
        return false;
    }
    if (decider->GetDecision({this->lost_to_follow_up_probability}) == 0) {
        this->QuitEngagement(person);
        return true;
    }
    return false;
}

void TreatmentIMPL::ChargeCost(std::shared_ptr<person::PersonBase> person,
                               double cost) {
    double discounted_cost = Event::DiscountEventCost(
        cost, this->discount, person->GetCurrentTimestep());
    person->AddCost(cost, discounted_cost, this->COST_CATEGORY);
}

std::vector<std::string> TreatmentIMPL::LoadEligibilityVectors(
    std::string config_key,
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string data = Utils::GetStringFromConfig(config_key, dm, true);
    if (data.empty()) {
        return {};
    }
    return Utils::split2vecT<std::string>(data, ',');
}

int TreatmentIMPL::GetTreatmentDuration(
    std::shared_ptr<person::PersonBase> person,
    int (*key_function)(std::shared_ptr<person::PersonBase>, void *)) {
    if (this->duration_data.empty()) {
        spdlog::get("main")->warn("No Treatment Duration Found!");
        return -1;
    }
    Utils::tuple_3i storage;
    key_function(person, &storage);
    double duration = this->duration_data[storage];
    return static_cast<int>(duration);
}

TreatmentIMPL::TreatmentIMPL(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    this->discount = Utils::GetDoubleFromConfig("cost.discounting_rate", dm);
}
} // namespace event
