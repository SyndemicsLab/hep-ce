////////////////////////////////////////////////////////////////////////////////
// File: hiv_linking_internals.hpp                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-03                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_LINKINGINTERNALS_HPP_
#define HEPCE_EVENT_HIV_LINKINGINTERNALS_HPP_

// Local Includes
#include "base_linking_internals.hpp"

namespace hepce {
namespace event {
class HIVLinking : public virtual LinkingBase {
public:
    // Factory
    static std::unique_ptr<Event> Create(const data::Inputs &inputs,
                                         const std::string &log_name);

    HIVLinking(const data::Inputs &inputs, const std::string &log)
        : EventBase("hiv_linking", inputs, log) {
        LoadData();
    }

    ~HIVLinking() = default;

    // Cloning
    std::unique_ptr<Event> clone() const override {
        return std::make_unique<HIVLinking>(GetInputs(), GetLogName());
    }

private:
    void LoadData();

    inline data::InfectionType GetInfectionType() const override {
        return data::InfectionType::kHiv;
    }

    inline const std::string TableName() const override {
        return "screening_and_linkage";
    }

    inline bool FalsePositive(model::Person &person) override {
        if (person.GetHIVDetails().hiv != data::HIV::kNone) {
            return false;
        }
        person.ClearDiagnosis(GetInfectionType());
        AddFalsePositiveCost(person, GetCostCategory());
        return true;
    }
};
} // namespace event
} // namespace hepce

#endif