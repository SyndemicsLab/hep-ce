////////////////////////////////////////////////////////////////////////////////
// File: moud.cpp                                                             //
// Project: hep-ce                                                            //
// Created Date: 2025-05-08                                                    //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-10                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/behavior/moud.hpp>

#include "internals/moud_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {
// Factory
std::unique_ptr<Event> Moud::Create(datamanagement::ModelData &model_data,
                                    const std::string &log_name) {
    return std::make_unique<MoudImpl>(model_data, log_name);
}

MoudImpl::MoudImpl(datamanagement::ModelData &model_data,
                   const std::string &log_name)
    : EventBase(model_data, log_name) {
    LoadData(model_data);
}

void MoudImpl::Execute(model::Person &person, model::Sampler &sampler) {
    if (!ValidExecute(person)) {
        return;
    }
}

void MoudImpl::LoadData(datamanagement::ModelData &model_data) {}
} // namespace behavior
} // namespace event
} // namespace hepce