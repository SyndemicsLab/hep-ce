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
/// This file contains the implementation of the Pregnancy Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Pregnancy.hpp"

namespace Event {
    Pregnancy::Pregnancy(std::mt19937_64 &generator, Data::IDataTablePtr table,
                         Data::Config &config,
                         std::shared_ptr<spdlog::logger> logger,
                         std::string name)
        : ProbEvent(generator, table, config, logger, name) {
        this->multDeliveryProb = this->config.get(
            "pregnancy.multiple_delivery_probability", (double)-1.0);

        this->verticalHCVTransProb = this->config.get(
            "pregnancy.vertical_hcv_transition_probability", (double)-1.0);

        this->infantHCVTestedProb = this->config.get(
            "pregnancy.infant_hcv_tested_probability", (double)-1.0);
    }

    void Pregnancy::doEvent(std::shared_ptr<Person::Person> person) {
        // If a person is dead, male, younger than 15, older than 45, or been in
        // postpartum for less than 3 months then skip
        if (!person->getIsAlive() || person->getSex() == Person::Sex::MALE ||
            person->age < 15 || person->age > 45 ||
            (person->getPregnancyState() ==
                 Person::PregnancyState::POSTPARTUM &&
             person->getTimeOfPregnancyChange() < 3)) {
            return;
        }

        // people infected with hcv have some probability of spontaneous
        // pregnancy.

        // if person isn't infected or is chronic, nothing to do
        if (person->getHEPCState() != Person::HEPCState::ACUTE) {
            return;
        }
        // 1. Get the probability of acute pregnancy
        std::vector<double> prob = this->getPregnancyProb();
        // 2. Decide whether the person clears
        int doesNotClear = this->getDecision(prob);
        // if you do not clear, return immediately
        if (doesNotClear) {
            return;
        }
        person->clearHCV(this->getCurrentTimestep());
    }

    std::vector<double> Pregnancy::getPregnancyProb() {
        return {this->pregnancyProb};
    }
} // namespace Event
