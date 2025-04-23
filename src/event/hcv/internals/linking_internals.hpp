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
#ifndef HEPCE_EVENT_HCV_LINKINGINTERNALS_HPP_
#define HEPCE_EVENT_HCV_LINKINGINTERNALS_HPP_

#include <hepce/event/hcv/linking.hpp>

#include "internals/linking_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {
class LinkingImpl : public virtual hcv::Linking, public event::LinkingBase {
public:
    LinkingImpl(std::shared_ptr<datamanagement::DataManagerBase> dm,
                const std::string &log_name = "console") {
        SetCostCategory(model::CostCategory::kLinking);
        SetLinkingStratifiedByPregnancy(CheckForPregnancyEvent(dm));
        LoadLinkingData(dm);
    }

    ~LinkingImpl() = default;

    int Execute(model::Person &person,
                std::shared_ptr<datamanagement::DataManagerBase> dm,
                model::Sampler &sampler) override {
        SetLinkageType(person.GetLinkageType(data::InfectionType::HCV));
        return 0;
    }

private:
    inline bool FalsePositive(model::Person &person) override {
        if (person.GetHCV() != data::HCV::NONE) {
            return false;
        }
        person.ClearDiagnosis(data::InfectionType::HCV);
        AddFalsePositiveCost(person, GetCostCategory());
        return true;
    }

    void LoadLinkingData(
        std::shared_ptr<datamanagement::DataManagerBase> dm) override {
        std::string error;
        int rc =
            dm->SelectCustomCallback(LinkSQL("screening_and_linkage"),
                                     this->callback_link, &link_data, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "Error retrieving Linking values "
                "for table screening_and_linkage;! Error Message: {}",
                error);
        }
        if (link_data.empty()) {
            spdlog::get("main")->warn("No HCV linking data found.");
        }
    }
};
} // namespace hcv
} // namespace event
} // namespace hepce

#endif