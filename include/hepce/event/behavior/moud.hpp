////////////////////////////////////////////////////////////////////////////////
// File: moud.hpp                                                             //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_MOUD_HPP_
#define HEPCE_EVENT_BEHAVIOR_MOUD_HPP_

#include <memory>
#include <string>

#include <hepce/event/event.hpp>

namespace hepce {
namespace event {
namespace behavior {
class Moud : public virtual Event {
public:
    virtual ~Moud() = default;

    static std::unique_ptr<Event>
    Create(datamanagement::ModelData &model_data,
           const std::string &log_name = "console");
};
} // namespace behavior
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BEHAVIOR_MOUD_HPP_