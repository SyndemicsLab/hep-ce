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
/// This file contains the declaration of the Event and ProbEvent Abstract
/// Classes.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef EVENT_EVENT_HPP_
#define EVENT_EVENT_HPP_

#include <memory>
#include <random>
#include <vector>

// Forward Defining Person to use in Execute
namespace person {
    class Person;
}

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    class EventBase {
    private:
        class Event;
        class ProbEvent;
        std::unique_ptr<Event> pImplEVENT;
        std::unique_ptr<ProbEvent> pImplPROBEVENT;

    public:
        EventBase(std::unique_ptr<std::mt19937_64> generator = NULL,
                  std::string dataquery = "", std::string name = "");
        ~EventBase() = default;
        int Execute(person::Person &person);
    };

} // namespace event
#endif
