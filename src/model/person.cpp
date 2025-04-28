////////////////////////////////////////////////////////////////////////////////
// File: person.cpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created Date: Mo Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/person.hpp>

#include "internals/person_internals.hpp"

namespace hepce {
namespace model {
namespace person {
PersonImpl::PersonImpl(const std::string &log_name) {}

void PersonImpl::InfectHCV() {
    // cannot be multiply infected
    if (_hcv_details.hcv != data::HCV::kNone) {
        return;
    }
    _hcv_details.hcv = data::HCV::kAcute;
    _hcv_details.time_changed = _current_time;
    _hcv_details.seropositive = true;
    _hcv_details.times_infected++;

    // once infected, immediately enter F0
    if (_hcv_details.fibrosis_state == data::FibrosisState::kNone) {
        UpdateTrueFibrosis(data::FibrosisState::kF0);
    }
}

void PersonImpl::UpdateTimers() {
    _current_time++;
    if (_behavior_details.behavior == data::Behavior::kNoninjection ||
        _behavior_details.behavior == data::Behavior::kInjection) {
        _behavior_details.time_last_active = _current_time;
    }
    if (_moud_details.moud_state == data::MOUD::kCurrent) {
        _moud_details.total_moud_months++;
    }
    if (_hiv_details.hiv == data::HIV::LoUn ||
        _hiv_details.hiv == data::HIV::LoSu) {
        _hiv_details.low_cd4_months_count++;
    }
    _moud_details.current_state_concurrent_months++;
}
} // namespace person
} // namespace model
} // namespace hepce