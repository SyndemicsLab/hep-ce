////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-18                                                  //
// Modified By: Dimitri Baptiste                                              //
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
class LinkingImpl : public virtual Linking, public LinkingBase {
public:
    LinkingImpl(datamanagement::ModelData &model_data,
                const std::string &log_name = "console");

    ~LinkingImpl() = default;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
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
        person.ClearDiagnosis(data::InfectionType::kHcv, true);
        AddFalsePositiveCost(person, GetEventCostCategory());
        return true;
    }
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif
