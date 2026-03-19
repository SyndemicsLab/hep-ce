////////////////////////////////////////////////////////////////////////////////
// File: event.hpp                                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_EVENT_HPP_
#define HEPCE_EVENT_EVENT_HPP_

#include <memory>
#include <string>
#include <vector>

#include <hepce/data/inputs.hpp>

#include <hepce/model/person.hpp>
#include <hepce/model/sampler.hpp>

namespace hepce {
namespace event {
class Event {
public:
    virtual ~Event() = default;
    Event(const Event &) = delete;
    Event &operator=(const Event &) = delete;
    virtual std::unique_ptr<Event> clone() const = 0;

    virtual bool ValidExecute(model::Person &person) const = 0;
    virtual void Execute(model::Person &person, model::Sampler &sampler) = 0;
    virtual void LoadData(data::Inputs &inputs) = 0;

protected:
    Event() = default;
};

using EventList = std::vector<std::unique_ptr<Event>>;
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_EVENT_HPP_