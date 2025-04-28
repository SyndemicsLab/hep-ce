////////////////////////////////////////////////////////////////////////////////
// File: staging.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_FIBROSIS_STAGING_HPP_
#define HEPCE_EVENT_FIBROSIS_STAGING_HPP_

#include <memory>
#include <string>

#include <hepce/event/event.hpp>

namespace hepce {
namespace event {
namespace fibrosis {
class Staging : public virtual Event {
public:
    virtual ~Staging() = default;

    static std::unique_ptr<Event>
    Create(datamanagement::ModelData &model_data,
           const std::string &log_name = "console");
};
} // namespace fibrosis
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_FIBROSIS_STAGING_HPP_