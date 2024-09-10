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
/// This file contains the implementation of the Clearance Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Clearance.hpp"

namespace event {
    Clearance::Clearance(std::mt19937_64 &generator, Data::IDataTablePtr table,
                         Data::Config &config,
                         std::shared_ptr<spdlog::logger> logger,
                         std::string name)
        : ProbEvent(generator, table, config, logger, name) {
        std::shared_ptr<Data::ReturnType> clearance =
            this->config.get_optional("infection.clearance_prob", (double)-1.0);
        if (clearance) {
            // if the user provides a clearance probability, use that value
            // instead
            this->clearanceProb = std::get<double>(*clearance);
        } else {
            // it's basically universally accepted that 25% of acute hcv
            // infections clear in the 6-month acute infection period
            this->clearanceProb = Utils::probabilityToRate(0.25) / 6.0;
        }
    }

    void Clearance::doEvent(std::shared_ptr<person::Person> person) {
        // people infected with hcv have some probability of spontaneous
        // clearance.

        // if person isn't infected or is chronic, nothing to do
        if (person->getHCV() != person::HCV::ACUTE) {
            return;
        }
        // 1. Get the probability of acute clearance
        std::vector<double> prob = this->getClearanceProb();
        // 2. Decide whether the person clears
        int doesNotClear = this->getDecision(prob);
        // if you do not clear, return immediately
        if (doesNotClear) {
            return;
        }
        person->clearHCV(this->getCurrentTimestep());
    }

    std::vector<double> Clearance::getClearanceProb() {
        return {this->clearanceProb};
    }
} // namespace event
