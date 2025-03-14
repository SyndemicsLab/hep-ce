////////////////////////////////////////////////////////////////////////////////
// File: HIVScreening.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-06                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-14                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVScreening.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <string>
#include <unordered_map>

namespace event {
class HIVScreening::HIVScreeningIMPL {
private:
    enum class SCREEN_TYPE { BACKGROUND = 0, INTERVENTION = 1 };
    person::InfectionType it = person::InfectionType::HIV;

    double discount = 0.0;

    // background HIV screening
    double background_rna_acute_sensitivity;
    double background_rna_chronic_sensitivity;
    double background_rna_specificity;
    double background_rna_cost;

    double background_ab_acute_sensitivity;
    double background_ab_chronic_sensitivity;
    double background_ab_specificity;
    double background_ab_cost;

    // intervention HIV screening
    int screening_period;
    std::string intervention_type;

    double intervention_rna_acute_sensitivity;
    double intervention_rna_chronic_sensitivity;
    double intervention_rna_specificity;
    double intervention_rna_cost;

    double intervention_ab_acute_sensitivity;
    double intervention_ab_chronic_sensitivity;
    double intervention_ab_specificity;
    double intervention_ab_cost;

    using hivscreenmap_t =
        std::unordered_map<Utils::tuple3i, double, Utils::key_hash_3i,
                           Utils::key_equal_3i>;
    hivscreenmap_t hiv_screen_data;

    static int callback_hivscreen(void *storage, int count, char **data,
                                  char **columns) {
        Utils::tuple_3i tup = std::make_tuple(
            std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
        (*((hivscreenmap_t *)storage))[tup] = Utils::stod_positive(data[3]);
        return 0;
    }

    std::string HIVScreenSQL(std::string column) const {
        return "SELECT at.age_years, sl.gender, sl.drug_behavior, " + column +
               " FROM antibody_testing AS at INNER JOIN "
               "screening_and_linkage AS sl ON ((at.age_years = "
               "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";
    }

    double
    GetDoubleFromConfig(std::string config_key,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        bool positive = true) {
        std::string config_data;
        dm->GetFromConfig(config_key, config_data);
        if (config_data.empty()) {
            spdlog::get("main")->warn("No {} Found!", config_key);
            config_data = "0.0";
        }
        if (positive) {
            return Utils::stod_positive(config_data);
        }
        return static_cast<double>(config_data);
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // if Person already linked, skip screening
        if (person->GetLinkState(it) == person::LinkageState::LINKED) {
            return;
        }
        // if not already tested positive for antibody, hiv antibody screen
        // if positive for HIV antibody, hiv rna screen
        // if rna positive, mark hiv identified
    }
    HIVScreeningIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        discount = GetDoubleFromConfig("cost.discounting_rate", dm);
    }
};
} // namespace event
