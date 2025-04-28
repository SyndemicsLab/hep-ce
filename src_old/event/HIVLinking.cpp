////////////////////////////////////////////////////////////////////////////////
// File: HIVLinking.cpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-25                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVLinking.hpp"
#include "Decider.hpp"
#include "Linking.hpp"
#include "spdlog/spdlog.h"
#include <datamanagement/datamanagement.hpp>
#include <string>

namespace event {
class HIVLinkingIMPL : public LinkingIMPL {
public:
    HIVLinkingIMPL(datamanagement::ModelData &model_data) : LinkingIMPL(dm) {
        this->INF_TYPE = person::InfectionType::HIV;
        this->LINK_COLUMNS = {{person::LinkageType::BACKGROUND,
                               "hiv_background_link_probability"},
                              {person::LinkageType::INTERVENTION,
                               "hiv_intervention_link_probability"}};
        this->COST_CATEGORY = cost::CostCategory::HIV;

        this->intervention_cost =
            Utils::GetDoubleFromConfig("hiv_linking.intervention_cost", dm);
        this->false_positive_test_cost = Utils::GetDoubleFromConfig(
            "hiv_linking.false_positive_test_cost", dm);
        this->recent_screen_multiplier = Utils::GetDoubleFromConfig(
            "hiv_linking.recent_screen_multiplier", dm);
        this->recent_screen_cutoff =
            Utils::GetIntFromConfig("hiv_linking.recent_screen_cutoff", dm);

        for (int link_type = 0;
             link_type < static_cast<int>(person::LinkageType::COUNT);
             ++link_type) {
            person::LinkageType type =
                static_cast<person::LinkageType>(link_type);
            LoadLinkingData(type, dm);
        }
    }
};

HIVLinking::HIVLinking(datamanagement::ModelData &model_data) {
    impl = std::make_unique<HIVLinkingIMPL>(dm);
}
HIVLinking::~HIVLinking() = default;
HIVLinking::HIVLinking(HIVLinking &&) noexcept = default;
HIVLinking &HIVLinking::operator=(HIVLinking &&) noexcept = default;
void HIVLinking::DoEvent(std::shared_ptr<person::PersonBase> person,
                         datamanagement::ModelData &model_data,
                         std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
