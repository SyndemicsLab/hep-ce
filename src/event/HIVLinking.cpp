////////////////////////////////////////////////////////////////////////////////
// File: HIVLinking.cpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-25                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-15                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVLinking.hpp"
#include "Decider.hpp"
#include "Linking.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <string>

namespace event {
class HIVLinkingIMPL : public LinkingIMPL {
private:
    // constants
    // redefining cost category for HIV Linking
    cost::CostCategory COST_CATEGORY = cost::CostCategory::HIV;
    /// @brief
    static const person::InfectionType INF_TYPE = person::InfectionType::HIV;
    /// @brief

    bool FalsePositive(std::shared_ptr<person::PersonBase> person) {
        if (person->GetHIV() == person::HIV::NONE) {
            person->ClearDiagnosis(INF_TYPE);
            this->AddLinkingCost(person, LINK_COST::FALSE_POSITIVE,
                                 COST_CATEGORY);
            return true;
        }
        return false;
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        bool is_linked =
            (person->GetLinkState(INF_TYPE) == person::LinkageState::LINKED);
        bool is_not_identified = (!person->IsIdentifiedAsInfected(INF_TYPE));
        if (is_linked || is_not_identified) {
            return;
        }

        if (FalsePositive(person)) {
            return;
        }

        double prob = GetLinkProbability(person, INF_TYPE);
        bool recently_screened = (person->GetTimeSinceLastScreening(INF_TYPE) <=
                                  recent_screen_cutoff);
        if (recently_screened && (prob < 1)) {
            prob = ApplyMultiplier(prob, recent_screen_multiplier);
        }

        if (decider->GetDecision({prob}) == 0) {
            person::LinkageType lt = person->GetLinkageType(INF_TYPE);
            person->Link(lt, INF_TYPE);
            if (lt == person::LinkageType::INTERVENTION) {
                this->AddLinkingCost(person, LINK_COST::INTERVENTION,
                                     COST_CATEGORY);
            }
        }
    }

    HIVLinkingIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm)
        : LinkingIMPL(dm) {
        this->LINK_COLUMNS = {{person::LinkageType::BACKGROUND,
                               "hiv_background_link_probability"},
                              {person::LinkageType::INTERVENTION,
                               "hiv_intervention_link_probability"}};

        this->intervention_cost =
            Utils::GetDoubleFromConfig("hiv_linking.intervention_cost", dm);
        this->false_positive_test_cost = Utils::GetDoubleFromConfig(
            "hiv_linking.false_positive_test_cost", dm);
        this->recent_screen_multiplier = Utils::GetDoubleFromConfig(
            "hiv_linking.recent_screen_multiplier", dm);
        this->recent_screen_cutoff =
            Utils::GetIntFromConfig("hiv_linking.recent_screen_cutoff", dm);

        for (int link_type = 0;
             link_type < static_cast<int>(person::LinkageType::NA);
             ++link_type) {
            person::LinkageType type =
                static_cast<person::LinkageType>(link_type);
            LoadLinkingData(type, dm);
        }
    }
};

HIVLinking::HIVLinking(std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<HIVLinkingIMPL>(dm);
}
HIVLinking::~HIVLinking() = default;
HIVLinking::HIVLinking(HIVLinking &&) noexcept = default;
HIVLinking &HIVLinking::operator=(HIVLinking &&) noexcept = default;
void HIVLinking::DoEvent(std::shared_ptr<person::PersonBase> person,
                         std::shared_ptr<datamanagement::DataManagerBase> dm,
                         std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
