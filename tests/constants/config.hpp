////////////////////////////////////////////////////////////////////////////////
// File: config.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_TESTS_CONSTANTS_CONFIG_HPP_
#define HEPCE_TESTS_CONSTANTS_CONFIG_HPP_

#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace hepce {
namespace testing {
static std::unordered_map<std::string, std::vector<std::string>>
    DEFAULT_CONFIG = {
        // clang-format off
    {"simulation", {
            "seed =",
            "population_size = 100",
            "events = Aging, BehaviorChanges, Clearance, FibrosisProgression, "
            "HCVInfection, HCVScreening, HCVLinking, HCVTreatment,"
            "HIVInfection, HIVScreening, HIVLinking, Death",
            "duration = 60",
            "start_time = 0"
        }},
    {"mortality", {
            "f4_infected = 0.000451633598615886",
            "f4_uninfected = 0.0000271037696334409",
            "decomp_infected = 0.01734776",
            "decomp_uninfected = 0.005688756",
            "hiv = 0.0"
        }},
    {"behavior", {
            "first_year_relapse_rate = 0.3",
            "later_years_relapse_rate = 0.1"
        }},
    {"overdose", {
            "probability_of_overdose_fatality = 0.1",
            "fatal_overdose_cost = 100.00"
        }},
    {"infection", {
            "clearance_prob = 0.0489",
            "genotype_three_prob = 0.153"
        }},
    {"eligibility", {
            "ineligible_drug_use =",
            "ineligible_fibrosis_stages =",
            "ineligible_time_former_threshold =",
            "ineligible_time_since_linked ="
        }},
    {"fibrosis", {
            "f01 = 0.008877",
            "f12 = 0.00681",
            "f23 = 0.0097026",
            "f34 = 0.0096201",
            "f4d = 0.00558434922840212",
            "add_cost_only_if_identified = false"
        }},
    {"fibrosis_staging", {
            "period = 12",
            "test_one = fib4",
            "test_one_cost = 0",
            "test_two = fibroscan",
            "test_two_cost = 140",
            "multitest_result_method = latest",
            "test_two_eligible_stages = f1,f2,f3"
        }},
    {"screening", {
            "intervention_type = one-time",
            "period = 12"
        }},
    {"screening_background_ab", {
            "cost = 14.27",
            "acute_sensitivity = 0.98",
            "chronic_sensitivity = 0.98",
            "specificity = 0.98"
        }},
    {"screening_background_rna", {
            "cost = 31.22",
            "acute_sensitivity = 0.988",
            "chronic_sensitivity = 0.988",
            "specificity = 1.0"
        }},
    {"screening_intervention_ab", {
            "cost = 14.27",
            "acute_sensitivity = 0.98",
            "chronic_sensitivity = 0.98",
            "specificity = 0.98"
        }},
    {"screening_intervention_rna", {
            "cost = 31.22",
            "acute_sensitivity = 0.988",
            "chronic_sensitivity = 0.988",
            "specificity = 1.0"
        }},
    {"linking", {
            "intervention_cost = 0",
            "voluntary_relinkage_probability = 0.001113",
            "voluntary_relink_duration = 3",
            "false_positive_test_cost = 442.39",
            "scaling_type = multiplier",
            "scaling_coefficient = 1.1",
            "recent_screen_cutoff = 0"
        }},
    {"treatment", {
            "treatment_limit = 5",
            "treatment_cost = 12603.02",
            "salvage_cost = 19332.50",
            "treatment_utility = 0.99",
            "tox_cost = 201.28",
            "tox_utility = 0.21"
        }},
    {"hiv_screening", {
            "intervention_type = null",
            "hiv_screening.period = 12"
        }},
    {"hiv_screening_background", {
            "ab_cost = 14.27",
            "ab_sensitivity = 0.98",
            "ab_specificity = 0.98",
            "rna_cost = 31.22",
            "rna_sensitivity = 0.988",
            "rna_specificity = 1.0"
        }},
    {"hiv_screening_intervention", {
            "ab_cost = 14.27",
            "ab_sensitivity = 0.98",
            "ab_specificity = 0.98",
            "rna_cost = 31.22",
            "rna_sensitivity = 0.988",
            "rna_specificity = 1.0"
        }},
    {"hiv_linking", {
            "intervention_cost = 100.00",
            "false_positive_test_cost = 1800.00",
            "scaling_coefficient = 2.0",
            "recent_screen_cutoff = 2"
        }},
    {"pregnancy", {
            "multiple_delivery_probability = 0.03283",
            "infant_hcv_tested_probability = 0.4",
            "vertical_hcv_transition_probability = 0.08"
        }},
    {"cost", {
      "discounting_rate = 0.0025"
        }}
        // clang-format on
};

static const std::string OVERDOSE_EVENTS =
    "events = Aging, BehaviorChanges, Clearance, FibrosisProgression, "
    "HCVInfection, HCVScreening, HCVLinking, HCVTreatment,"
    "HIVInfection, HIVScreening, HIVLinking, Overdose, Death";

static const std::string PREGNANCY_EVENTS =
    "events = Aging, BehaviorChanges, Clearance, Pregnancy, "
    "FibrosisProgression,HCVInfection, HCVScreening, HCVLinking, HCVTreatment,"
    "HIVInfection, HIVScreening, HIVLinking, Death";

inline void
BuildSimConf(const std::string &name,
             std::unordered_map<std::string, std::vector<std::string>> config =
                 DEFAULT_CONFIG) {
    std::stringstream ss;
    for (auto &[section, keys] : config) {
        ss << '[' << section << ']' << std::endl;
        for (auto &k : keys) {
            ss << k << std::endl;
        }
    }
    std::ofstream f(name);
    f << ss.str();
    f.close();
}
} // namespace testing
} // namespace hepce

#endif // HEPCE_TESTS_CONSTANTS_CONFIG_HPP_
