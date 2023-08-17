//===-- VoluntaryRelinking.cpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid 
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, August 15th 2023, 8:51:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "VoluntaryRelinking.hpp"

namespace Event {
    void VoluntaryRelinking::doEvent(Person::Person &person){
        double relinkProbability = 0.5; // Need to read this from a parameter

        std::bernoulli_distribution backgroundProbability(relinkProbability);
        this->generatorMutex.lock();
        int relink = backgroundProbability(this->generator);
        this->generatorMutex.unlock();

        if(
            person.getLinkState() != Person::LinkageState::UNLINKED ||
            (this->getCurrentTimestep() - person.getTimeLinkChange()) > this->voluntaryRelinkDuration ||
            !relink
        ){
            return; // if linked or never linked OR too long since last linked OR relink draw is false
        }

        // This is sorta a place for a background screening?

    }
}