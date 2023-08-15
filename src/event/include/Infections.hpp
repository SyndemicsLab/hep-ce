//===-- Infect.hpp - Instruction class definition -------*- C++ -*-===//
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
/// Created Date: Tuesday, August 15th 2023, 1:46:27 pm
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef EVENT_INFECTIONS_HPP_
#define EVENT_INFECTIONS_HPP_

#include "Event.hpp"

namespace Event {

class Infections : public Event {
private:
public:
    Infections() {};
    virtual ~Infections() = default;

    /// @brief Implementation of abstract function, primary entry point for all events
    /// @param population vector containing the people undergoing the simulation
    /// @param timestep current timestep of the simulation
    /// @return vector of people after the event occurs
    void execute(std::vector<Person::Person> &population, int timestep) override;
};

}

#endif