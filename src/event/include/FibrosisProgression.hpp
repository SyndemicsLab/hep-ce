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
/// This file contains the declaration of the FibrosisProgression Event
/// Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_FIBROSISPROGRESSION_HPP_
#define EVENT_FIBROSISPROGRESSION_HPP_

#include "Event.hpp"
#include <map>
#include <memory>

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

    /// @brief Subclass of Event used to Progress HCV
    class FibrosisProgression : public Event {
    private:
        class FibrosisProgressionIMPL;
        std::unique_ptr<FibrosisProgressionIMPL> impl;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::unique_ptr<stats::Decider> &decider) override;

    public:
        FibrosisProgression();
        ~FibrosisProgression();

        // Copy Operations
        FibrosisProgression(FibrosisProgression const &) = delete;
        FibrosisProgression &operator=(FibrosisProgression const &) = delete;
        FibrosisProgression(FibrosisProgression &&) noexcept;
        FibrosisProgression &operator=(FibrosisProgression &&) noexcept;
    };
} // namespace event
#endif
