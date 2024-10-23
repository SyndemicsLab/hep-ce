//===-- Fibrosis.hpp - Instruction class definition -------*- C++ -*-===//
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
/// Created Date: Tuesday, August 15th 2023, 1:53:22 pm
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_FIBROSIS_HPP_
#define EVENT_FIBROSIS_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {

    /// @brief Subclass of Event used to Progress Fibrosis Stages
    class FibrosisStaging : public Event {
    private:
        class FibrosisStagingIMPL;
        std::unique_ptr<FibrosisStagingIMPL> impl;

        double testOneCost = 0;
        double testTwoCost = 0;
        /// @brief Implementation of Virtual Function DoEvent
        /// @param person Individual Person undergoing Event
        void DoEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) override;

    public:
        FibrosisStaging(std::shared_ptr<datamanagement::DataManagerBase> dm);
        ~FibrosisStaging();

        // Copy Operations
        FibrosisStaging(FibrosisStaging const &) = delete;
        FibrosisStaging &operator=(FibrosisStaging const &) = delete;
        FibrosisStaging(FibrosisStaging &&) noexcept;
        FibrosisStaging &operator=(FibrosisStaging &&) noexcept;
    };
} // namespace event

#endif
