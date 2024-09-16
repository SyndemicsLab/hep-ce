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
        std::shared_ptr<FibrosisStagingIMPL> impl;
        std::shared_ptr<stats::Decider> decider;

        double testOneCost = 0;
        double testTwoCost = 0;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person) override;

    public:
        FibrosisStaging(std::shared_ptr<stats::Decider> decider,
                        std::shared_ptr<datamanagement::DataManager> dm,
                        std::string name = std::string("FibrosisStaging"));
        virtual ~FibrosisStaging() = default;
    };
} // namespace event

#endif
