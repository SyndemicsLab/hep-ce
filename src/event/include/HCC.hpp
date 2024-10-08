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
/// This file contains the declaration of the HCC Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_HCC_HPP_
#define EVENT_HCC_HPP_

#include "Event.hpp"

namespace event {
    /// @brief Subclass of Event used to process medication for opioid use
    /// disorder
    class HCC : public Event {
    private:
        class HCCIMPL;
        std::unique_ptr<HCCIMPL> impl;
        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) override;

    public:
        HCC(std::shared_ptr<datamanagement::DataManagerBase> dm);
        ~HCC();

        // Copy Operations
        HCC(HCC const &) = delete;
        HCC &operator=(HCC const &) = delete;
        HCC(HCC &&) noexcept;
        HCC &operator=(HCC &&) noexcept;
    };
} // namespace event
#endif // EVENT_HCC_HPP_
