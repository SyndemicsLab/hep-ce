////////////////////////////////////////////////////////////////////////////////
// File: HCVLinking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "HCVLinking.hpp"
#include "Decider.hpp"
#include "Linking.hpp"
#include "spdlog/spdlog.h"
#include <datamanagement/datamanagement.hpp>

namespace event {
class HCVLinkingIMPL : public LinkingIMPL {
public:
    HCVLinkingIMPL(datamanagement::ModelData &model_data) : LinkingIMPL(dm) {
        this->INF_TYPE = person:: ::kHcv;
        this->LINK_COLUMNS = {
            {person::LinkageType::kBackground, "background_link_probability"},
            {person::LinkageType::kIntervention,
             "intervention_link_probability"}};

        this->intervention_cost =
            Utils::GetDoubleFromConfig("linking.intervention_cost", dm);
        this->false_positive_test_cost =
            Utils::GetDoubleFromConfig("linking.false_positive_test_cost", dm);
        this->recent_screen_multiplier =
            Utils::GetDoubleFromConfig("linking.recent_screen_multiplier", dm);
        this->recent_screen_cutoff =
            Utils::GetIntFromConfig("linking.recent_screen_cutoff", dm);

        for (int link_type = 0;
             link_type < static_cast<int>(person::LinkageType::kCount);
             ++link_type) {
            person::LinkageType type =
                static_cast<person::LinkageType>(link_type);
            LoadLinkingData(type, dm);
        }
    }
};

HCVLinking::HCVLinking(datamanagement::ModelData &model_data) {
    impl = std::make_unique<HCVLinkingIMPL>(dm);
}

HCVLinking::~HCVLinking() = default;
HCVLinking::HCVLinking(HCVLinking &&) noexcept = default;
HCVLinking &HCVLinking::operator=(HCVLinking &&) noexcept = default;

void HCVLinking::DoEvent(std::shared_ptr<person::PersonBase> person,
                         datamanagement::ModelData &model_data,
                         std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
