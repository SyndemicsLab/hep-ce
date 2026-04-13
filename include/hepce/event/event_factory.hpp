////////////////////////////////////////////////////////////////////////////////
// File: event_factory.hpp                                                    //
// Project: hep-ce                                                            //
// Created Date: 2026-03-19                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_EVENT_FACTORY_HPP_
#define HEPCE_EVENT_EVENT_FACTORY_HPP_

#include <memory>

#include <hepce/data/inputs.hpp>

#include <hepce/event/event.hpp>

namespace hepce {
namespace event {
class EventFactory {
public:
    static std::unique_ptr<Event> CreateEvent(const std::string &name,
                                              const data::Inputs &inputs,
                                              const std::string &log_name);
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_EVENT_FACTORY_HPP_