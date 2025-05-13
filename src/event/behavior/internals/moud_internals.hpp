////////////////////////////////////////////////////////////////////////////////
// File: moud_internals.hpp                                                   //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_BEHAVIOR_MOUD_INTERNALS_HPP_
#define HEPCE_EVENT_BEHAVIOR_MOUD_INTERNALS_HPP_

#include <hepce/event/behavior/moud.hpp>

#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {
class MoudImpl : public virtual Moud, public EventBase {
public:
    MoudImpl(datamanagement::ModelData &model_data,
             const std::string &log_name = "console");
    ~MoudImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;
};
} // namespace behavior
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_BEHAVIOR_MOUD_INTERNALS_HPP_