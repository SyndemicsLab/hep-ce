////////////////////////////////////////////////////////////////////////////////
// File: person.cpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-21                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/model/person.hpp>

// Local Includes
#include "internals/person_internals.hpp"

namespace hepce {
namespace model {
// Factory
std::unique_ptr<Person> Person::Create(const std::string &log_name) {
    return std::make_unique<PersonImpl>(log_name);
}

// Constructor
PersonImpl::PersonImpl(const std::string &log_name) : _log_name(log_name) {
    _costs = Costs::Create(log_name);
    SetInfectionDefaults(data::InfectionType::kHcv);
    SetInfectionDefaults(data::InfectionType::kHiv);

    SetUtility(1, UtilityCategory::kBehavior);
    SetUtility(1, UtilityCategory::kLiver);
    SetUtility(1, UtilityCategory::kTreatment);
    SetUtility(1, UtilityCategory::kBackground);
    SetUtility(1, UtilityCategory::kHiv);
    SetUtility(1, UtilityCategory::kMoud);
    SetUtility(1, UtilityCategory::kOverdose);
}

void PersonImpl::SetPersonDetails(const data::PersonSelect &storage) {
    // basic characteristics
    _sex = storage.sex;
    _age = storage.age;
    _is_alive = storage.is_alive;
    _boomer_classification = storage.boomer_classification;
    SetDeathReason(storage.death_reason);

    // BehaviorDetails
    _behavior_details.time_last_active = storage.time_last_active_drug_use;
    SetBehavior(storage.drug_behavior);

    // HCVDetails
    _hcv_details.hcv = storage.hcv;
    _hcv_details.fibrosis_state = storage.fibrosis_state;
    _hcv_details.is_genotype_three = storage.is_genotype_three;
    _hcv_details.seropositive = storage.seropositive;
    _hcv_details.time_changed = storage.time_hcv_changed;
    _hcv_details.time_fibrosis_state_changed =
        storage.time_fibrosis_state_changed;
    _hcv_details.times_infected = storage.times_hcv_infected;
    _hcv_details.times_acute_cleared = storage.times_acute_cleared;
    _hcv_details.svrs = storage.svrs;

    // HIVDetails
    _hiv_details.hiv = storage.hiv;
    _hiv_details.time_changed = storage.time_hiv_changed;
    _hiv_details.low_cd4_months_count = storage.low_cd4_months_count;

    // HCCDetails
    _hcc_details.hcc_state = storage.hcc_state;
    _hcc_details.hcc_diagnosed = storage.hcc_diagnosed;

    // Overdoses
    _num_overdoses = storage.num_overdoses;
    _currently_overdosing = storage.currently_overdosing;

    // MOUDDetails
    _moud_details.moud_state = storage.moud_state;
    _moud_details.time_started_moud = storage.time_started_moud;
    _moud_details.current_state_concurrent_months =
        storage.current_moud_concurrent_months;
    _moud_details.total_moud_months = storage.total_moud_months;

    // PregnancyDetails
    _pregnancy_details.pregnancy_state = storage.pregnancy_state;
    _pregnancy_details.time_of_pregnancy_change =
        storage.time_of_pregnancy_change;
    _pregnancy_details.count = storage.pregnancy_count;
    _pregnancy_details.num_infants = storage.num_infants;
    _pregnancy_details.num_stillbirths = storage.num_stillbirths;
    _pregnancy_details.num_hcv_exposures = storage.num_infant_hcv_exposures;
    _pregnancy_details.num_hcv_infections = storage.num_infant_hcv_infections;
    _pregnancy_details.num_hcv_tests = storage.num_infant_hcv_tests;

    // StagingDetails
    _staging_details.measured_fibrosis_state = storage.measured_fibrosis_state;
    _staging_details.had_second_test = storage.had_second_test;
    _staging_details.time_of_last_staging = storage.time_of_last_staging;

    // HCV
    data::InfectionType it = data::InfectionType::kHcv;

    // LinkageDetails
    _linkage_details[it].link_state = storage.hcv_link_state;
    _linkage_details[it].time_link_change = storage.time_of_hcv_link_change;
    _linkage_details[it].link_count = storage.hcv_link_count;

    // ScreeningDetails
    _screening_details[it].time_of_last_screening =
        storage.time_of_last_hcv_screening;
    _screening_details[it].num_ab_tests = storage.num_hcv_ab_tests;
    _screening_details[it].num_rna_tests = storage.num_hcv_rna_tests;
    _screening_details[it].ab_positive =
        (storage.hcv_antibody_positive || storage.seropositive);
    _screening_details[it].identified = storage.hcv_identified;
    _screening_details[it].time_identified = storage.time_hcv_identified;
    _screening_details[it].times_identified = storage.times_hcv_identified;
    _screening_details[it].screen_type = storage.hcv_link_type;
    _screening_details[it].num_false_negatives =
        storage.num_hcv_false_negatives;
    _screening_details[it].identifications_cleared =
        storage.hcv_identifications_cleared;

    // TreatmentDetails
    _treatment_details[it].initiated_treatment =
        storage.initiated_hcv_treatment;
    _treatment_details[it].time_of_treatment_initiation =
        storage.time_of_hcv_treatment_initiation;
    _treatment_details[it].num_starts = storage.num_hcv_treatment_starts;
    _treatment_details[it].num_withdrawals =
        storage.num_hcv_treatment_withdrawals;
    _treatment_details[it].num_toxic_reactions =
        storage.num_hcv_treatment_toxic_reactions;
    _treatment_details[it].num_completed = storage.num_completed_hcv_treatments;
    _treatment_details[it].num_salvages = storage.num_hcv_salvages;
    _treatment_details[it].in_salvage_treatment = storage.in_hcv_salvage;

    // HIV
    it = data::InfectionType::kHiv;

    // LinkageDetails
    _linkage_details[it].link_state = storage.hiv_link_state;
    _linkage_details[it].time_link_change = storage.time_of_hiv_link_change;
    _linkage_details[it].link_count = storage.hiv_link_count;

    // ScreeningDetails
    _screening_details[it].time_of_last_screening =
        storage.time_of_last_hiv_screening;
    _screening_details[it].num_ab_tests = storage.num_hiv_ab_tests;
    _screening_details[it].num_rna_tests = storage.num_hiv_rna_tests;
    _screening_details[it].ab_positive = storage.hiv_antibody_positive;
    _screening_details[it].identified = storage.hiv_identified;
    _screening_details[it].time_identified = storage.time_hiv_identified;
    _screening_details[it].times_identified = storage.times_hiv_identified;
    _screening_details[it].screen_type = storage.hiv_link_type;

    // TreatmentDetails
    _treatment_details[it].initiated_treatment =
        storage.initiated_hiv_treatment;
    _treatment_details[it].time_of_treatment_initiation =
        storage.time_of_hiv_treatment_initiation;
    _treatment_details[it].num_starts = storage.num_hiv_treatment_starts;
    _treatment_details[it].num_withdrawals =
        storage.num_hiv_treatment_withdrawals;
    _treatment_details[it].num_toxic_reactions =
        storage.num_hiv_treatment_toxic_reactions;

    // UtilityTracker
    SetUtility(storage.behavior_utility, UtilityCategory::kBehavior);
    SetUtility(storage.liver_utility, UtilityCategory::kLiver);
    SetUtility(storage.treatment_utility, UtilityCategory::kTreatment);
    SetUtility(storage.background_utility, UtilityCategory::kBackground);
    SetUtility(storage.hiv_utility, UtilityCategory::kHiv);
    SetUtility(storage.moud_utility, UtilityCategory::kMoud);
    SetUtility(storage.overdose_utility, UtilityCategory::kOverdose);
}

void PersonImpl::InfectHCV() {
    // cannot be multiply infected
    if (_hcv_details.hcv != data::HCV::kNone) {
        return;
    }
    _hcv_details.hcv = data::HCV::kAcute;
    _hcv_details.time_changed = _current_time;
    _hcv_details.seropositive = true;
    _hcv_details.times_infected++;

    // once infected, immediately enter F0
    if (_hcv_details.fibrosis_state == data::FibrosisState::kNone) {
        SetFibrosis(data::FibrosisState::kF0);
    }
}

void PersonImpl::UpdateTimers() {
    _current_time++;
    if (_behavior_details.behavior == data::Behavior::kNoninjection ||
        _behavior_details.behavior == data::Behavior::kInjection) {
        _behavior_details.time_last_active = _current_time;
    }
    if (_moud_details.moud_state == data::MOUD::kCurrent) {
        _moud_details.total_moud_months++;
    }
    if (_hiv_details.hiv == data::HIV::kLoUn ||
        _hiv_details.hiv == data::HIV::kLoSu) {
        _hiv_details.low_cd4_months_count++;
    }
    _moud_details.current_state_concurrent_months++;
}

void PersonImpl::InitiateTreatment(data::InfectionType it) {
    data::TreatmentDetails &td = _treatment_details[it];
    // cannot continue being treated if already in salvage
    if (td.in_salvage_treatment) {
        return;
    } else if (td.initiated_treatment) {
        td.in_salvage_treatment = true;
        td.num_salvages++;
    } else {
        td.initiated_treatment = true;
        td.num_starts++;
        td.time_of_treatment_initiation = _current_time;
    }
}
void PersonImpl::SetBehavior(data::Behavior bc) {
    // nothing to do -- cannot go back to kNever
    if (bc == _behavior_details.behavior || bc == data::Behavior::kNever) {
        return;
    }
    if (bc == data::Behavior::kNoninjection ||
        bc == data::Behavior::kInjection) {
        _behavior_details.time_last_active = _current_time;
    }
    _behavior_details.behavior = bc;
}
bool PersonImpl::IsCirrhotic() const {
    if (GetHCVDetails().fibrosis_state == data::FibrosisState::kF4 ||
        GetHCVDetails().fibrosis_state == data::FibrosisState::kDecomp) {
        return true;
    }
    return false;
}

// NOTE: A person can be in postpartum state before giving birth (i.e. multiple births)
void PersonImpl::Birth(const data::Child &child) {
    if (child.hcv != data::HCV::kNone) {
        _pregnancy_details.num_hcv_infections++;
    }
    if (child.tested) {
        _pregnancy_details.num_hcv_tests++;
    }
    _pregnancy_details.children.push_back(child);
    _pregnancy_details.num_infants++;
    _pregnancy_details.pregnancy_state =
        data::PregnancyState::kRestrictedPostpartum;
}

void PersonImpl::TransitionMOUD() {
    if (GetBehaviorDetails().behavior == data::Behavior::kNever) {
        return;
    }
    data::MOUD current = GetMoudDetails().moud_state;
    if (current == data::MOUD::kCurrent) {
        SetMoudState(data::MOUD::kPost);
    } else if (current == data::MOUD::kPost) {
        SetMoudState(data::MOUD::kNone);
    } else if (current == data::MOUD::kNone) {
        SetMoudState(data::MOUD::kCurrent);
    }
}

void PersonImpl::SetMoudState(data::MOUD moud) {
    if (moud == data::MOUD::kCurrent) {
        _moud_details.time_started_moud = _current_time;
    }
    _moud_details.current_state_concurrent_months = 0;
    _moud_details.moud_state = moud;
}

void PersonImpl::DevelopHCC(data::HCCState state) {
    data::HCCState current = _hcc_details.hcc_state;
    switch (current) {
    case data::HCCState::kNone:
        _hcc_details.hcc_state = data::HCCState::kEarly;
        break;
    case data::HCCState::kEarly:
        _hcc_details.hcc_state = data::HCCState::kLate;
        break;
    default:
        break;
    }
}
std::string PersonImpl::MakePopulationRow() const {
    std::stringstream population_row;
    // clang-format off
    // basic characteristics
    population_row << _sex << ","
                   << _age << ","
                   << std::boolalpha << _is_alive << ","
                   << std::boolalpha << _boomer_classification << ","
                   << _death_reason << ",";
    // BehaviorDetails
    const auto &bd = _behavior_details;
    population_row << bd.behavior << ","
                   << bd.time_last_active << ",";
    // HCVDetails
    const auto &hcv = _hcv_details;
    population_row << hcv.hcv << ","
                   << hcv.fibrosis_state << ","
                   << std::boolalpha << hcv.is_genotype_three << ","
                   << std::boolalpha << hcv.seropositive << ","
                   << hcv.time_changed << ","
                   << hcv.time_fibrosis_state_changed << ","
                   << hcv.times_infected << ","
                   << hcv.times_acute_cleared << ","
                   << hcv.svrs << ",";
    // HIVDetails
    const auto &hiv = _hiv_details;
    population_row << hiv.hiv << ","
                   << hiv.time_changed << ","
                   << hiv.low_cd4_months_count << ",";
    // HCCDetails
    const auto &hcc = _hcc_details;
    population_row << hcc.hcc_state << ","
                   << std::boolalpha << hcc.hcc_diagnosed << ",";
    // overdose characteristics
    population_row << std::boolalpha << _currently_overdosing << ","
                   << _num_overdoses << ",";
    // MOUDDetails
    const auto &moud = _moud_details;
    population_row << moud.moud_state << ","
                   << moud.time_started_moud << ","
                   << moud.current_state_concurrent_months << ","
                   << moud.total_moud_months << ",";
    // PregnancyDetails
    const auto &pd = _pregnancy_details;
    population_row << pd.pregnancy_state << ","
                   << pd.time_of_pregnancy_change << ","
                   << pd.count << ","
                   << pd.num_infants << ","
                   << pd.num_stillbirths << ","
                   << pd.num_hcv_exposures << ","
                   << pd.num_hcv_infections << ","
                   << pd.num_hcv_tests << ",";
    // StagingDetails
    const auto &sd = _staging_details;
    population_row << sd.measured_fibrosis_state << ","
                   << std::boolalpha << sd.had_second_test << ","
                   << sd.time_of_last_staging << ",";
    // LinkageDetails
    const auto &hcvld = _linkage_details.at(data::InfectionType::kHcv);
    population_row << hcvld.link_state << ","
                   << hcvld.time_link_change << ","
                   << hcvld.link_count << ",";
    const auto &hivld = _linkage_details.at(data::InfectionType::kHiv);
    population_row << hivld.link_state << ","
                   << hivld.time_link_change << ","
                   << hivld.link_count << ",";
    // ScreeningDetails
    const auto &hcvsd = _screening_details.at(data::InfectionType::kHcv);
    population_row << hcvsd.time_of_last_screening << ","
                   << hcvsd.num_ab_tests << ","
                   << hcvsd.num_rna_tests << ","
                   << std::boolalpha << hcvsd.ab_positive << ","
                   << std::boolalpha << hcvsd.identified << ","
                   << hcvsd.time_identified << ","
                   << hcvsd.times_identified << ","
                   << hcvsd.screen_type << ","
                   << hcvsd.num_false_negatives << ","
                   << hcvsd.identifications_cleared << ",";
    const auto &hivsd = _screening_details.at(data::InfectionType::kHiv);
    population_row << hivsd.time_of_last_screening << ","
                   << hivsd.num_ab_tests << ","
                   << hivsd.num_rna_tests << ","
                   << std::boolalpha << hivsd.ab_positive << ","
                   << std::boolalpha << hivsd.identified << ","
                   << hivsd.time_identified << ","
                   << hivsd.times_identified << ","
                   << hivsd.screen_type << ",";
    const auto &hcvtd = _treatment_details.at(data::InfectionType::kHcv);
    population_row << std::boolalpha << hcvtd.initiated_treatment << ","
                   << hcvtd.time_of_treatment_initiation << ","
                   << hcvtd.num_starts << ","
                   << hcvtd.num_withdrawals << ","
                   << hcvtd.num_toxic_reactions << ","
                   << hcvtd.num_completed << ","
                   << hcvtd.num_salvages << ","
                   << std::boolalpha << hcvtd.in_salvage_treatment << ",";
    const auto &hivtd = _treatment_details.at(data::InfectionType::kHiv);
    population_row << std::boolalpha << hivtd.initiated_treatment << ","
                   << hivtd.time_of_treatment_initiation << ","
                   << hivtd.num_starts << ","
                   << hivtd.num_withdrawals << ","
                   << hivtd.num_toxic_reactions << ",";
    // Utilities
    // current utilities
    const auto &cu = GetUtilities();
    population_row << cu.at(model::UtilityCategory::kBehavior) << ","
                   << cu.at(model::UtilityCategory::kLiver) << ","
                   << cu.at(model::UtilityCategory::kTreatment) << ","
                   << cu.at(model::UtilityCategory::kBackground) << ","
                   << cu.at(model::UtilityCategory::kHiv) << ","
                   << cu.at(model::UtilityCategory::kMoud) << ","
                   << cu.at(model::UtilityCategory::kOverdose) << ",";
    // total/lifetime utilities
    const auto &tu = GetTotalUtility();
    population_row << tu.min_util << ","
                   << tu.mult_util << ","
                   << tu.discount_min_util << ","
                   << tu.discount_mult_util << ",";
    // lifespan
    population_row << _life_span << ","
                   << _discounted_life_span << ",";

    // Cost Totals
    const std::pair<double, double> &ct = GetCostTotals();
    population_row << ct.first << ","
                   << ct.second;
    // clang-format on
    return population_row.str();
}
} // namespace model
} // namespace hepce
