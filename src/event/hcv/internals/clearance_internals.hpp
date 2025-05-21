////////////////////////////////////////////////////////////////////////////////
// File: clearance_internals.hpp                                              //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_CLEARANCEINTERNALS_HPP_
#define HEPCE_EVENT_HCV_CLEARANCEINTERNALS_HPP_

// File Header
#include <hepce/event/hcv/clearance.hpp>

// Local Includes
#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {
class ClearanceImpl : public virtual Clearance, public EventBase {
public:
    ClearanceImpl(datamanagement::ModelData &model_data,
                  const std::string &log_name = "console");

    ~ClearanceImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    double _probability = 0.0;
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_CLEARANCEINTERNALS_HPP_