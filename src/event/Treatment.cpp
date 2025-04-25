////////////////////////////////////////////////////////////////////////////////
// File: Treatment.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-16                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
// Modified By: Matthew Carroll                                               //
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
    if (pregnancy_state == person::PregnancyState::NA) {
        return true;
    }
    for (std::string state : this->ineligible_pregnancy) {
        person::PregnancyState temp;
        temp << state;
        if (pregnancy_state == temp) {
            return false;
        }
    }
    return true;
}

void TreatmentIMPL::ResetUtility(std::shared_ptr<person::PersonBase> person) {
    // used for HCV
    person->SetUtility(1.0, this->UTIL_CATEGORY);
}

void TreatmentIMPL::QuitEngagement(std::shared_ptr<person::PersonBase> person) {
    person->EndTreatment(this->INF_TYPE);
    person->Unlink(this->INF_TYPE);
    // reset utility - moved to its own function to allow HIV to be handled
    // differently
    this->ResetUtility(person);
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

bool TreatmentIMPL::IsEligible(
    std::shared_ptr<person::PersonBase> person) const {
    // If currently on a treatment, a new eligibility check should
    // verify retreatment is allowed
    bool check_retreatment = person->HasInitiatedTreatment();
    if (!this->allow_retreatment && check_retreatment) {
        return false;
    }
    person::FibrosisState fibrosisState = person->GetTrueFibrosisState();
    person::Behavior behavior = person->GetBehavior();
    int timeSinceLastUse = person->GetTimeBehaviorChange();
    int timeSinceLinked = person->GetTimeSinceLinkChange();
    person::PregnancyState pregnancyState = person->GetPregnancyState();
    // if a person is an eligible fibrosis state, behavior, linked time,
    // and hasn't used
    if (isEligibleFibrosisStage(fibrosisState) &&
        isEligibleBehavior(behavior) &&
        (pregnancyState == person::PregnancyState::NA ||
         isEligiblePregnancy(pregnancyState)) &&
        (timeSinceLastUse > ineligible_time_since_last_use) &&
        (timeSinceLinked > ineligible_time_since_linked)) {
        return true;
    }
    return false;
}

TreatmentIMPL::TreatmentIMPL(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    this->discount = Utils::GetDoubleFromConfig("cost.discounting_rate", dm);
}
} // namespace event
