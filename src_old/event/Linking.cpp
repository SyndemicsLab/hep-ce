////////////////////////////////////////////////////////////////////////////////
// File: Linking.cpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-02                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Linking.hpp"
#include "Decider.hpp"
#include "spdlog/spdlog.h"
#include <datamanagement/datamanagement.hpp>
#include <string>

namespace event {
int LinkingIMPL::callback_link(void *storage, int count, char **data,
                               char **columns) {
    Utils::tuple_4i tup =
        std::make_tuple(std::stoi(data[0]), std::stoi(data[1]),
                        std::stoi(data[2]), std::stoi(data[3]));
    (*((linkmap_t *)storage))[tup] = Utils::stod_positive(data[4]);
    return 0;
}

/// @brief
/// @param
/// @return
std::string LinkingIMPL::LinkSQL(std::string column) const {
    if (this->pregnancy_strata) {
        return "SELECT age_years, gender, drug_behavior, pregnancy, " + column +
               " FROM screening_and_linkage;";
    }
    return "SELECT age_years, gender, drug_behavior, -1, " + column +
           " FROM screening_and_linkage;";
}

double
LinkingIMPL::GetLinkProbability(std::shared_ptr<person::PersonBase> person,
                                person::InfectionType it) {
    int age_years = (int)(person->GetAge() / 12.0);
    int gender = (int)person->GetSex();
    int drug_behavior = (int)person->GetBehavior();
    int pregnancy = (pregnancy_strata) ? (int)person->GetPregnancyState() : -1;
    const person::LinkageType &type = person->GetLinkageType(it);
    Utils::tuple_4i tup =
        std::make_tuple(age_years, gender, drug_behavior, pregnancy);
    if (type == person::LinkageType::BACKGROUND) {
        return this->background_link_data[tup];
    } else if (type == person::LinkageType::INTERVENTION) {
        return this->intervention_link_data[tup];
    }
    return 0.0;
}

void LinkingIMPL::AddLinkingCost(std::shared_ptr<person::PersonBase> person,
                                 LINK_COST type, cost::CostCategory category) {
    double cost;
    if (type == LINK_COST::INTERVENTION) {
        cost = this->intervention_cost;
    } else if (type == LINK_COST::FALSE_POSITIVE) {
        cost = this->false_positive_test_cost;
    } else {
        return;
    }

    double discountAdjustedCost =
        Event::DiscountEventCost(cost, discount, person->GetCurrentTimestep());
    person->AddCost(cost, discountAdjustedCost, category);
}

double LinkingIMPL::ApplyMultiplier(double prob, double mult) {
    return Utils::rateToProbability(Utils::probabilityToRate(prob) * mult);
}

bool LinkingIMPL::CheckForPregnancyEvent(
    datamanagement::ModelData &model_data) {
    std::vector<std::string> event_list = Utils::split2vecT<std::string>(
        Utils::GetStringFromConfig("simulation.events", dm), ',');

    return (std::find(event_list.begin(), event_list.end(), "pregnancy") !=
            event_list.end());
}

void LinkingIMPL::LoadLinkingData(person::LinkageType type,
                                  datamanagement::ModelData &model_data) {
    std::string infection =
        (this->COST_CATEGORY == cost::CostCategory::LINKING) ? "HCV" : "HIV";
    linkmap_t *chosen_linkmap = (type == person::LinkageType::BACKGROUND)
                                    ? &this->background_link_data
                                    : &this->intervention_link_data;
    std::string column = this->LINK_COLUMNS.at(type);
    std::string error;
    int rc = dm->SelectCustomCallback(LinkSQL(column), this->callback_link,
                                      chosen_linkmap, error);
    if (rc != 0) {
        spdlog::get("main")->error("Error retrieving {} Linking values "
                                   "for column `{};! Error Message: {}",
                                   infection, column, error);
    }
    if ((*chosen_linkmap).empty()) {
        spdlog::get("main")->warn("No `" + column + "' found.");
    }
}

bool LinkingIMPL::FalsePositive(std::shared_ptr<person::PersonBase> person) {
    if (this->INF_TYPE == person::InfectionType::HCV) {
        if (person->GetHCV() == person::HCV::NONE) {
            person->ClearDiagnosis();
            this->AddLinkingCost(person, LINK_COST::FALSE_POSITIVE,
                                 this->COST_CATEGORY);
            return true;
        }
    } else if (this->INF_TYPE == person::InfectionType::HIV) {
        if (person->GetHIV() == person::HIV::NONE) {
            person->ClearDiagnosis(INF_TYPE);
            this->AddLinkingCost(person, LINK_COST::FALSE_POSITIVE,
                                 this->COST_CATEGORY);
            return true;
        }
    }
    return false;
}

void LinkingIMPL::DoEvent(std::shared_ptr<person::PersonBase> person,
                          datamanagement::ModelData &model_data,
                          std::shared_ptr<stats::DeciderBase> decider) {
    bool is_linked =
        (person->GetLinkState(this->INF_TYPE) == person::LinkageState::LINKED);
    bool is_not_identified = (!person->IsIdentifiedAsInfected(this->INF_TYPE));
    if (is_linked || is_not_identified) {
        return;
    }

    if (FalsePositive(person)) {
        return;
    }

    double prob = GetLinkProbability(person, this->INF_TYPE);
    // check if the person was recently screened, for multiplier
    bool recently_screened = (person->GetTimeSinceLastScreening(
                                  this->INF_TYPE) <= recent_screen_cutoff);
    // apply the multiplier to recently screened persons
    if (recently_screened && (prob < 1)) {
        prob = ApplyMultiplier(prob, recent_screen_multiplier);
    }

    // draw from link probability
    if (decider->GetDecision({prob}) == 0) {
        person::LinkageType lt = person->GetLinkageType(this->INF_TYPE);
        person->Link(lt, this->INF_TYPE);
        if (lt == person::LinkageType::INTERVENTION) {
            this->AddLinkingCost(person, LINK_COST::INTERVENTION,
                                 this->COST_CATEGORY);
        }
    }
}

LinkingIMPL::LinkingIMPL(datamanagement::ModelData &model_data) {
    this->discount = Utils::GetDoubleFromConfig("cost.discounting_rate", dm);
    this->pregnancy_strata = CheckForPregnancyEvent(dm);
}
} // namespace event
