////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_LINKINGINTERNALS_HPP_
#define HEPCE_EVENT_HCV_LINKINGINTERNALS_HPP_

#include <hepce/event/hcv/linking.hpp>

#include "internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {
class LinkingImpl : public virtual hcv::Linking, public event::LinkingBase {
public:
    LinkingImpl(datamanagement::ModelData &model_data,
                const std::string &log_name = "console");

    ~LinkingImpl() = default;

    data::InfectionType GetInfectionType() const override {
        return data:: ::kHcv;
    }

private:
    inline bool FalsePositive(model::Person &person) override {
        if (person.GetHCV() != data::HCV::kNone) {
            return false;
        }
        person.ClearDiagnosis(data:: ::kHcv);
        AddFalsePositiveCost(person, GetCostCategory());
        return true;
    }
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif