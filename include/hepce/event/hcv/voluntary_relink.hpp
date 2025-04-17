////////////////////////////////////////////////////////////////////////////////
// File: voluntary_relink.hpp                                                 //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_VOLUNTARYRELINK_HPP_
#define HEPCE_EVENT_HCV_VOLUNTARYRELINK_HPP_

#include <memory>
#include <string>

#include <hepce/event/event.hpp>

namespace hepce {
namespace event {
namespace hcv {
class VoluntaryRelink : public virtual Event {
public:
    virtual ~VoluntaryRelink() = default;

    static std::unique_ptr<Event>
    Create(std::shared_ptr<datamanagement::DataManagerBase> dm,
           const std::string &log_name = "console");
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_VOLUNTARYRELINKS_HPP_