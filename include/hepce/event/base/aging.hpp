////////////////////////////////////////////////////////////////////////////////
// File: aging.hpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BASE_AGING_HPP_
#define HEPCE_EVENT_BASE_AGING_HPP_

#include <memory>
#include <string>

#include <datamanagement/datamanagement.hpp>

#include <hepce/event/event.hpp>

namespace hepce {
namespace event {
namespace base {
class Aging : public virtual Event {
public:
    virtual ~Aging() = default;

    static std::unique_ptr<Event>
    Create(datamanagement::ModelData &model_data,
           const std::string &log_name = "console");
};
} // namespace base
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BASE_AGING_HPP_