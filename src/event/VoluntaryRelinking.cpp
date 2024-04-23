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
/// This file contains the implementation of the VoluntaryRelinking Event
/// Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "VoluntaryRelinking.hpp"
#include <string>

namespace Event {
    void VoluntaryRelinking::doEvent(std::shared_ptr<Person::Person> person) {
        if (!person->getIsAlive()) {
            return;
        }

        double relinkProbability =
            config.get("linking.voluntary_relinkage_probability", 0.0);

        int relink = this->getDecision({relinkProbability});

        if (person->getLinkState() != Person::LinkageState::UNLINKED ||
            (this->getCurrentTimestep() - person->getTimeOfLinkChange()) >
                this->voluntaryRelinkDuration ||
            relink == 1) {
            return; // if linked or never linked OR too long since last linked
                    // OR relink draw is false
        }
        person->link(this->getCurrentTimestep(),
                     Person::LinkageType::BACKGROUND);
        // This is sorta a place for a background screening?
    }
} // namespace Event
