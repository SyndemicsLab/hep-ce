////////////////////////////////////////////////////////////////////////////////
// File: config.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-23                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_TESTS_CONSTANTS_CONFIG_HPP_
#define HEPCE_TESTS_CONSTANTS_CONFIG_HPP_

#include <hepce/utils/logging.hpp>

#include <fstream>
#include <sstream>
#include <string>

namespace hepce {
namespace testing {
inline void BuildSimConf(const std::string &name) {
    std::stringstream s;
    s << "[simulation]" << std::endl
      << "seed =" << std::endl
      << "population_size = 100" << std::endl
      << "events = Aging, BehaviorChanges, Clearance, FibrosisProgression, "
         "HCVInfections, HCVScreening, HCVLinking, HCVTreatment, "
         "HIVInfections, HIVScreening, HIVLinking, Death"
      << std::endl
      << "duration = 60" << std::endl
      << "start_time = 0" << std::endl
      << "[mortality]" << std::endl
      << "f4_infected = 0.000451633598615886" << std::endl
      << "f4_uninfected = 0.0000271037696334409" << std::endl
      << "decomp_infected = 0.01734776" << std::endl
      << "decomp_uninfected = 0.005688756" << std::endl
      << "[infection]" << std::endl
      << std::endl
      << "clearance_prob = 0.0489" << std::endl
      << "genotype_three_prob = 0.153" << std::endl
      << "[eligibility]" << std::endl
      << "ineligible_drug_use =" << std::endl
      << "ineligible_fibrosis_stages =" << std::endl
      << "ineligible_time_former_threshold =" << std::endl
      << "ineligible_time_since_linked =" << std::endl
      << "[fibrosis]" << std::endl
      << "f01 = 0.008877" << std::endl
      << "f12 = 0.00681" << std::endl
      << "f23 = 0.0097026" << std::endl
      << "f34 = 0.0096201" << std::endl
      << "f4d = 0.00558434922840212" << std::endl
      << "add_cost_only_if_identified = false" << std::endl
      << "[fibrosis_staging]" << std::endl
      << "period = 12" << std::endl
      << "test_one = fib4" << std::endl
      << "test_one_cost = 0" << std::endl
      << "test_two = fibroscan" << std::endl
      << "test_two_cost = 140" << std::endl
      << "multitest_result_method = latest" << std::endl
      << "test_two_eligible_stages = f1,f2,f3" << std::endl
      << "[screening]" << std::endl
      << "intervention_type = one-time" << std::endl
      << "period = 12" << std::endl
      << "[screening_background_ab]" << std::endl
      << "cost = 14.27" << std::endl
      << "acute_sensitivity = 0.98" << std::endl
      << "chronic_sensitivity = 0.98" << std::endl
      << "specificity = 0.98" << std::endl
      << "[screening_background_rna]" << std::endl
      << "cost = 31.22" << std::endl
      << "acute_sensitivity = 0.988" << std::endl
      << "chronic_sensitivity = 0.988" << std::endl
      << "specificity = 1.0" << std::endl
      << "[screening_intervention_ab]" << std::endl
      << "cost = 14.27" << std::endl
      << "acute_sensitivity = 0.98" << std::endl
      << "chronic_sensitivity = 0.98" << std::endl
      << "specificity = 0.98" << std::endl
      << "[screening_intervention_rna]" << std::endl
      << "cost = 31.22" << std::endl
      << "acute_sensitivity = 0.988" << std::endl
      << "chronic_sensitivity = 0.988" << std::endl
      << "specificity = 1.0" << std::endl
      << "[linking]" << std::endl
      << "intervention_cost = 0" << std::endl
      << "voluntary_relinkage_probability = 0.001113" << std::endl
      << "voluntary_relink_duration = 3" << std::endl
      << "false_positive_test_cost = 442.39" << std::endl
      << "recent_screen_multiplier = 1.1" << std::endl
      << "recent_screen_cutoff = 0" << std::endl
      << "[treatment]" << std::endl
      << "treatment_limit = 5" << std::endl
      << "treatment_cost = 12603.02" << std::endl
      << "salvage_cost = 19332.50" << std::endl
      << "treatment_utility = 0.99" << std::endl
      << "tox_cost = 201.28" << std::endl
      << "tox_utility = 0.21" << std::endl
      << "[hiv_screening]" << std::endl
      << "intervention_type = null" << std::endl
      << "hiv_screening.period = 12" << std::endl
      << "[hiv_screening_background]" << std::endl
      << "ab_cost = 14.27" << std::endl
      << "ab_sensitivity = 0.98" << std::endl
      << "ab_specificity = 0.98" << std::endl
      << "rna_cost = 31.22" << std::endl
      << "rna_sensitivity = 0.988" << std::endl
      << "rna_specificity = 1.0" << std::endl
      << "[hiv_screening_intervention]" << std::endl
      << "ab_cost = 14.27" << std::endl
      << "ab_sensitivity = 0.98" << std::endl
      << "ab_specificity = 0.98" << std::endl
      << "rna_cost = 31.22" << std::endl
      << "rna_sensitivity = 0.988" << std::endl
      << "rna_specificity = 1.0" << std::endl
      << "[hiv_linking]" << std::endl
      << "intervention_cost = 100.00" << std::endl
      << "false_positive_test_cost = 1800.00" << std::endl
      << "recent_screen_multiplier = 2.0" << std::endl
      << "recent_screen_cutoff = 2" << std::endl
      << "[pregnancy]" << std::endl
      << "multiple_delivery_probability = 0.03283" << std::endl
      << "infant_hcv_tested_probability = 0.4" << std::endl
      << "vertical_hcv_transition_probability = 0.08" << std::endl
      << "[cost]" << std::endl
      << "discounting_rate = 0.0025" << std::endl;

    std::ofstream f(name);
    f << s.str();
    f.close();
}

inline void BuildAlternateSimConf(const std::string &name) {
    std::stringstream s;
    s << "[simulation]" << std::endl
      << "seed =" << std::endl
      << "population_size = 100" << std::endl
      << "events = Aging, BehaviorChanges, Clearance, Pregnancy, "
         "FibrosisProgression, HCVInfections, HCVScreening, HCVLinking, "
         "HCVTreatment, HIVInfection, HIVScreening, HIVLinking, Death"
      << std::endl
      << "duration = 60" << std::endl
      << "start_time = 0" << std::endl
      << "[mortality]" << std::endl
      << "f4_infected = 0.000451633598615886" << std::endl
      << "f4_uninfected = 0.0000271037696334409" << std::endl
      << "decomp_infected = 0.01734776" << std::endl
      << "decomp_uninfected = 0.005688756" << std::endl
      << "hiv = 0.0" << std::endl
      << "[infection]" << std::endl
      << std::endl
      << "clearance_prob = 0.0489" << std::endl
      << "genotype_three_prob = 0.153" << std::endl
      << "[eligibility]" << std::endl
      << "ineligible_drug_use =" << std::endl
      << "ineligible_fibrosis_stages =" << std::endl
      << "ineligible_time_former_threshold =" << std::endl
      << "ineligible_time_since_linked =" << std::endl
      << "[fibrosis]" << std::endl
      << "f01 = 0.008877" << std::endl
      << "f12 = 0.00681" << std::endl
      << "f23 = 0.0097026" << std::endl
      << "f34 = 0.0096201" << std::endl
      << "f4d = 0.00558434922840212" << std::endl
      << "add_cost_only_if_identified = true" << std::endl
      << "[fibrosis_staging]" << std::endl
      << "period = 12" << std::endl
      << "test_one = fib4" << std::endl
      << "test_one_cost = 0" << std::endl
      << "test_two = fibroscan" << std::endl
      << "test_two_cost = 140" << std::endl
      << "multitest_result_method = maximum" << std::endl
      << "test_two_eligible_stages = f1,f2,f3" << std::endl
      << "[screening]" << std::endl
      << "intervention_type = one-time" << std::endl
      << "period = 12" << std::endl
      << "[screening_background_ab]" << std::endl
      << "cost = 14.27" << std::endl
      << "acute_sensitivity = 0.98" << std::endl
      << "chronic_sensitivity = 0.98" << std::endl
      << "specificity = 0.98" << std::endl
      << "[screening_background_rna]" << std::endl
      << "cost = 31.22" << std::endl
      << "acute_sensitivity = 0.988" << std::endl
      << "chronic_sensitivity = 0.988" << std::endl
      << "specificity = 1.0" << std::endl
      << "[screening_intervention_ab]" << std::endl
      << "cost = 14.27" << std::endl
      << "acute_sensitivity = 0.98" << std::endl
      << "chronic_sensitivity = 0.98" << std::endl
      << "specificity = 0.98" << std::endl
      << "[screening_intervention_rna]" << std::endl
      << "cost = 31.22" << std::endl
      << "acute_sensitivity = 0.988" << std::endl
      << "chronic_sensitivity = 0.988" << std::endl
      << "specificity = 1.0" << std::endl
      << "[linking]" << std::endl
      << "intervention_cost = 0" << std::endl
      << "voluntary_relinkage_probability = 0.001113" << std::endl
      << "voluntary_relink_duration = 3" << std::endl
      << "false_positive_test_cost = 442.39" << std::endl
      << "recent_screen_multiplier = 1.0" << std::endl
      << "recent_screen_cutoff = 6" << std::endl
      << "[treatment]" << std::endl
      << "treatment_limit = 5" << std::endl
      << "treatment_cost = 12603.02" << std::endl
      << "salvage_cost = 19332.50" << std::endl
      << "treatment_utility = 0.99" << std::endl
      << "tox_cost = 201.28" << std::endl
      << "tox_utility = 0.21" << std::endl
      << "[hiv_screening]" << std::endl
      << "intervention_type = null" << std::endl
      << "hiv_screening.period = 12" << std::endl
      << "[hiv_screening_background]" << std::endl
      << "ab_cost = 14.27" << std::endl
      << "ab_sensitivity = 0.98" << std::endl
      << "ab_specificity = 0.98" << std::endl
      << "rna_cost = 31.22" << std::endl
      << "rna_sensitivity = 0.988" << std::endl
      << "rna_specificity = 1.0" << std::endl
      << "[hiv_screening_intervention]" << std::endl
      << "ab_cost = 14.27" << std::endl
      << "ab_sensitivity = 0.98" << std::endl
      << "ab_specificity = 0.98" << std::endl
      << "rna_cost = 31.22" << std::endl
      << "rna_sensitivity = 0.988" << std::endl
      << "rna_specificity = 1.0" << std::endl
      << "[hiv_linking]" << std::endl
      << "intervention_cost = 100.00" << std::endl
      << "false_positive_test_cost = 1800.00" << std::endl
      << "recent_screen_multiplier = 2.0" << std::endl
      << "recent_screen_cutoff = 2" << std::endl
      << "[pregnancy]" << std::endl
      << "multiple_delivery_probability = 0.03283" << std::endl
      << "infant_hcv_tested_probability = 0.4" << std::endl
      << "vertical_hcv_transition_probability = 0.08" << std::endl
      << "[cost]" << std::endl
      << "discounting_rate = 0.0025" << std::endl;

    std::ofstream f(name);
    f << s.str();
    f.close();
}

inline void BuildOverdoseSimConf(const std::string &name) {
    std::stringstream s;
    s << "[simulation]" << std::endl
      << "seed =" << std::endl
      << "population_size = 100" << std::endl
      << "events = Aging, BehaviorChanges, Clearance, FibrosisProgression, "
         "HCVInfections, HCVScreening, HCVLinking, HCVTreatment, "
         "HIVInfections, HIVScreening, HIVLinking, Overdose, Death"
      << std::endl
      << "duration = 60" << std::endl
      << "start_time = 0" << std::endl
      << "[mortality]" << std::endl
      << "f4_infected = 0.000451633598615886" << std::endl
      << "f4_uninfected = 0.0000271037696334409" << std::endl
      << "decomp_infected = 0.01734776" << std::endl
      << "decomp_uninfected = 0.005688756" << std::endl
      << "[infection]" << std::endl
      << std::endl
      << "clearance_prob = 0.0489" << std::endl
      << "genotype_three_prob = 0.153" << std::endl
      << "[eligibility]" << std::endl
      << "ineligible_drug_use =" << std::endl
      << "ineligible_fibrosis_stages =" << std::endl
      << "ineligible_time_former_threshold =" << std::endl
      << "ineligible_time_since_linked =" << std::endl
      << "[fibrosis]" << std::endl
      << "f01 = 0.008877" << std::endl
      << "f12 = 0.00681" << std::endl
      << "f23 = 0.0097026" << std::endl
      << "f34 = 0.0096201" << std::endl
      << "f4d = 0.00558434922840212" << std::endl
      << "add_cost_only_if_identified = false" << std::endl
      << "[fibrosis_staging]" << std::endl
      << "period = 12" << std::endl
      << "test_one = fib4" << std::endl
      << "test_one_cost = 0" << std::endl
      << "test_two = fibroscan" << std::endl
      << "test_two_cost = 140" << std::endl
      << "multitest_result_method = latest" << std::endl
      << "test_two_eligible_stages = f1,f2,f3" << std::endl
      << "[screening]" << std::endl
      << "intervention_type = one-time" << std::endl
      << "period = 12" << std::endl
      << "[screening_background_ab]" << std::endl
      << "cost = 14.27" << std::endl
      << "acute_sensitivity = 0.98" << std::endl
      << "chronic_sensitivity = 0.98" << std::endl
      << "specificity = 0.98" << std::endl
      << "[screening_background_rna]" << std::endl
      << "cost = 31.22" << std::endl
      << "acute_sensitivity = 0.988" << std::endl
      << "chronic_sensitivity = 0.988" << std::endl
      << "specificity = 1.0" << std::endl
      << "[screening_intervention_ab]" << std::endl
      << "cost = 14.27" << std::endl
      << "acute_sensitivity = 0.98" << std::endl
      << "chronic_sensitivity = 0.98" << std::endl
      << "specificity = 0.98" << std::endl
      << "[screening_intervention_rna]" << std::endl
      << "cost = 31.22" << std::endl
      << "acute_sensitivity = 0.988" << std::endl
      << "chronic_sensitivity = 0.988" << std::endl
      << "specificity = 1.0" << std::endl
      << "[linking]" << std::endl
      << "intervention_cost = 0" << std::endl
      << "voluntary_relinkage_probability = 0.001113" << std::endl
      << "voluntary_relink_duration = 3" << std::endl
      << "false_positive_test_cost = 442.39" << std::endl
      << "recent_screen_multiplier = 1.1" << std::endl
      << "recent_screen_cutoff = 0" << std::endl
      << "[treatment]" << std::endl
      << "treatment_limit = 5" << std::endl
      << "treatment_cost = 12603.02" << std::endl
      << "salvage_cost = 19332.50" << std::endl
      << "treatment_utility = 0.99" << std::endl
      << "tox_cost = 201.28" << std::endl
      << "tox_utility = 0.21" << std::endl
      << "[hiv_screening]" << std::endl
      << "intervention_type = null" << std::endl
      << "hiv_screening.period = 12" << std::endl
      << "[hiv_screening_background]" << std::endl
      << "ab_cost = 14.27" << std::endl
      << "ab_sensitivity = 0.98" << std::endl
      << "ab_specificity = 0.98" << std::endl
      << "rna_cost = 31.22" << std::endl
      << "rna_sensitivity = 0.988" << std::endl
      << "rna_specificity = 1.0" << std::endl
      << "[hiv_screening_intervention]" << std::endl
      << "ab_cost = 14.27" << std::endl
      << "ab_sensitivity = 0.98" << std::endl
      << "ab_specificity = 0.98" << std::endl
      << "rna_cost = 31.22" << std::endl
      << "rna_sensitivity = 0.988" << std::endl
      << "rna_specificity = 1.0" << std::endl
      << "[hiv_linking]" << std::endl
      << "intervention_cost = 100.00" << std::endl
      << "false_positive_test_cost = 1800.00" << std::endl
      << "recent_screen_multiplier = 2.0" << std::endl
      << "recent_screen_cutoff = 2" << std::endl
      << "[pregnancy]" << std::endl
      << "multiple_delivery_probability = 0.03283" << std::endl
      << "infant_hcv_tested_probability = 0.4" << std::endl
      << "vertical_hcv_transition_probability = 0.08" << std::endl
      << "[cost]" << std::endl
      << "discounting_rate = 0.0025" << std::endl;

    std::ofstream f(name);
    f << s.str();
    f.close();
}

inline void BuildEligibilitySimConf(const std::string &name) {
    std::stringstream s;
    s << "[simulation]" << std::endl
      << "seed =" << std::endl
      << "population_size = 100" << std::endl
      << "events = Aging, BehaviorChanges, Clearance, FibrosisProgression, "
         "HCVInfections, HCVScreening, HCVLinking, HCVTreatment, "
         "HIVInfections, HIVScreening, HIVLinking, Overdose, Death"
      << std::endl
      << "duration = 60" << std::endl
      << "start_time = 0" << std::endl
      << "[mortality]" << std::endl
      << "f4_infected = 0.000451633598615886" << std::endl
      << "f4_uninfected = 0.0000271037696334409" << std::endl
      << "decomp_infected = 0.01734776" << std::endl
      << "decomp_uninfected = 0.005688756" << std::endl
      << "[infection]" << std::endl
      << std::endl
      << "clearance_prob = 0.0489" << std::endl
      << "genotype_three_prob = 0.153" << std::endl
      << "[eligibility]" << std::endl
      << "ineligible_drug_use =" << std::endl
      << "ineligible_fibrosis_stages =" << std::endl
      << "ineligible_time_former_threshold = 6" << std::endl
      << "ineligible_time_since_linked =" << std::endl
      << "[fibrosis]" << std::endl
      << "f01 = 0.008877" << std::endl
      << "f12 = 0.00681" << std::endl
      << "f23 = 0.0097026" << std::endl
      << "f34 = 0.0096201" << std::endl
      << "f4d = 0.00558434922840212" << std::endl
      << "add_cost_only_if_identified = false" << std::endl
      << "[fibrosis_staging]" << std::endl
      << "period = 12" << std::endl
      << "test_one = fib4" << std::endl
      << "test_one_cost = 0" << std::endl
      << "test_two = fibroscan" << std::endl
      << "test_two_cost = 140" << std::endl
      << "multitest_result_method = latest" << std::endl
      << "test_two_eligible_stages = f1,f2,f3" << std::endl
      << "[screening]" << std::endl
      << "intervention_type = one-time" << std::endl
      << "period = 12" << std::endl
      << "[screening_background_ab]" << std::endl
      << "cost = 14.27" << std::endl
      << "acute_sensitivity = 0.98" << std::endl
      << "chronic_sensitivity = 0.98" << std::endl
      << "specificity = 0.98" << std::endl
      << "[screening_background_rna]" << std::endl
      << "cost = 31.22" << std::endl
      << "acute_sensitivity = 0.988" << std::endl
      << "chronic_sensitivity = 0.988" << std::endl
      << "specificity = 1.0" << std::endl
      << "[screening_intervention_ab]" << std::endl
      << "cost = 14.27" << std::endl
      << "acute_sensitivity = 0.98" << std::endl
      << "chronic_sensitivity = 0.98" << std::endl
      << "specificity = 0.98" << std::endl
      << "[screening_intervention_rna]" << std::endl
      << "cost = 31.22" << std::endl
      << "acute_sensitivity = 0.988" << std::endl
      << "chronic_sensitivity = 0.988" << std::endl
      << "specificity = 1.0" << std::endl
      << "[linking]" << std::endl
      << "intervention_cost = 0" << std::endl
      << "voluntary_relinkage_probability = 0.001113" << std::endl
      << "voluntary_relink_duration = 3" << std::endl
      << "false_positive_test_cost = 442.39" << std::endl
      << "recent_screen_multiplier = 1.1" << std::endl
      << "recent_screen_cutoff = 0" << std::endl
      << "[treatment]" << std::endl
      << "treatment_limit = 5" << std::endl
      << "treatment_cost = 12603.02" << std::endl
      << "salvage_cost = 19332.50" << std::endl
      << "treatment_utility = 0.99" << std::endl
      << "tox_cost = 201.28" << std::endl
      << "tox_utility = 0.21" << std::endl
      << "[hiv_screening]" << std::endl
      << "intervention_type = null" << std::endl
      << "hiv_screening.period = 12" << std::endl
      << "[hiv_screening_background]" << std::endl
      << "ab_cost = 14.27" << std::endl
      << "ab_sensitivity = 0.98" << std::endl
      << "ab_specificity = 0.98" << std::endl
      << "rna_cost = 31.22" << std::endl
      << "rna_sensitivity = 0.988" << std::endl
      << "rna_specificity = 1.0" << std::endl
      << "[hiv_screening_intervention]" << std::endl
      << "ab_cost = 14.27" << std::endl
      << "ab_sensitivity = 0.98" << std::endl
      << "ab_specificity = 0.98" << std::endl
      << "rna_cost = 31.22" << std::endl
      << "rna_sensitivity = 0.988" << std::endl
      << "rna_specificity = 1.0" << std::endl
      << "[hiv_linking]" << std::endl
      << "intervention_cost = 100.00" << std::endl
      << "false_positive_test_cost = 1800.00" << std::endl
      << "recent_screen_multiplier = 2.0" << std::endl
      << "recent_screen_cutoff = 2" << std::endl
      << "[pregnancy]" << std::endl
      << "multiple_delivery_probability = 0.03283" << std::endl
      << "infant_hcv_tested_probability = 0.4" << std::endl
      << "vertical_hcv_transition_probability = 0.08" << std::endl
      << "[cost]" << std::endl
      << "discounting_rate = 0.0025" << std::endl;

    std::ofstream f(name);
    f << s.str();
    f.close();
}

inline bool SetSimConfValue(std::string& file, const std::string& log,
  const std::string& s, std::string val) {
  int index = s.find('.');
  std::string sec = s.substr(0, index);
  sec.insert(0, 1, '[');
  sec += ']';
  std::string key = s.substr(index + 1);
  key += " =";

  std::ifstream fin(file);
  if(!fin) {
    hepce::utils::LogError(log, "Failed to open sim.conf for reading!");
    return false;
  }
  std::string tmp;
  std::vector<std::string> lines;
  while (getline(fin, tmp)) {
    lines.push_back(tmp);
    if (tmp == sec) {
        while(getline(fin, tmp)) {
            if (!tmp.empty() && tmp.front() == '[') {
                //key not found
                std::string msg = "Failed to change sim.conf value -- ";
                msg += s;
                msg += " -- not found";
                hepce::utils::LogError(log, msg);
                fin.close();
                return false;
            }
            auto key_len = key.size();
            if (tmp.substr(0, key_len) == key) {
              if (val != "") {
                val.insert(0, 1, ' ');
              }
                tmp.replace(key_len, tmp.size() - key_len, val);
                lines.push_back(tmp);
                break;
            }
            lines.push_back(tmp);
        }
    }
  }
  fin.close();
  std::ofstream fout(file, std::ios::trunc);
  if(!fout) {
    hepce::utils::LogError(log, "Failed to open sim.conf for writing!");
    return false;
  }
  for (auto& l : lines) {
    fout << l << std::endl;
  }
  fout.close();
  return true;
}
} // namespace testing
} // namespace hepce

#endif // HEPCE_TESTS_CONSTANTS_CONFIG_HPP_