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
/// This file contains the declaration of the MOUD Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_MOUD_HPP_
#define EVENT_MOUD_HPP_

#include "Event.hpp"

namespace event {
/// @brief Subclass of Event used to process medication for opioid use
/// disorder
class MOUD : public Event {
private:
    class MOUDIMPL;
    std::unique_ptr<MOUDIMPL> impl;
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    MOUD(std::shared_ptr<datamanagement::DataManagerBase> dm);
    ~MOUD();

    // Copy Operations
    MOUD(MOUD const &) = delete;
    MOUD &operator=(MOUD const &) = delete;
    MOUD(MOUD &&) noexcept;
    MOUD &operator=(MOUD &&) noexcept;
};
} // namespace event
#endif // EVENT_MOUD_HPP_
