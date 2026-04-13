////////////////////////////////////////////////////////////////////////////////
// File: hcv_screening_internals.hpp                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_SCREENINGINTERNALS_HPP_
#define HEPCE_EVENT_HCV_SCREENINGINTERNALS_HPP_

// STL Libraries
#include <functional>

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "base_screening_internals.hpp"

namespace hepce {
namespace event {
class HCVScreening : public virtual ScreeningBase {
public:
    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    HCVScreening(const data::Inputs &inputs, const std::string &log_name)
        : EventBase("hcv_screening", inputs, log_name) {
        LoadData();
    }

    ~HCVScreening() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<HCVScreening>(GetInputs(), GetLogName());
    }

private:
    void LoadData();
    data::InfectionType GetInfectionType() const override {
        return data::InfectionType::kHcv;
    }
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_SCREENINGINTERNALS_HPP_