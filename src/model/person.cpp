////////////////////////////////////////////////////////////////////////////////
// File: person.cpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created Date: Mo Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/model/person.hpp>

#include "internals/person_internals.hpp"

namespace hepce {
namespace model {
namespace person {
PersonImpl::PersonImpl(const std::string &log_name) {}

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
        UpdateTrueFibrosis(data::FibrosisState::kF0);
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
    if (_hiv_details.hiv == data::HIV::LoUn ||
        _hiv_details.hiv == data::HIV::LoSu) {
        _hiv_details.low_cd4_months_count++;
    }
    _moud_details.current_state_concurrent_months++;
}

void PersonImpl::InitiateTreatment(data::InfectionType it) {
    data::TreatmentDetails &td = _treatment_details[it];
    // cannot continue being treated if already in retreatment
    if (td.initiated_treatment && td.retreatment) {
        return;
    } else if (td.initiated_treatment) {
        td.retreatment = true;
        td.num_retreatments++;
    } else {
        td.initiated_treatment = true;
    }
    // treatment starts counts treatment regimens
    td.num_starts++;
    td.time_of_treatment_initiation = _current_time;
}
void PersonImpl::SetBehavior(data::Behavior bc) {
    // nothing to do -- cannot go back to kNever
    if (bc == _behavior_details.behavior || bc == data::Behavior::kNever) {
        return;
    }
    if ((bc == data::Behavior::kNoninjection ||
         bc == data::Behavior::kInjection) &&
        (_behavior_details.behavior == data::Behavior::kNever ||
         _behavior_details.behavior == data::Behavior::kFormerInjection ||
         _behavior_details.behavior == data::Behavior::kFormerNoninjection)) {
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
std::string PersonImpl::GetPersonDataString() const {
    std::stringstream data;
    data
        << GetAge() << "," << GetSex() << "," << GetBehaviorDetails().behavior
        << "," << GetBehaviorDetails().time_last_active << ","
        << GetHCVDetails().seropositive << ","
        << GetHCVDetails().is_genotype_three << ","
        << GetHCVDetails().fibrosis_state << ","
        << GetScreeningDetails(data::InfectionType::kHcv).identified << ","
        << GetLinkageDetails(data::InfectionType::kHcv).link_state << ","
        << IsAlive() << "," << GetDeathReason() << ","
        << GetScreeningDetails(data::InfectionType::kHcv).time_identified << ","
        << GetHCVDetails().hcv << "," << GetHCVDetails().time_changed << ","
        << GetHCVDetails().time_fibrosis_state_changed << ","
        << GetBehaviorDetails().time_last_active << ","
        << GetLinkageDetails(data::InfectionType::kHcv).time_link_change << ","
        << GetLinkageDetails(data::InfectionType::kHcv).link_type << ","
        << GetLinkageDetails(data::InfectionType::kHcv).link_count << ","
        << GetFibrosisStagingDetails().measured_fibrosis_state << ","
        << GetFibrosisStagingDetails().time_of_last_staging << ","
        << GetScreeningDetails(data::InfectionType::kHcv).time_of_last_screening
        << "," << GetScreeningDetails(data::InfectionType::kHcv).number_ab_tests
        << ","
        << GetScreeningDetails(data::InfectionType::kHcv).number_rna_tests
        << "," << GetHCVDetails().times_infected << ","
        << GetHCVDetails().times_acute_cleared << "," << std::boolalpha
        << GetTreatmentDetails(data::InfectionType::kHcv).initiated_treatment
        << ","
        << GetTreatmentDetails(data::InfectionType::kHcv)
               .time_of_treatment_initiation
        << "," << std::to_string(GetTotalUtility().min_util) << ","
        << std::to_string(GetTotalUtility().mult_util);
    return data.str();
}
inline void PersonImpl::AddChild(data::HCV hcv, bool test) {}
void PersonImpl::TransitionMOUD() {}
void PersonImpl::DevelopHCC(data::HCCState state) {}
std::string PersonImpl::MakePopulationRow() const {}
} // namespace person
} // namespace model
} // namespace hepce