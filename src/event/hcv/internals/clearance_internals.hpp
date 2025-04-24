////////////////////////////////////////////////////////////////////////////////
// File: clearance_internals.hpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_CLEARANCEINTERNALS_HPP_
#define HEPCE_EVENT_HCV_CLEARANCEINTERNALS_HPP_

#include <hepce/event/hcv/clearance.hpp>

#include "internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {
class ClearanceImpl : public virtual Clearance, public EventBase {
public:
    ClearanceImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                  const std::string &log_name = "console");

    ~ClearanceImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override;

private:
    double _probability = 0.0;
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_CLEARANCEINTERNALS_HPP_