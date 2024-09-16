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
    class PersonBase;
}

// Forward Defining DataManager to require in constructor
namespace datamanagement {
    class DataManager;
}

// Forward Defining Decider from stats project
namespace stats {
    class Decider;
}

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    class Event {
    protected:
        std::shared_ptr<datamanagement::DataManager> dm;
        virtual void doEvent(person::PersonBase &person) = 0;

    public:
        const std::string EVENT_NAME;
        Event(std::shared_ptr<datamanagement::DataManager> dm,
              std::string name = std::string("Event"))
            : dm(dm), EVENT_NAME(name) {}
        ~Event() = default;
        int Execute(person::PersonBase &person);
    };
} // namespace event
#endif
