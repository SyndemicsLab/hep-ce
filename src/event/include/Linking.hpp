////////////////////////////////////////////////////////////////////////////////
// File: Linking.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-09                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-09                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Event.hpp"
#include "Person.hpp"
#include "Utils.hpp"
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

    using linkmap_t =
        std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                           Utils::key_equal_4i>;

    linkmap_t background_link_data;
    linkmap_t intervention_link_data;

    static int callback_link(void *storage, int count, char **data,
                             char **columns);
    double
    GetLinkProbability(std::shared_ptr<person::PersonBase> person,
                       person::InfectionType it = person::InfectionType::HCV);
    void AddLinkingCost(std::shared_ptr<person::PersonBase> person,
                        LINK_COST type);
    static double ApplyMultiplier(double prob, double mult);

    static bool
    CheckForPregnancyEvent(std::shared_ptr<datamanagement::DataManagerBase> dm);

public:
    LinkingIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm);
};
} // namespace event
