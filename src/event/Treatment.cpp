////////////////////////////////////////////////////////////////////////////////
// File: Treatment.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-16                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-17                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Treatment.hpp"

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

TreatmentIMPL::TreatmentIMPL(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    this->discount = Utils::GetDoubleFromConfig("cost.discounting_rate", dm);
}
} // namespace event
