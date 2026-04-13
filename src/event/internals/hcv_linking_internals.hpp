////////////////////////////////////////////////////////////////////////////////
// File: hcv_linking_internals.hpp                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_LINKINGINTERNALS_HPP_
#define HEPCE_EVENT_HCV_LINKINGINTERNALS_HPP_

// Local Includes
#include "base_linking_internals.hpp"

namespace hepce {
namespace event {
class HCVLinking : public virtual LinkingBase {
public:
    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    HCVLinking(const data::Inputs &inputs, const std::string &log_name)
        : EventBase("hcv_linking", inputs, log_name) {
        LoadData();
    }

    ~HCVLinking() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<HCVLinking>(GetInputs(), GetLogName());
    }

private:
    void LoadData();

    inline const std::string TableName() const override {
        return "screening_and_linkage";
    }

    inline data::InfectionType GetInfectionType() const override {
        return data::InfectionType::kHcv;
    }
    inline bool FalsePositive(model::Person &person) override {
        if (person.GetHCVDetails().hcv != data::HCV::kNone) {
            return false;
        }
        person.FalsePositive(GetInfectionType());
        AddFalsePositiveCost(person, GetCostCategory());
        return true;
    }
};
} // namespace event
} // namespace hepce

#endif
