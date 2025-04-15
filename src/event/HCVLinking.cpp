////////////////////////////////////////////////////////////////////////////////
// File: HCVLinking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-15                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "HCVLinking.hpp"
#include "Decider.hpp"
#include "Linking.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>

namespace event {
class HCVLinkingIMPL : public LinkingIMPL {
public:
    HCVLinkingIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm)
        : LinkingIMPL(dm) {
        this->INF_TYPE = person::InfectionType::HCV;
        this->LINK_COLUMNS = {
            {person::LinkageType::BACKGROUND, "background_link_probability"},
            {person::LinkageType::INTERVENTION,
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
             link_type < static_cast<int>(person::LinkageType::COUNT);
             ++link_type) {
            person::LinkageType type =
                static_cast<person::LinkageType>(link_type);
            LoadLinkingData(type, dm);
        }
    }
};

HCVLinking::HCVLinking(std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<HCVLinkingIMPL>(dm);
}

HCVLinking::~HCVLinking() = default;
HCVLinking::HCVLinking(HCVLinking &&) noexcept = default;
HCVLinking &HCVLinking::operator=(HCVLinking &&) noexcept = default;

void HCVLinking::DoEvent(std::shared_ptr<person::PersonBase> person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
