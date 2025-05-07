////////////////////////////////////////////////////////////////////////////////
// File: simulation.cpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-22                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/simulation.hpp>

#include <hepce/event/all_events.hpp>
#include <hepce/utils/config.hpp>
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>

#include "internals/simulation_internals.hpp"

namespace hepce {
namespace model {

//Factory
std::unique_ptr<Hepce> Hepce::Create(const std::string &log_name) {
    return std::make_unique<HepceImpl>(log_name);
}

// Constructor
HepceImpl::HepceImpl(const std::string &log_name) : _log_name(log_name) {}

void HepceImpl::Run(
    std::vector<model::Person> &people,
    const std::vector<std::unique_ptr<event::Event>> &discrete_events,
    const int duration, const int seed) {
    auto sampler = hepce::model::Sampler::Create(seed, GetLogName());
    for (int i = 0; i < duration; ++i) {
        for (auto &&event : discrete_events) {
            for (auto person : people) {
                event->Execute(person, *sampler);
            }
        }
    }
}

std::vector<std::unique_ptr<event::Event>>
HepceImpl::CreateEvents(datamanagement::ModelData &model_data) const {
    std::vector<std::unique_ptr<event::Event>> events;
    auto event_strings = utils::SplitToVecT<std::string>(
        utils::GetStringFromConfig("simulation.events", model_data), ',');
    for (std::string e : event_strings) {
        events.push_back(CreateEvent(e, model_data));
    }
    return events;
}

int HepceImpl::CreatePersonFromTable(
    model::Person &person, datamanagement::ModelData &model_data) const {
    return 0;
}
int HepceImpl::LoadICValues(model::Person &person,
                            const std::vector<std::string> &icValues) {
    return 0;
}

std::unique_ptr<event::Event>
HepceImpl::CreateEvent(std::string event_name,
                       datamanagement::ModelData &model_data) const {
    if (event_name == "Aging") {
        return hepce::event::base::Aging::Create(model_data, GetLogName());
    }
    if (event_name == "BehaviorChanges") {
        return hepce::event::behavior::BehaviorChanges::Create(model_data,
                                                               GetLogName());
    }
    if (event_name == "Clearance") {
        return hepce::event::hcv::Clearance::Create(model_data, GetLogName());
    }
    if (event_name == "Death") {
        return hepce::event::base::Death::Create(model_data, GetLogName());
    }
    if (event_name == "FibrosisProgression") {
        return hepce::event::fibrosis::Progression::Create(model_data,
                                                           GetLogName());
    }
    if (event_name == "FibrosisStaging") {
        return hepce::event::fibrosis::Staging::Create(model_data,
                                                       GetLogName());
    }
    if (event_name == "HCVInfections") {
        return hepce::event::hcv::Infection::Create(model_data, GetLogName());
    }
    if (event_name == "HCVLinking") {
        return hepce::event::hcv::Linking::Create(model_data, GetLogName());
    }
    if (event_name == "HCVScreening") {
        return hepce::event::hcv::Screening::Create(model_data, GetLogName());
    }
    if (event_name == "HCVTreatment") {
        return hepce::event::hcv::Treatment::Create(model_data, GetLogName());
    }
    if (event_name == "HIVInfections") {
        return hepce::event::hiv::Infection::Create(model_data, GetLogName());
    }
    if (event_name == "HIVLinking") {
        return hepce::event::hiv::Linking::Create(model_data, GetLogName());
    }
    if (event_name == "HIVScreening") {
        return hepce::event::hiv::Screening::Create(model_data, GetLogName());
    }
    if (event_name == "Overdose") {
        return hepce::event::behavior::Overdose::Create(model_data,
                                                        GetLogName());
    }
    if (event_name == "VoluntaryRelinking") {
        return hepce::event::hcv::VoluntaryRelink::Create(model_data,
                                                          GetLogName());
    }
    if (event_name == "Pregnancy") {
        return hepce::event::behavior::Pregnancy::Create(model_data,
                                                         GetLogName());
    }
    if (event_name == "MOUD") {
        return hepce::event::behavior::Moud::Create(model_data, GetLogName());
    }
    hepce::utils::LogError(GetLogName(),
                           "Unknown Event Provided in sim.conf! Exiting...");
    exit(-1);
}
} // namespace model
} // namespace hepce