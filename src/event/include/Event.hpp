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

// Forward Defining DataManagerBase to require in constructor
namespace datamanagement {
    class DataManagerBase;
}

// Forward Defining Decider from stats project
namespace stats {
    class Decider;
}

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    class Event {
    protected:
        virtual void
        doEvent(person::PersonBase &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                std::unique_ptr<stats::Decider> &decider) = 0;

    public:
        Event();
        ~Event();

        // Copy Operations
        Event(Event const &) = delete;
        Event &operator=(Event const &) = delete;
        Event(Event &&) noexcept;
        Event &operator=(Event &&) noexcept;

        int Execute(person::PersonBase &person,
                    std::shared_ptr<datamanagement::DataManagerBase> dm,
                    std::unique_ptr<stats::Decider> &decider);
    };
} // namespace event
#endif
