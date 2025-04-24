////////////////////////////////////////////////////////////////////////////////
// File: PersonTableOperations.hpp                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-21                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-24                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef PERSON_PERSONTABLEOPERATIONS_HPP_
#define PERSON_PERSONTABLEOPERATIONS_HPP_

#include "Containers.hpp"
#include "Utils.hpp"
#include <string>

namespace person {
struct person_select {
    // basic characteristics
    Sex sex = Sex::MALE;
    int age = 0;
    bool is_alive = true;
    bool boomer_classification = false;
    DeathReason death_reason = DeathReason::NA;
    // BehaviorDetails
    Behavior drug_behavior = Behavior::NEVER;
    int time_last_active_drug_use = -1;
    // HCVDetails
    HCV hcv = HCV::NONE;
    FibrosisState fibrosis_state = FibrosisState::NONE;
    bool is_genotype_three = false;
    bool seropositive = false;
    int time_hcv_changed = -1;
    int time_fibrosis_state_changed = -1;
    int times_hcv_infected = 0;
    int times_acute_cleared = 0;
    int svrs = 0;
    // HIVDetails
    HIV hiv = HIV::NONE;
    int time_hiv_changed = -1;
    int low_cd4_months_count = 0;
    // HCCDetails
    HCCState hcc_state = HCCState::NONE;
    bool hcc_diagnosed = false;
    // Overdoses
    int num_overdoses = 0;
    bool currently_overdosing = false;
    // MOUDDetails
    MOUD moud_state = MOUD::NONE;
    int time_started_moud = -1;
    int current_moud_concurrent_months = 0;
    int total_moud_months = 0;
    // PregnancyDetails - NOTE: No instantiation of the children born prior to
    // this simulation run in the `children' vector.
    PregnancyState pregnancy_state = PregnancyState::NA;
    int time_of_pregnancy_change = -1;
    int pregnancy_count = 0;
    int num_infants = 0;
    int num_miscarriages = 0;
    int num_infant_hcv_exposures = 0;
    int num_infant_hcv_infections = 0;
    int num_infant_hcv_tests = 0;
    // StagingDetails
    MeasuredFibrosisState measured_fibrosis_state = MeasuredFibrosisState::NONE;
    bool had_second_test = false;
    int time_of_last_staging = -1;
    // LinkageDetails
    // HCV
    LinkageState hcv_link_state = LinkageState::NEVER;
    int time_of_hcv_link_change = -1;
    LinkageType hcv_link_type = LinkageType::NA;
    int hcv_link_count = 0;
    // HIV
    LinkageState hiv_link_state = LinkageState::NEVER;
    int time_of_hiv_link_change = -1;
    LinkageType hiv_link_type = LinkageType::NA;
    int hiv_link_count = 0;
    // ScreeningDetails
    // HCV
    int time_of_last_hcv_screening = -1;
    int num_hcv_ab_tests = 0;
    int num_hcv_rna_tests = 0;
    bool hcv_antibody_positive = false;
    int hcv_identified = false;
    int time_hcv_identified = -1;
    // HIV
    int time_of_last_hiv_screening = -1;
    int num_hiv_ab_tests = 0;
    int num_hiv_rna_tests = 0;
    bool hiv_antibody_positive = false;
    int hiv_identified = false;
    int time_hiv_identified = -1;
    // TreatmentDetails
    // HCV
    bool initiated_hcv_treatment = false;
    int time_of_hcv_treatment_initiation = -1;
    int num_hcv_treatment_starts = 0;
    int num_hcv_treatment_withdrawals = 0;
    int num_hcv_treatment_toxic_reactions = 0;
    int num_completed_hcv_treatments = 0;
    int num_hcv_retreatments = 0;
    bool in_hcv_retreatment = false;
    // HIV - NOTE: No retreatment analogue for HIV, so values stay default.
    // Also, there is no "completing" HIV treatment since it cannot be cured.
    bool initiated_hiv_treatment = false;
    int time_of_hiv_treatment_initiation = -1;
    int num_hiv_treatment_starts = 0;
    int num_hiv_treatment_withdrawals = 0;
    int num_hiv_treatment_toxic_reactions = 0;
    // NOTE: No CostTracker. Costs are only those accrued during the time
    // horizon of the simulation
    // UtilityTracker
    double behavior_utility = 1.0;
    double liver_utility = 1.0;
    double treatment_utility = 1.0;
    double background_utility = 1.0;
    double hiv_utility = 1.0;
    // NOTE: Like cost, LifetimeUtility, life_span, and discounted_life_span are
    // used to track only the accumulation over the course of the simulated time
    // horizon, so they are not used when creating a new person.
};

static inline int person_callback(void *storage, int count, char **data,
                                  char **columns) {
    struct person_select *temp = (struct person_select *)storage;
    temp->sex << data[0];
    temp->age = std::stoi(data[1]);
    temp->is_alive = std::stoi(data[2]);
    temp->boomer_classification = std::stoi(data[3]);
    temp->death_reason << data[4];
    temp->drug_behavior << data[5];
    temp->time_last_active_drug_use = std::stoi(data[6]);
    temp->hcv << data[7];
    temp->fibrosis_state << data[8];
    temp->is_genotype_three = std::stoi(data[9]);
    temp->seropositive = std::stoi(data[10]);
    temp->time_hcv_changed = std::stoi(data[11]);
    temp->time_fibrosis_state_changed = std::stoi(data[12]);
    temp->times_hcv_infected = std::stoi(data[13]);
    temp->times_acute_cleared = std::stoi(data[14]);
    temp->svrs = std::stoi(data[15]);
    temp->hiv << data[16];
    temp->time_hiv_changed = std::stoi(data[17]);
    temp->low_cd4_months_count = std::stoi(data[18]);
    temp->hcc_state << data[19];
    temp->hcc_diagnosed = std::stoi(data[20]);
    temp->currently_overdosing = std::stoi(data[21]);
    temp->num_overdoses = std::stoi(data[22]);
    temp->moud_state << data[23];
    temp->time_started_moud = std::stoi(data[24]);
    temp->current_moud_concurrent_months = std::stoi(data[25]);
    temp->total_moud_months = std::stoi(data[26]);
    temp->pregnancy_state << data[27];
    temp->time_of_pregnancy_change = std::stoi(data[28]);
    temp->pregnancy_count = std::stoi(data[29]);
    temp->num_infants = std::stoi(data[30]);
    temp->num_miscarriages = std::stoi(data[31]);
    temp->num_infant_hcv_exposures = std::stoi(data[32]);
    temp->num_infant_hcv_infections = std::stoi(data[33]);
    temp->num_infant_hcv_tests = std::stoi(data[34]);
    temp->measured_fibrosis_state << data[35];
    temp->had_second_test = std::stoi(data[36]);
    temp->time_of_last_staging = std::stoi(data[37]);
    temp->hcv_link_state << data[38];
    temp->time_of_hcv_link_change = std::stoi(data[39]);
    temp->hcv_link_type << data[40];
    temp->hcv_link_count = std::stoi(data[41]);
    temp->hiv_link_state << data[42];
    temp->time_of_hiv_link_change = std::stoi(data[43]);
    temp->hiv_link_type << data[44];
    temp->hiv_link_count = std::stoi(data[45]);
    temp->time_of_last_hcv_screening = std::stoi(data[46]);
    temp->num_hcv_ab_tests = std::stoi(data[47]);
    temp->num_hcv_rna_tests = std::stoi(data[48]);
    temp->hcv_antibody_positive = std::stoi(data[49]);
    temp->hcv_identified = std::stoi(data[50]);
    temp->time_hcv_identified = std::stoi(data[51]);
    temp->time_of_last_hiv_screening = std::stoi(data[52]);
    temp->num_hiv_ab_tests = std::stoi(data[53]);
    temp->num_hiv_rna_tests = std::stoi(data[54]);
    temp->hiv_antibody_positive = std::stoi(data[55]);
    temp->hiv_identified = std::stoi(data[56]);
    temp->time_hiv_identified = std::stoi(data[57]);
    temp->initiated_hcv_treatment = std::stoi(data[58]);
    temp->time_of_hcv_treatment_initiation = std::stoi(data[59]);
    temp->num_hcv_treatment_starts = std::stoi(data[60]);
    temp->num_hcv_treatment_withdrawals = std::stoi(data[61]);
    temp->num_hcv_treatment_toxic_reactions = std::stoi(data[62]);
    temp->num_completed_hcv_treatments = std::stoi(data[63]);
    temp->num_hcv_retreatments = std::stoi(data[64]);
    temp->in_hcv_retreatment = std::stoi(data[65]);
    temp->initiated_hiv_treatment = std::stoi(data[66]);
    temp->time_of_hiv_treatment_initiation = std::stoi(data[67]);
    temp->num_hiv_treatment_starts = std::stoi(data[68]);
    temp->num_hiv_treatment_withdrawals = std::stoi(data[69]);
    temp->num_hiv_treatment_toxic_reactions = std::stoi(data[70]);
    temp->behavior_utility = Utils::stod_positive(data[71]);
    temp->liver_utility = Utils::stod_positive(data[72]);
    temp->treatment_utility = Utils::stod_positive(data[73]);
    temp->background_utility = Utils::stod_positive(data[74]);
    temp->hiv_utility = Utils::stod_positive(data[75]);
    return 0;
}
} // namespace person
#endif
