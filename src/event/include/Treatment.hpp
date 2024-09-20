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
/// This file contains the declaration of the Treatment Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_TREATMENT_HPP_
#define EVENT_TREATMENT_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    /// @brief Subclass of Event used to Provide Treatment to People
    class Treatment : public Event {
    private:
        class TreatmentIMPL;
        std::unique_ptr<TreatmentIMPL> impl;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) override;

    public:
        Treatment();
        ~Treatment();

        // Copy Operations
        Treatment(Treatment const &) = delete;
        Treatment &operator=(Treatment const &) = delete;
        Treatment(Treatment &&) noexcept;
        Treatment &operator=(Treatment &&) noexcept;
    };
} // namespace event
#endif
