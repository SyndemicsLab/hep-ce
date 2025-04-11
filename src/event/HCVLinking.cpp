////////////////////////////////////////////////////////////////////////////////
// File: HCVLinking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-11                                                  //
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
class HCVLinkingIMPL : LinkingIMPL {
private:
    // constants
    const std::unordered_map<person::LinkageType, std::string> LINK_COLUMNS = {
        {person::LinkageType::BACKGROUND, "background_link_probability"},
        {person::LinkageType::INTERVENTION, "intervention_link_probability"}};

    std::string LinkSQL(std::string const column) const {
        if (this->pregnancy_strata) {
            return "SELECT age_years, gender, drug_behavior, pregnancy, " +
                   column + " FROM screening_and_linkage;";
        }
        return "SELECT age_years, gender, drug_behavior, -1, " + column +
               " FROM screening_and_linkage;";
    }

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

    linkmap_t *PickMap(person::LinkageType type) {
        switch (type) {
        case person::LinkageType::BACKGROUND:
            return &this->background_link_data;
        case person::LinkageType::INTERVENTION:
            return &this->intervention_link_data;
        default:
            break;
        }
        return nullptr;
    }

    void LoadLinkingData(person::LinkageType type,
                         std::shared_ptr<datamanagement::DataManagerBase> dm) {
        linkmap_t *chosen_linkmap = PickMap(type);
        std::string column = LINK_COLUMNS.at(type);
        std::string error;
        int rc = dm->SelectCustomCallback(LinkSQL(column), this->callback_link,
                                          chosen_linkmap, error);
        if (rc != 0) {
            spdlog::get("main")->error("Error retrieving HCV Linking values "
                                       "for column `{};! Error Message: {}",
                                       column, error);
        }
        if ((*chosen_linkmap).empty()) {
            spdlog::get("main")->warn("No `" + column + "' found.");
        }
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
        this->intervention_cost =
            Event::GetDoubleFromConfig("linking.intervention_cost", dm);
        this->false_positive_test_cost =
            Event::GetDoubleFromConfig("linking.false_positive_test_cost", dm);
        this->recent_screen_multiplier =
            Event::GetDoubleFromConfig("linking.recent_screen_multiplier", dm);
        this->recent_screen_cutoff =
            Event::GetIntFromConfig("linking.recent_screen_cutoff", dm);

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
