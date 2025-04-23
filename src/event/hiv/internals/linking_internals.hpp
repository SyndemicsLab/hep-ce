////////////////////////////////////////////////////////////////////////////////
// File: linking_internals.hpp                                                //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
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
    LinkingImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                const std::string &log_name = "console") {
        SetCostCategory(model::CostCategory::kHiv);
        SetLinkingStratifiedByPregnancy(CheckForPregnancyEvent(dm));
        LoadLinkingData(dm);
    }

    ~LinkingImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override {
        SetLinkageType(person.GetLinkageType(data::InfectionType::HIV));
        return 0;
    }

private:
    inline bool FalsePositive(model::Person &person) override {
        if (person.GetHIV() != data::HIV::NONE) {
            return false;
        }
        person.ClearDiagnosis(data::InfectionType::HCV);
        AddFalsePositiveCost(person, GetCostCategory());
        return true;
    }

    void LoadLinkingData(
        std::shared_ptr<datamanagement::DataManagerBase> dm) override {
        std::string error;
        int rc = dm->SelectCustomCallback(
            LinkSQL("hiv_linking"), this->callback_link, &link_data, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "Error retrieving Linking values "
                "for table hiv_linking;! Error Message: {}",
                error);
        }
        if (link_data.empty()) {
            spdlog::get("main")->warn("No HIV linking data found.");
        }
    }
};
} // namespace hiv
} // namespace event
} // namespace hepce

#endif