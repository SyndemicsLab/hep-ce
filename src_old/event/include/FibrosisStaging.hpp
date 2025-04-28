////////////////////////////////////////////////////////////////////////////////
// File: FibrosisStaging.hpp                                                  //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

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

    /// @brief Implementation of Virtual Function DoEvent
    /// @param person Individual Person undergoing Event
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) override;

public:
    FibrosisStaging(datamanagement::ModelData &model_data);
    ~FibrosisStaging();

    // Copy Operations
    FibrosisStaging(FibrosisStaging const &) = delete;
    FibrosisStaging &operator=(FibrosisStaging const &) = delete;
    FibrosisStaging(FibrosisStaging &&) noexcept;
    FibrosisStaging &operator=(FibrosisStaging &&) noexcept;
};
} // namespace event

#endif
