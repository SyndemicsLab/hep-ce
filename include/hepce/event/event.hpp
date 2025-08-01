////////////////////////////////////////////////////////////////////////////////
// File: event.hpp                                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-10                                                  //
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
    virtual bool ValidExecute(model::Person &person) const = 0;
    virtual void Execute(model::Person &person, model::Sampler &sampler) = 0;
    virtual void LoadData(datamanagement::ModelData &model_data) = 0;
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_EVENT_HPP_