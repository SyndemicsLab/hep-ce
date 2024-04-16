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
        if (!person->getIsAlive()) {
            return;
        }

        // can only progress in fibrosis state if actively infected with HCV
        if (person->getHEPCState() == Person::HEPCState::NONE) {
            return;
        }
        // 1. Get current fibrosis status
        Person::FibrosisState fs = person->getFibrosisState();
        // 2. Get the transition probabilities from that state
        std::vector<double> probs = getTransitions(person);
        // 3. Randomly draw the state to transition to
        Person::FibrosisState toFS =
            (Person::FibrosisState)this->getDecision(probs);
        // 4. Transition to the new state
        person->updateFibrosis(toFS, this->getCurrentTimestep());

        // insert Person's liver-related disease cost (taking the highest
        // fibrosis state)
        // TODO: make the ability to add this only if identified a toggle-able
        this->addLiverDiseaseCost(person);
    }

    std::vector<double> FibrosisProgression::getTransitions(
        std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;

        // intentional truncation
        selectCriteria["initial_state"] = Person::Person::
            fibrosisStateEnumToStringMap[person->getFibrosisState()];
        auto resultTable = table->selectWhere(selectCriteria);
        std::map<Person::FibrosisState, double> probMap =
            getProbabilityMap(resultTable);

        std::vector<double> result = {};
        for (auto kv : probMap) {
            result.push_back(kv.second);
        }
        return result;
    }
    std::map<Person::FibrosisState, double>
    FibrosisProgression::getProbabilityMap(Data::IDataTablePtr subTable) const {
        std::map<Person::FibrosisState, double> probMap;
        for (auto kv : Person::Person::fibrosisStateEnumToStringMap) {
            probMap[kv.first] = 0.0;
        }

        std::vector<std::string> newStateColumn =
            subTable->getColumn("new_state");
        std::vector<std::string> probColumn =
            subTable->getColumn("probability");

        for (int i = 0; i < newStateColumn.size(); ++i) {
            probMap[Person::Person::fibrosisStateMap[newStateColumn[i]]] =
                stod(probColumn[i]);
        }
        return probMap;
    }

    void FibrosisProgression::addLiverDiseaseCost(
        std::shared_ptr<Person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;
        selectCriteria["hcv_status"] =
            Person::Person::hepcStateEnumToStringMap[person->getHEPCState()];
        selectCriteria["initial_state"] = Person::Person::
            fibrosisStateEnumToStringMap[person->getFibrosisState()];

        auto resultTable = table->selectWhere(selectCriteria);
        auto res = (*resultTable)["cost"];
        double cost = std::stod(res[0]);

        Cost::Cost liverDiseaseCost = {this->costCategory, "Liver Disease Care",
                                       cost};

        person->addCost(liverDiseaseCost, this->getCurrentTimestep());
    }
} // namespace Event
