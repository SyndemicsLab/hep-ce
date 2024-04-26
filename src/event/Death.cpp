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
/// This file contains the implementation of the Death Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Death.hpp"

namespace Event {
    void Death::doEvent(std::shared_ptr<Person::Person> person) {
        if (!person->getIsAlive()) {
            return;
        }

        if (person->age >= 1200) {
            this->die(person);
            return;
        }

        // "Calculate background mortality rate based on age, gender, and IDU"
        double backgroundMortality = 1.0;

        // Fatal Overdose
        // 1. Check if the person overdosed this timestep. If no, return.
        // if (!person->getOverdose()) {
        //     return;
        // }

        double backgroundMortProb = 0.0;
        double smr = 0.0;
        // double fatalOverdoseProb = 0.0;
        double fibrosisDeathProb = 0.0;

        // 2. Get fatal OD probability.
        this->getMortalityProbabilities(person, backgroundMortProb, smr,
                                        // fatalOverdoseProb,
                                        fibrosisDeathProb);
        // 3. Decide whether the person dies. If not, unset their overdose
        // property.

        double totalProb = (backgroundMortProb * smr) // + fatalOverdoseProb
                           + fibrosisDeathProb;

        std::vector<double> probVec = {(backgroundMortProb * smr),
                                       // fatalOverdoseProb,
                                       fibrosisDeathProb, 1 - totalProb};

        int retIdx = this->getDecision(probVec);
        if (retIdx != 2) {
            this->die(person);
        }
    }

    void Death::die(std::shared_ptr<Person::Person> person) {
        // do the death thing here
        person->die();
    }

    void
    Death::getMortalityProbabilities(std::shared_ptr<Person::Person> const
                                         person, // double &fatalOverdoseProb,
                                     double &backgroundMortProb, double &smr,
                                     double &fibrosisDeathProb) {
        std::unordered_map<std::string, std::string> selectCriteria;

        selectCriteria["age_years"] = std::to_string((int)person->age);
        selectCriteria["gender"] =
            Person::Person::sexEnumToStringMap[person->getSex()];
        selectCriteria["drug_behavior"] =
            Person::Person::behaviorClassificationEnumToStringMap
                [person->getBehaviorClassification()];

        auto resultTable = table->selectWhere(selectCriteria);

        backgroundMortProb =
            std::stod((*resultTable)["background_mortality"][0]);

        smr = std::stod((*resultTable)["SMR"][0]);

        switch (person->getFibrosisState()) {
        case Person::FibrosisState::F4:
            fibrosisDeathProb = this->f4Mort;
            break;
        case Person::FibrosisState::DECOMP:
            fibrosisDeathProb = this->decompMort;
            break;
        default:
            fibrosisDeathProb = 0;
        }

        // if (person->getOverdose()) {
        //     // we need to figure out how we're gonna make this time based
        //     fatalOverdoseProb =
        //         stod((*resultTable)["fatal_overdose_cycle52"][0]);
        // }
    }
} // namespace Event
