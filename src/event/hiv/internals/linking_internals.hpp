////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HIV_LINKINGINTERNALS_HPP_
#define HEPCE_EVENT_HIV_LINKINGINTERNALS_HPP_

// Header File
#include <hepce/event/hiv/linking.hpp>

// Local Includes
#include "../../internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hiv {
class LinkingImpl : public virtual Linking, public LinkingBase {
public:
    LinkingImpl(datamanagement::ModelData &model_data,
                const std::string &log_name = "console");

    ~LinkingImpl() = default;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
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
        AddFalsePositiveCost(person, GetEventCostCategory());
        return true;
    }
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif