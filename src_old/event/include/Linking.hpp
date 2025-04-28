////////////////////////////////////////////////////////////////////////////////
// File: Linking.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-09                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_LINKING_HPP_
#define EVENT_LINKING_HPP_

#include "Cost.hpp"
#include "Event.hpp"
#include "Person.hpp"
#include <unordered_map>

namespace event {
class LinkingIMPL {
protected:
    // constants
    // two types of linking - background and intervention
    /// @brief
    enum class LINK_COST : int { INTERVENTION = 0, FALSE_POSITIVE = 1 };

    // user-provided values
    double discount = 0.0;
    double intervention_cost = 0.0;
    double false_positive_test_cost = 0.0;
    int recent_screen_cutoff = -1;
    double recent_screen_multiplier = 1.0;
    bool pregnancy_strata = false;
    // defaults to "regular" hcv linking cost category, overwritten in events
    // for other infections
    cost::CostCategory COST_CATEGORY = cost::CostCategory::LINKING;

    // event-specific values
    std::unordered_map<person::LinkageType, std::string> LINK_COLUMNS;
    person::InfectionType INF_TYPE;

    using linkmap_t =
        std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                           Utils::key_equal_4i>;

    linkmap_t background_link_data;
    linkmap_t intervention_link_data;

    static int callback_link(void *storage, int count, char **data,
                             char **columns);
    std::string LinkSQL(std::string column) const;
    double
    GetLinkProbability(std::shared_ptr<person::PersonBase> person,
                       person::InfectionType it = person::InfectionType::HCV);
    void
    AddLinkingCost(std::shared_ptr<person::PersonBase> person, LINK_COST type,
                   cost::CostCategory category = cost::CostCategory::LINKING);
    double ApplyMultiplier(double prob, double mult);
    bool FalsePositive(std::shared_ptr<person::PersonBase> person);
    bool CheckForPregnancyEvent(datamanagement::ModelData &model_data);
    void LoadLinkingData(person::LinkageType type,
                         datamanagement::ModelData &model_data);

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider);
    LinkingIMPL(datamanagement::ModelData &model_data);
};
} // namespace event
#endif
