//===-- Event.hpp - Instruction class definition -------*- C++ -*-===//
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
/// Created Date: Wednesday, August 2nd 2023, 9:44:04 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef EVENT_EVENT_HPP_
#define EVENT_EVENT_HPP_

#include "Person.hpp"

#include <vector>

/// @brief Namespace containing the Events that occur during the simulation
namespace Event{

/// @brief Abstract class that superclasses all Events. Contains execute function definition
class Event {
private:

public:
    Event(){};
    virtual ~Event() = default;

    /// @brief Primary function required in all Event subclasses. Used to run the event
    /// @param  Population vector containing all the people in the simulation
    /// @param  timestep integer containing the current timestep of the simulation
    /// @return The population vector after the event is executed
    virtual std::vector<Person::Person> execute(std::vector<Person::Person>, int) = 0;
};

}
#endif