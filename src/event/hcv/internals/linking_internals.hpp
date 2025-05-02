////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_LINKINGINTERNALS_HPP_
#define HEPCE_EVENT_HCV_LINKINGINTERNALS_HPP_

// File Header
#include <hepce/event/hcv/linking.hpp>

// Local Includes
#include "../../internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {
class LinkingImpl : public virtual hcv::Linking, public event::LinkingBase {
public:
    LinkingImpl(datamanagement::ModelData &model_data,
                const std::string &log_name = "console");

    ~LinkingImpl() = default;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    inline data::InfectionType GetInfectionType() const override {
        return data::InfectionType::kHcv;
    }
    inline bool FalsePositive(model::Person &person) override {
        if (person.GetHCVDetails().hcv != data::HCV::kNone) {
            return false;
        }
        person.ClearDiagnosis(data::InfectionType::kHcv);
        AddFalsePositiveCost(person, GetEventCostCategory());
        return true;
    }
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif