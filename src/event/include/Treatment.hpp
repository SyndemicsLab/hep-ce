////////////////////////////////////////////////////////////////////////////////
// File: Treatment.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-16                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-17                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENT_TREATMENT_HPP_
#define EVENT_TREATMENT_HPP_

#include "Cost.hpp"
#include "Event.hpp"
#include "Person.hpp"
#include "Utility.hpp"
#include <unordered_map>

namespace event {
class TreatmentIMPL {
protected:
    utility::UtilityCategory UTIL_CATEGORY =
        utility::UtilityCategory::TREATMENT;
    double discount = 0.0;
    double lost_to_follow_up_probability;
    double treatment_cost;
    double retreatment_cost;
    double treatment_utility;
    double toxicity_cost;
    double toxicity_utility;
    double treatment_init_probability;
    bool allow_retreatment = true;
    // including all ineligibility variables because it's unclear if any would
    // make a person ineligible for HIV treatment - erring on the side of
    // caution
    std::vector<std::string> ineligible_behaviors = {};
    std::vector<std::string> ineligible_fibrosis = {};
    std::vector<std::string> ineligible_pregnancy = {};
    int ineligible_time_since_linked = -2;
    int ineligible_time_since_last_use = -2;

    using treatmentmap_t =
        std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                           Utils::key_equal_3i>;
    treatmentmap_t duration_data;
    treatmentmap_t cost_data;
    treatmentmap_t svr_data;
    treatmentmap_t toxicity_data;
    treatmentmap_t withdrawal_data;

public:
    TreatmentIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm);
};
} // namespace event
#endif
