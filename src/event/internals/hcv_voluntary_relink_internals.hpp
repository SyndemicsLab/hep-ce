////////////////////////////////////////////////////////////////////////////////
// File: hcv_voluntary_relink_internals.hpp                                   //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_VOLUNTARYRELINKINTERNALS_HPP_
#define HEPCE_EVENT_HCV_VOLUNTARYRELINKINTERNALS_HPP_

// Local Includes
#include "base_event_internals.hpp"

namespace hepce {
namespace event {

class VoluntaryRelink : public virtual EventBase {
public:
    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    VoluntaryRelink(const data::Inputs &inputs, const std::string &log)
        : EventBase("voluntary_relink", inputs, log) {
        LoadData();
    }

    ~VoluntaryRelink() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<VoluntaryRelink>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

private:
    double _relink_probability = 0.0;
    int _voluntary_relink_duration = 0.0;
    double _cost = 0.0;

    void LoadData();

    inline bool RelinkInTime(const model::Person &person) const {
        return (GetTimeSince(person,
                             person.GetLinkageDetails(data::InfectionType::kHcv)
                                 .time_link_change) <
                _voluntary_relink_duration);
    }

    inline bool Unlinked(const model::Person &person) const {
        return (
            person.GetLinkageDetails(data::InfectionType::kHcv).link_state ==
            data::LinkageState::kUnlinked);
    }
};

} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_VOLUNTARYRELINKINTERNALS_HPP_