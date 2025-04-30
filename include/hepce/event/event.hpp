////////////////////////////////////////////////////////////////////////////////
// File: event.hpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_EVENT_HPP_
#define HEPCE_EVENT_EVENT_HPP_

#include <memory>
#include <string>

#include <datamanagement/datamanagement.hpp>

#include <hepce/model/person.hpp>
#include <hepce/model/sampler.hpp>

namespace hepce {
namespace event {
class Event {
public:
    virtual ~Event() = default;
    virtual int Execute(model::Person &person, model::Sampler &sampler) = 0;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_EVENT_HPP_