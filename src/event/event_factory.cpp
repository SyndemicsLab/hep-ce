////////////////////////////////////////////////////////////////////////////////
// File: event_factory.cpp                                                    //
// Project: hep-ce                                                            //
// Created Date: 2026-03-19                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/event/event_factory.hpp>

#include ""

namespace hepce {
namespace event {
std::unique_ptr<Event>
EventFactory::CreateEvent(const std::string &name,
                          datamanagement::ModelData &model_data,
                          const std::string &log_name) {
    if (name == "Aging") {
        return hepce::event::base::Aging::Create(model_data, log_name);
    }
    if (name == "BehaviorChanges") {
        return hepce::event::behavior::BehaviorChanges::Create(model_data,
                                                               log_name);
    }
    if (name == "Clearance") {
        return hepce::event::hcv::Clearance::Create(model_data, log_name);
    }
    if (name == "Death") {
        return hepce::event::base::Death::Create(model_data, log_name);
    }
    if (name == "FibrosisProgression") {
        return hepce::event::fibrosis::Progression::Create(model_data,
                                                           log_name);
    }
    if (name == "FibrosisStaging") {
        return hepce::event::fibrosis::Staging::Create(model_data, log_name);
    }
    if (name == "HCVInfection") {
        return hepce::event::hcv::Infection::Create(model_data, log_name);
    }
    if (name == "HCVLinking") {
        return hepce::event::hcv::Linking::Create(model_data, log_name);
    }
    if (name == "HCVScreening") {
        return hepce::event::hcv::Screening::Create(model_data, log_name);
    }
    if (name == "HCVTreatment") {
        return hepce::event::hcv::Treatment::Create(model_data, log_name);
    }
    if (name == "HIVInfection") {
        return hepce::event::hiv::Infection::Create(model_data, log_name);
    }
    if (name == "HIVLinking") {
        return hepce::event::hiv::Linking::Create(model_data, log_name);
    }
    if (name == "HIVScreening") {
        return hepce::event::hiv::Screening::Create(model_data, log_name);
    }
    if (name == "Overdose") {
        return hepce::event::behavior::Overdose::Create(model_data, log_name);
    }
    if (name == "VoluntaryRelinking") {
        return hepce::event::hcv::VoluntaryRelink::Create(model_data, log_name);
    }
    if (name == "Pregnancy") {
        return hepce::event::behavior::Pregnancy::Create(model_data, log_name);
    }
    if (name == "MOUD") {
        return hepce::event::behavior::Moud::Create(model_data, log_name);
    }
    return nullptr;
}
} // namespace event
} // namespace hepce