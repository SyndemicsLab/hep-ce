////////////////////////////////////////////////////////////////////////////////
// File: moud.cpp                                                             //
// Project: hep-ce                                                            //
// Created Date: 2025-05-08                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-09                                                  //
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
    if (!ValidExecute(person) || !HistoryOfOud(person)) {
        return;
    }
    // Draw Transition Probability, increment months or start/stop
    switch (person.GetMoudDetails().moud_state) {
    case data::MOUD::kPost:
        person.SetMoudState(data::MOUD::kNone);
        break;
    case data::MOUD::kCurrent:
        break;
    case data::MOUD::kNone:
        break;
    default:
        break;
    }
}

void MoudImpl::LoadData(datamanagement::ModelData &model_data) {}

bool MoudImpl::HistoryOfOud(const model::Person &person) const {
    return (person.GetBehaviorDetails().behavior != data::Behavior::kNever);
}

bool MoudImpl::ActiveOud(const model::Person &person) const {
    data::Behavior b = person.GetBehaviorDetails().behavior;
    return (b == data::Behavior::kInjection || b == data::Behavior::kInjection);
}
} // namespace behavior
} // namespace event
} // namespace hepce
