////////////////////////////////////////////////////////////////////////////////
// File: infection.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_INFECTION_HPP_
#define HEPCE_EVENT_HIV_INFECTION_HPP_

#include <memory>
#include <string>

#include <hepce/event/event.hpp>

namespace hepce {
namespace event {
namespace hiv {
class Infection : public virtual Event {
public:
    virtual ~Infection() = default;

    static std::unique_ptr<Event>
    Create(datamanagement::ModelData &model_data,
           const std::string &log_name = "console");
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_INFECTIONS_HPP_