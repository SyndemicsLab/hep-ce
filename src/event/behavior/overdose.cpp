////////////////////////////////////////////////////////////////////////////////
// File: overdose.cpp                                                         //
// Project: hep-ce                                                            //
// Created Date: 2025-05-08                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-08                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/event/behavior/overdose.hpp>

#include "internals/overdose_internals.hpp"

namespace hepce {
namespace event {
namespace behavior {
// Factory
std::unique_ptr<hepce::event::Event>
Overdose::Create(datamanagement::ModelData &model_data,
                 const std::string &log_name) {
    return std::make_unique<OverdoseImpl>(model_data, log_name);
}

OverdoseImpl::OverdoseImpl(datamanagement::ModelData &model_data,
                           const std::string &log_name)
    : EventBase(model_data, log_name) {
    LoadData(model_data);
}

void OverdoseImpl::Execute(model::Person &person, model::Sampler &sampler) {}

void OverdoseImpl::LoadData(datamanagement::ModelData &model_data) {}
} // namespace behavior
} // namespace event
} // namespace hepce