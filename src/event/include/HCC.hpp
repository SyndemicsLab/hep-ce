////////////////////////////////////////////////////////////////////////////////
// File: HCC.hpp                                                              //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

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
    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
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
