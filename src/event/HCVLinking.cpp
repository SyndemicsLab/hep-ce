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
private:
    bool FalsePositive(std::shared_ptr<person::PersonBase> person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm) {
        if (person->GetHCV() == person::HCV::NONE) {
            person->ClearDiagnosis();
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
            (person->GetLinkState() == person::LinkageState::LINKED);
        bool is_not_identified = (!person->IsIdentifiedAsInfected());
        if (is_linked || is_not_identified) {
            return;
        }

        if (FalsePositive(person, dm)) {
            return;
        }

        double prob = GetLinkProbability(person);

        // check if the person was recently screened, for multiplier
        bool recently_screened =
            (person->GetTimeSinceLastScreening() <= recent_screen_cutoff);
        // apply the multiplier to recently screened persons
        if (recently_screened && (prob < 1)) {
            prob = ApplyMultiplier(prob, recent_screen_multiplier);
        }

        // draw from link probability
        if (decider->GetDecision({prob}) == 0) {
            person::LinkageType lt = person->GetLinkageType();
            person->Link(lt);
            if (lt == person::LinkageType::INTERVENTION) {
                this->AddLinkingCost(person, LINK_COST::INTERVENTION,
                                     COST_CATEGORY);
            }
        }
    }
    HCVLinkingIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm)
        : LinkingIMPL(dm) {
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
             link_type < static_cast<int>(person::LinkageType::NA);
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
