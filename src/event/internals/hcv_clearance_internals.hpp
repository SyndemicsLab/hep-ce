////////////////////////////////////////////////////////////////////////////////
// File: hcv_clearance_internals.hpp                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_CLEARANCEINTERNALS_HPP_
#define HEPCE_EVENT_HCV_CLEARANCEINTERNALS_HPP_

// Local Includes
#include "base_event_internals.hpp"

namespace hepce {
namespace event {
class HCVClearance : public virtual EventBase {
public:
    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    HCVClearance(const data::Inputs &inputs, const std::string &log)
        : EventBase("death", inputs, log) {
        LoadData();
    }

    ~HCVClearance() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<HCVClearance>(GetInputs(), GetLogName());
    }

    void Execute(model::Person &person, const model::Sampler &sampler) override;

private:
    double _probability = 0.0;

    void LoadData();
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_CLEARANCEINTERNALS_HPP_