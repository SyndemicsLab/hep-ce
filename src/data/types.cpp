////////////////////////////////////////////////////////////////////////////////
// File: types.cpp                                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-04-30                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-23                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// File Header
#include <hepce/data/types.hpp>
#include <hepce/utils/formatting.hpp>

#include <tuple>

namespace hepce {
namespace data {
std::ostream &operator<<(std::ostream &os, const InfectionType &inst) {
    switch (inst) {
    case InfectionType::kHcv:
        os << "kHcv";
        break;
    case InfectionType::kHiv:
        os << "kHiv";
        break;
    default:
        os << "INVALID";
        break;
    }
    return os;
}
InfectionType &operator<<(InfectionType &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "hiv") {
        inst = InfectionType::kHiv;
    } else {
        inst = InfectionType::kHcv;
    }
    return inst;
}
InfectionType &operator++(InfectionType &inst) {
    switch (inst) {
    case InfectionType::kHcv:
        inst = InfectionType::kHiv;
        break;
    case InfectionType::kHiv:
        inst = InfectionType::kCount;
        break;
    default:
        break;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const HCV &inst) {
    switch (inst) {
    case HCV::kAcute:
        os << "acute";
        break;
    case HCV::kChronic:
        os << "chronic";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
HCV &operator<<(HCV &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "acute") {
        inst = HCV::kAcute;
    } else if (temp_string == "chronic") {
        inst = HCV::kChronic;
    } else {
        inst = HCV::kNone;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const HIV &inst) {
    switch (inst) {
    case HIV::kHiUn:
        os << "hi-un";
        break;
    case HIV::kHiSu:
        os << "hi-su";
        break;
    case HIV::kLoUn:
        os << "lo-un";
        break;
    case HIV::kLoSu:
        os << "lo-su";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
HIV &operator<<(HIV &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "hi-un") {
        inst = HIV::kHiUn;
    } else if (temp_string == "hi-su") {
        inst = HIV::kHiSu;
    } else if (temp_string == "lo-un") {
        inst = HIV::kLoUn;
    } else if (temp_string == "lo-su") {
        inst = HIV::kLoSu;
    } else {
        inst = HIV::kNone;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const DeathReason &inst) {
    switch (inst) {
    case DeathReason::kBackground:
        os << "background";
        break;
    case DeathReason::kLiver:
        os << "liver";
        break;
    case DeathReason::kInfection:
        os << "infection";
        break;
    case DeathReason::kAge:
        os << "age";
        break;
    case DeathReason::kOverdose:
        os << "overdose";
        break;
    default:
        os << "na";
        break;
    }
    return os;
}
DeathReason &operator<<(DeathReason &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "background") {
        inst = DeathReason::kBackground;
    } else if (temp_string == "liver") {
        inst = DeathReason::kLiver;
    } else if (temp_string == "infection") {
        inst = DeathReason::kInfection;
    } else if (temp_string == "age") {
        inst = DeathReason::kAge;
    } else if (temp_string == "overdose") {
        inst = DeathReason::kOverdose;
    } else {
        inst = DeathReason::kNa;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const Behavior &inst) {
    switch (inst) {
    case Behavior::kFormerInjection:
        os << "former_injection";
        break;
    case Behavior::kFormerNoninjection:
        os << "former_noninjection";
        break;
    case Behavior::kInjection:
        os << "injection";
        break;
    case Behavior::kNoninjection:
        os << "noninjection";
        break;
    default:
        os << "never";
        break;
    }
    return os;
}
Behavior &operator<<(Behavior &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "former_injection") {
        inst = Behavior::kFormerInjection;
    } else if (temp_string == "former_noninjection") {
        inst = Behavior::kFormerNoninjection;
    } else if (temp_string == "injection") {
        inst = Behavior::kInjection;
    } else if (temp_string == "noninjection") {
        inst = Behavior::kNoninjection;
    } else {
        inst = Behavior::kNever;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const ScreeningType &inst) {
    switch (inst) {
    case ScreeningType::kBackground:
        os << "background";
        break;
    case ScreeningType::kIntervention:
        os << "intervention";
        break;
    default:
        os << "na";
        break;
    }
    return os;
}
ScreeningType &operator<<(ScreeningType &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "background") {
        inst = ScreeningType::kBackground;
    } else if (temp_string == "intervention") {
        inst = ScreeningType::kIntervention;
    } else {
        inst = ScreeningType::kNa;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const ScreeningTest &inst) {
    switch (inst) {
    case ScreeningTest::kAb:
        os << "antibody";
        break;
    case ScreeningTest::kRna:
        os << "rna";
        break;
    default:
        os << "na";
        break;
    }
    return os;
}
ScreeningTest &operator<<(ScreeningTest &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "antibody") {
        inst = ScreeningTest::kAb;
    } else if (temp_string == "rna") {
        inst = ScreeningTest::kRna;
    } else {
        inst = ScreeningTest::kNa;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const LinkageState &inst) {
    switch (inst) {
    case LinkageState::kLinked:
        os << "linked";
        break;
    case LinkageState::kUnlinked:
        os << "unlinked";
        break;
    default:
        os << "never";
        break;
    }
    return os;
}
LinkageState &operator<<(LinkageState &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "linked") {
        inst = LinkageState::kLinked;
    } else if (temp_string == "unlinked") {
        inst = LinkageState::kUnlinked;
    } else {
        inst == LinkageState::kNever;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const FibrosisState &inst) {
    switch (inst) {
    case FibrosisState::kDecomp:
        os << "decomp";
        break;
    case FibrosisState::kF0:
        os << "f0";
        break;
    case FibrosisState::kF1:
        os << "f1";
        break;
    case FibrosisState::kF2:
        os << "f2";
        break;
    case FibrosisState::kF3:
        os << "f3";
        break;
    case FibrosisState::kF4:
        os << "f4";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
FibrosisState &operator<<(FibrosisState &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "decomp") {
        inst = FibrosisState::kDecomp;
    } else if (temp_string == "f0") {
        inst = FibrosisState::kF0;
    } else if (temp_string == "f1") {
        inst = FibrosisState::kF1;
    } else if (temp_string == "f2") {
        inst = FibrosisState::kF2;
    } else if (temp_string == "f3") {
        inst = FibrosisState::kF3;
    } else if (temp_string == "f4") {
        inst = FibrosisState::kF4;
    } else {
        inst = FibrosisState::kNone;
    }
    return inst;
}

FibrosisState &operator++(FibrosisState &inst) {
    switch (inst) {
    case FibrosisState::kF0:
        inst = FibrosisState::kF1;
        break;
    case FibrosisState::kF1:
        inst = FibrosisState::kF2;
        break;
    case FibrosisState::kF2:
        inst = FibrosisState::kF3;
        break;
    case FibrosisState::kF3:
        inst = FibrosisState::kF4;
        break;
    case FibrosisState::kF4:
        inst = FibrosisState::kDecomp;
        break;
    case FibrosisState::kDecomp:
        inst = FibrosisState::kDecomp;
        break;
    default:
        break;
    }
    return inst;
}
FibrosisState operator++(FibrosisState &inst, int) {
    FibrosisState rValue = inst;
    ++inst;
    return rValue;
}

std::ostream &operator<<(std::ostream &os, const HCCState &inst) {
    switch (inst) {
    case HCCState::kEarly:
        os << "early";
        break;
    case HCCState::kLate:
        os << "late";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
HCCState &operator<<(HCCState &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "early") {
        inst = HCCState::kEarly;
    } else if (temp_string == "late") {
        inst = HCCState::kLate;
    } else {
        inst = HCCState::kNone;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const MeasuredFibrosisState &inst) {
    switch (inst) {
    case MeasuredFibrosisState::kDecomp:
        os << "decomp";
        break;
    case MeasuredFibrosisState::kF01:
        os << "f01";
        break;
    case MeasuredFibrosisState::kF23:
        os << "f23";
        break;
    case MeasuredFibrosisState::kF4:
        os << "f4";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
MeasuredFibrosisState &operator<<(MeasuredFibrosisState &inst,
                                  const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "decomp") {
        inst = MeasuredFibrosisState::kDecomp;
    } else if (temp_string == "f01") {
        inst = MeasuredFibrosisState::kF01;
    } else if (temp_string == "f23") {
        inst = MeasuredFibrosisState::kF23;
    } else if (temp_string == "f4") {
        inst = MeasuredFibrosisState::kF4;
    } else {
        inst = MeasuredFibrosisState::kNone;
    }
    return inst;
}

MeasuredFibrosisState &operator++(MeasuredFibrosisState &inst) {
    switch (inst) {
    case MeasuredFibrosisState::kF01:
        inst = MeasuredFibrosisState::kF23;
        break;
    case MeasuredFibrosisState::kF23:
        inst = MeasuredFibrosisState::kF4;
        break;
    case MeasuredFibrosisState::kF4:
        inst = MeasuredFibrosisState::kDecomp;
        break;
    case MeasuredFibrosisState::kDecomp:
        inst = MeasuredFibrosisState::kDecomp;
        break;
    default:
        break;
    }
    return inst;
}
MeasuredFibrosisState operator++(MeasuredFibrosisState &inst, int) {
    MeasuredFibrosisState rValue = inst;
    ++inst;
    return rValue;
}

std::ostream &operator<<(std::ostream &os, const MOUD &inst) {
    switch (inst) {
    case MOUD::kCurrent:
        os << "current";
        break;
    case MOUD::kPost:
        os << "post";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
MOUD &operator<<(MOUD &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "current") {
        inst = MOUD::kCurrent;
    } else if (temp_string == "post") {
        inst = MOUD::kPost;
    } else {
        inst = MOUD::kNone;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const Sex &inst) {
    switch (inst) {
    case Sex::kMale:
        os << "male";
        break;
    case Sex::kFemale:
        os << "female";
        break;
    default:
        os << "";
        break;
    }
    return os;
}
Sex &operator<<(Sex &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "male") {
        inst = Sex::kMale;
    } else {
        inst = Sex::kFemale;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const PregnancyState &inst) {
    switch (inst) {
    case PregnancyState::kRestrictedPostpartum:
        os << "restricted-postpartum";
        break;
    case PregnancyState::kYearOnePostpartum:
        os << "year-one-postpartum";
        break;
    case PregnancyState::kYearTwoPostpartum:
        os << "year-two-postpartum";
        break;
    case PregnancyState::kPregnant:
        os << "pregnant";
        break;
    case PregnancyState::kNone:
        os << "none";
        break;
    default:
        os << "na";
        break;
    }
    return os;
}
PregnancyState &operator<<(PregnancyState &inst, const std::string &str) {
    const std::string temp_string = utils::ToLower(str);
    if (temp_string == "restricted_postpartum") {
        inst = PregnancyState::kRestrictedPostpartum;
    } else if (temp_string == "year_one_postpartum") {
        inst = PregnancyState::kYearOnePostpartum;
    } else if (temp_string == "year_two_postpartum") {
        inst = PregnancyState::kYearTwoPostpartum;
    } else if (temp_string == "pregnant") {
        inst = PregnancyState::kPregnant;
    } else if (temp_string == "none") {
        inst = PregnancyState::kNone;
    } else {
        inst = PregnancyState::kNa;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, HCVDetails const &inst) {
    os << "HCV State: " << inst.hcv << std::endl;
    os << "Fibrosis State: " << inst.fibrosis_state << std::endl;
    os << "Genotype 3 Infection: " << std::boolalpha << inst.is_genotype_three
       << std::endl;
    os << "Seropositive: " << std::boolalpha << inst.seropositive << std::endl;
    os << "Last Time the HCV State Changed: " << inst.time_changed << std::endl;
    os << "Last Time the Fibrosis State Changed: "
       << inst.time_fibrosis_state_changed << std::endl;
    os << "Number of Times Infected: " << inst.times_infected << std::endl;
    os << "Number of Times Spontaneously Cleared: " << inst.times_acute_cleared
       << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, HIVDetails const &inst) {
    os << "HIV State: " << inst.hiv << std::endl;
    os << "Last Time the HIV State Changed: " << inst.time_changed << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, HCCDetails const &inst) {
    os << "HCC State: " << inst.hcc_state << std::endl;
    os << "HCC Diagnosed: " << std::boolalpha << inst.hcc_diagnosed
       << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, BehaviorDetails const &behav) {
    os << "Behavior Type: " << behav.behavior << std::endl;
    os << "Last Timestep Active: " << behav.time_last_active << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, LinkageDetails const &ldet) {
    os << "Linkage State: " << ldet.link_state << std::endl;
    os << "Last Time Linkage State Changed: " << ldet.time_link_change
       << std::endl;
    os << "Times Linked: " << ldet.link_count << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, MOUDDetails const &mdet) {
    os << "MOUD State: " << mdet.moud_state << std::endl;
    os << "Time Started MOUD: " << mdet.time_started_moud << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, PregnancyDetails const &pdet) {
    os << "Pregnancy State: " << pdet.pregnancy_state << std::endl;
    os << "Time of Pregnancy State Change: " << pdet.time_of_pregnancy_change
       << std::endl;
    os << "Number of Pregnancies: " << pdet.count << std::endl;
    os << "Infant Count: " << pdet.num_infants << std::endl;
    os << "Stillbirth Count: " << pdet.num_stillbirths << std::endl;
    os << "Infant HCV Exposure Count: " << pdet.num_hcv_exposures << std::endl;
    os << "Infant HCV Infection Count: " << pdet.num_hcv_infections
       << std::endl;
    os << "Infant HCV Test Count: " << pdet.num_hcv_tests << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, Child const &inst) {
    os << "HCV State: " << inst.hcv << std::endl;
    os << "Tested for HCV: " << inst.tested << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, StagingDetails const &sdet) {
    os << "Fibrosis State: " << sdet.measured_fibrosis_state << std::endl;
    os << "Has a Second Fibrosis Test: " << sdet.had_second_test << std::endl;
    os << "Time of Last Staging: " << sdet.time_of_last_staging << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, ScreeningDetails const &sdet) {
    os << "Time of Last Screening: " << sdet.time_of_last_screening
       << std::endl;
    os << "AB Test Counts: " << sdet.num_ab_tests << std::endl;
    os << "RNA Testing Counts: " << sdet.num_rna_tests << std::endl;
    os << "Antibody Positive: " << std::boolalpha << sdet.ab_positive
       << std::endl;
    os << "Is Identified: " << std::boolalpha << sdet.identified << std::endl;
    os << "Time Identified: " << sdet.time_identified << std::endl;
    os << "Screening Type: " << sdet.screen_type << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, TreatmentDetails const &tdet) {
    os << "Has the person Initiated Treatment: " << tdet.initiated_treatment
       << std::endl;
    os << "Time Person Initiated Treatment: "
       << tdet.time_of_treatment_initiation << std::endl;
    os << "Number of Treatment Initializations: " << tdet.num_starts
       << std::endl;
    os << "Number of Treatment Withdrawals: " << tdet.num_withdrawals
       << std::endl;
    os << "Number of Toxic Treatment Reactions: " << tdet.num_toxic_reactions
       << std::endl;
    os << "Number of Treatment Completions: " << tdet.num_completed
       << std::endl;
    os << "Number of Salvages: " << tdet.num_salvages << std::endl;
    os << "Person is in Salvage Treatment: " << std::boolalpha
       << tdet.in_salvage_treatment << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, LifetimeUtility const &lu) {
    os << "Multiplicative Utility: " << lu.mult_util << std::endl;
    os << "Minimum Utility: " << lu.min_util << std::endl;
    os << "Discounted Multiplicative Utility: " << lu.discount_mult_util
       << std::endl;
    os << "Discounted Minimum Utility: " << lu.discount_min_util << std::endl;
    return os;
}

bool operator==(LifetimeUtility const &lhs, LifetimeUtility const &rhs) {
    return (std::tie(lhs.min_util, lhs.mult_util, lhs.discount_min_util,
                     lhs.discount_mult_util) ==
            std::tie(rhs.min_util, rhs.mult_util, rhs.discount_min_util,
                     rhs.discount_mult_util));
}
} // namespace data
} // namespace hepce
