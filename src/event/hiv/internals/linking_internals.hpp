////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_LINKINGINTERNALS_HPP_
#define HEPCE_EVENT_HIV_LINKINGINTERNALS_HPP_

#include <hepce/event/hiv/linking.hpp>

#include "internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
class LinkingImpl : public virtual hiv::Linking, public event::LinkingBase {
public:
    LinkingImpl(datamanagement::ModelData &model_data,
                const std::string &log_name = "console");

    ~LinkingImpl() = default;

private:
    inline bool FalsePositive(model::Person &person) override {
        if (person.GetHIVDetails().hiv != data::HIV::kNone) {
            return false;
        }
        person.ClearDiagnosis(data::InfectionType::kHcv);
        AddFalsePositiveCost(person, GetEventCostCategory());
        return true;
    }
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif