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

namespace Event {
    class Fibrosis : public Event {
    private:
        void doEvent(Person::Person &person) override;

    public:
        Fibrosis(){};
        virtual ~Fibrosis() = default;
    };
} // namespace Event

#endif
