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

namespace event {
    void FibrosisProgression::doEvent(std::shared_ptr<person::Person> person) {
        // can only progress in fibrosis state if actively infected with HCV
        if (person->getHCV() == person::HCV::NONE) {
            return;
        }
        // 1. Get current fibrosis status
        person::FibrosisState fs = person->getFibrosisState();
        // 2. Get the transition probability
        std::vector<double> prob = getTransition(fs);
        // 3. Draw whether the person's fibrosis state progresses
        int res = ((int)fs + this->getDecision(prob));
        if (res >= (int)person::FibrosisState::COUNT) {
            this->logger->error("Fibrosis Progression Decision returned "
                                "value outside bounds");
            return;
        }
        person::FibrosisState toFS = (person::FibrosisState)res;
        // 4. Apply the result state
        person->updateFibrosis(toFS, this->getCurrentTimestep());

        // insert Person's liver-related disease cost (taking the highest
        // fibrosis state)
        if (this->addCostOnlyIfIdentified &&
            !person->isIdentifiedAsInfected()) {
            return;
        }
        this->addLiverDiseaseCost(person);
    }

    std::vector<double>
    FibrosisProgression::getTransition(person::FibrosisState fs) {
        // get the probability of transitioning to the next fibrosis state
        Data::ReturnType temp;

        switch (fs) {
        case person::FibrosisState::F0:
            temp = this->config.get("fibrosis.f01", 0.0);
            break;
        case person::FibrosisState::F1:
            temp = this->config.get("fibrosis.f12", 0.0);
            break;
        case person::FibrosisState::F2:
            temp = this->config.get("fibrosis.f23", 0.0);
            break;
        case person::FibrosisState::F3:
            temp = this->config.get("fibrosis.f34", 0.0);
            break;
        case person::FibrosisState::F4:
            temp = this->config.get("fibrosis.f4d", 0.0);
            break;
        case person::FibrosisState::DECOMP:
            temp = (Data::ReturnType)0.0;
            break;
        }

        // to work with getDecision, return the complement to the transition
        // probability
        return {1 - std::get<double>(temp)};
    }

    void FibrosisProgression::addLiverDiseaseCost(
        std::shared_ptr<person::Person> person) {
        std::unordered_map<std::string, std::string> selectCriteria;
        selectCriteria["hcv_status"] =
            person::person::hcvEnumToStringMap[person->getHCV()];
        selectCriteria["metavir_stage"] = person::person::
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
} // namespace event
