////////////////////////////////////////////////////////////////////////////////
// File: hiv_screening_internals.hpp                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-03                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_SCREENINGINTERNALS_HPP_
#define HEPCE_EVENT_HIV_SCREENINGINTERNALS_HPP_

// STL Includes
#include <functional>

// Library Includes
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include "base_screening_internals.hpp"
namespace hepce {
namespace event {
class HIVScreening : public virtual ScreeningBase {
public:
    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    HIVScreening(const data::Inputs &inputs, const std::string &log)
        : EventBase("hiv_screening", inputs, log) {
        LoadData();
    }

    ~HIVScreening() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<HIVScreening>(GetInputs(), GetLogName());
    }

private:
    void LoadData();

    data::InfectionType GetInfectionType() const override {
        return data::InfectionType::kHiv;
    }
};
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HIV_SCREENINGINTERNALS_HPP_