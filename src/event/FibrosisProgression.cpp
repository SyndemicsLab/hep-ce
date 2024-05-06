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
/// This file contains the implementation of the FibrosisProgression Event
/// Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "FibrosisProgression.hpp"

namespace Event {
    void FibrosisProgression::doEvent(std::shared_ptr<Person::Person> person) {
        // can only progress in fibrosis state if actively infected with HCV
        if (person->getHEPCState() == Person::HEPCState::NONE) {
            return;
        }
        // 1. Get current fibrosis status
        Person::FibrosisState fs = person->getFibrosisState();
        // 2. Get the transition probability
        std::vector<double> prob = getTransition(fs);
        // 3. Draw whether the person's fibrosis state progresses
        Person::FibrosisState toFS =
            (Person::FibrosisState)((int)fs + this->getDecision(prob));
        // 4. Apply the result state
        person->updateFibrosis(toFS, this->getCurrentTimestep());

        // insert Person's liver-related disease cost (taking the highest
        // fibrosis state)
        // TODO: make the ability to add this only if identified a toggle-able
        this->addLiverDiseaseCost(person);
    }

    std::vector<double>
    FibrosisProgression::getTransition(Person::FibrosisState fs) {
        // get the probability of transitioning to the next fibrosis state
        Data::ReturnType temp;

        switch (fs) {
        case Person::FibrosisState::F0:
            temp = this->config.get("fibrosis.f01", 0.0);
            break;
        case Person::FibrosisState::F1:
            temp = this->config.get("fibrosis.f12", 0.0);
            break;
        case Person::FibrosisState::F2:
            temp = this->config.get("fibrosis.f23", 0.0);
            break;
        case Person::FibrosisState::F3:
            temp = this->config.get("fibrosis.f34", 0.0);
            break;
        case Person::FibrosisState::F4:
            temp = this->config.get("fibrosis.f4d", 0.0);
            break;
        case Person::FibrosisState::DECOMP:
            temp = (Data::ReturnType)0.0;
            break;
        }

        // to work with getDecision, return the complement to the transition
        // probability
        return {1 - std::get<double>(temp)};
    }

    void FibrosisProgression::addLiverDiseaseCost(
        std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;
        selectCriteria["hcv_status"] =
            Person::Person::hepcStateEnumToStringMap[person->getHEPCState()];
        selectCriteria["metavir_stage"] = Person::Person::
            fibrosisStateEnumToStringMap[person->getFibrosisState()];
        auto resultTable = table->selectWhere(selectCriteria);
        double cost;
        if (resultTable->empty()) {
            // yell
            cost = 0;
        } else {
            auto res = (*resultTable)["cost"];
            cost = std::stod(res[0]);
        }

        Cost::Cost liverDiseaseCost = {this->costCategory, "Liver Disease Care",
                                       cost};

        person->addCost(liverDiseaseCost, this->getCurrentTimestep());
    }
} // namespace Event
