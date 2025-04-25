////////////////////////////////////////////////////////////////////////////////
// File: ContainerOperators.cpp                                               //
// Project: HEPCESimulationv2                                                 //
// Created: 2024-09-03                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2024-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Containers.hpp"

namespace person {
std::ostream &operator<<(std::ostream &os, const InfectionType &inst) {
    switch (inst) {
    case InfectionType::HCV:
        os << "HCV";
        break;
    case InfectionType::HIV:
        os << "HIV";
        break;
    default:
        os << "INVALID";
        break;
    }
    return os;
}
InfectionType &operator<<(InfectionType &inst, const std::string &str) {
    if (str == "HIV") {
        inst = InfectionType::HIV;
    } else {
        inst = InfectionType::HCV;
    }
    return inst;
}
InfectionType &operator++(InfectionType &inst) {
    switch (inst) {
    case InfectionType::HCV:
        inst = InfectionType::HIV;
        break;
    case InfectionType::HIV:
        inst = InfectionType::COUNT;
        break;
    default:
        break;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const HCV &inst) {
    switch (inst) {
    case HCV::ACUTE:
        os << "acute";
        break;
    case HCV::CHRONIC:
        os << "chronic";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
HCV &operator<<(HCV &inst, const std::string &str) {
    if (str == "acute") {
        inst = HCV::ACUTE;
    } else if (str == "chronic") {
        inst = HCV::CHRONIC;
    } else {
        inst = HCV::NONE;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const HIV &inst) {
    switch (inst) {
    case HIV::HIUN:
        os << "hi-un";
        break;
    case HIV::HISU:
        os << "hi-su";
        break;
    case HIV::LOUN:
        os << "lo-un";
        break;
    case HIV::LOSU:
        os << "lo-su";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
HIV &operator<<(HIV &inst, const std::string &str) {
    if (str == "hi-un") {
        inst = HIV::HIUN;
    } else if (str == "hi-su") {
        inst = HIV::HISU;
    } else if (str == "lo-un") {
        inst = HIV::LOUN;
    } else if (str == "lo-su") {
        inst = HIV::LOSU;
    } else {
        inst = HIV::NONE;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const DeathReason &inst) {
    switch (inst) {
    case DeathReason::BACKGROUND:
        os << "background";
        break;
    case DeathReason::LIVER:
        os << "liver";
        break;
    case DeathReason::INFECTION:
        os << "infection";
        break;
    case DeathReason::AGE:
        os << "age";
        break;
    case DeathReason::OVERDOSE:
        os << "overdose";
        break;
    default:
        os << "na";
        break;
    }
    return os;
}
DeathReason &operator<<(DeathReason &inst, const std::string &str) {
    if (str == "background") {
        inst = DeathReason::BACKGROUND;
    } else if (str == "liver") {
        inst = DeathReason::LIVER;
    } else if (str == "infection") {
        inst = DeathReason::INFECTION;
    } else if (str == "age") {
        inst = DeathReason::AGE;
    } else if (str == "overdose") {
        inst = DeathReason::OVERDOSE;
    } else {
        inst = DeathReason::NA;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const Behavior &inst) {
    switch (inst) {
    case Behavior::FORMER_INJECTION:
        os << "former_injection";
        break;
    case Behavior::FORMER_NONINJECTION:
        os << "former_noninjection";
        break;
    case Behavior::INJECTION:
        os << "injection";
        break;
    case Behavior::NONINJECTION:
        os << "noninjection";
        break;
    default:
        os << "never";
        break;
    }
    return os;
}
Behavior &operator<<(Behavior &inst, const std::string &str) {
    if (str == "former_injection") {
        inst = Behavior::FORMER_INJECTION;
    } else if (str == "former_noninjection") {
        inst = Behavior::FORMER_NONINJECTION;
    } else if (str == "injection") {
        inst = Behavior::INJECTION;
    } else if (str == "noninjection") {
        inst = Behavior::NONINJECTION;
    } else {
        inst = Behavior::NEVER;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const LinkageType &inst) {
    switch (inst) {
    case LinkageType::BACKGROUND:
        os << "background";
        break;
    case LinkageType::INTERVENTION:
        os << "intervention";
        break;
    default:
        os << "na";
        break;
    }
    return os;
}
LinkageType &operator<<(LinkageType &inst, const std::string &str) {
    if (str == "background") {
        inst = LinkageType::BACKGROUND;
    } else if (str == "intervention") {
        inst = LinkageType::INTERVENTION;
    } else {
        inst = LinkageType::NA;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const LinkageState &inst) {
    switch (inst) {
    case LinkageState::LINKED:
        os << "linked";
        break;
    case LinkageState::UNLINKED:
        os << "unlinked";
        break;
    default:
        os << "never";
        break;
    }
    return os;
}
LinkageState &operator<<(LinkageState &inst, const std::string &str) {
    if (str == "linked") {
        inst = LinkageState::LINKED;
    } else if (str == "unlinked") {
        inst = LinkageState::UNLINKED;
    } else {
        inst == LinkageState::NEVER;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const FibrosisState &inst) {
    switch (inst) {
    case FibrosisState::DECOMP:
        os << "decomp";
        break;
    case FibrosisState::F0:
        os << "f0";
        break;
    case FibrosisState::F1:
        os << "f1";
        break;
    case FibrosisState::F2:
        os << "f2";
        break;
    case FibrosisState::F3:
        os << "f3";
        break;
    case FibrosisState::F4:
        os << "f4";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
FibrosisState &operator<<(FibrosisState &inst, const std::string &str) {
    if (str == "decomp") {
        inst = FibrosisState::DECOMP;
    } else if (str == "f0") {
        inst = FibrosisState::F0;
    } else if (str == "f1") {
        inst = FibrosisState::F1;
    } else if (str == "f2") {
        inst = FibrosisState::F2;
    } else if (str == "f3") {
        inst = FibrosisState::F3;
    } else if (str == "f4") {
        inst = FibrosisState::F4;
    } else {
        inst = FibrosisState::NONE;
    }
    return inst;
}

FibrosisState &operator++(FibrosisState &inst) {
    switch (inst) {
    case FibrosisState::F0:
        inst = FibrosisState::F1;
        break;
    case FibrosisState::F1:
        inst = FibrosisState::F2;
        break;
    case FibrosisState::F2:
        inst = FibrosisState::F3;
        break;
    case FibrosisState::F3:
        inst = FibrosisState::F4;
        break;
    case FibrosisState::F4:
        inst = FibrosisState::DECOMP;
        break;
    case FibrosisState::DECOMP:
        inst = FibrosisState::DECOMP;
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
    case HCCState::EARLY:
        os << "early";
        break;
    case HCCState::LATE:
        os << "late";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
HCCState &operator<<(HCCState &inst, const std::string &str) {
    if (str == "early") {
        inst = HCCState::EARLY;
    } else if (str == "late") {
        inst = HCCState::LATE;
    } else {
        inst = HCCState::NONE;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const MeasuredFibrosisState &inst) {
    switch (inst) {
    case MeasuredFibrosisState::DECOMP:
        os << "decomp";
        break;
    case MeasuredFibrosisState::F01:
        os << "f01";
        break;
    case MeasuredFibrosisState::F23:
        os << "f23";
        break;
    case MeasuredFibrosisState::F4:
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
    if (str == "decomp") {
        inst = MeasuredFibrosisState::DECOMP;
    } else if (str == "f01") {
        inst = MeasuredFibrosisState::F01;
    } else if (str == "f23") {
        inst = MeasuredFibrosisState::F23;
    } else if (str == "f4") {
        inst = MeasuredFibrosisState::F4;
    } else {
        inst = MeasuredFibrosisState::NONE;
    }
    return inst;
}

MeasuredFibrosisState &operator++(MeasuredFibrosisState &inst) {
    switch (inst) {
    case MeasuredFibrosisState::F01:
        inst = MeasuredFibrosisState::F23;
        break;
    case MeasuredFibrosisState::F23:
        inst = MeasuredFibrosisState::F4;
        break;
    case MeasuredFibrosisState::F4:
        inst = MeasuredFibrosisState::DECOMP;
        break;
    case MeasuredFibrosisState::DECOMP:
        inst = MeasuredFibrosisState::DECOMP;
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
    case MOUD::CURRENT:
        os << "current";
        break;
    case MOUD::POST:
        os << "post";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
MOUD &operator<<(MOUD &inst, const std::string &str) {
    if (str == "current") {
        inst = MOUD::CURRENT;
    } else if (str == "post") {
        inst = MOUD::POST;
    } else {
        inst = MOUD::NONE;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const Sex &inst) {
    switch (inst) {
    case Sex::MALE:
        os << "male";
        break;
    case Sex::FEMALE:
        os << "female";
        break;
    default:
        os << "";
        break;
    }
    return os;
}
Sex &operator<<(Sex &inst, const std::string &str) {
    if (str == "male") {
        inst = Sex::MALE;
    } else {
        inst = Sex::FEMALE;
    }
    return inst;
}

std::ostream &operator<<(std::ostream &os, const PregnancyState &inst) {
    switch (inst) {
    case PregnancyState::POSTPARTUM:
        os << "postpartum";
        break;
    case PregnancyState::PREGNANT:
        os << "pregnant";
        break;
    case PregnancyState::NA:
        os << "na";
        break;
    default:
        os << "none";
        break;
    }
    return os;
}
PregnancyState &operator<<(PregnancyState &inst, const std::string &str) {
    if (str == "postpartum") {
        inst = PregnancyState::POSTPARTUM;
    } else if (str == "pregnant") {
        inst = PregnancyState::PREGNANT;
    } else if (str == "none") {
        inst = PregnancyState::NONE;
    } else {
        inst = PregnancyState::NA;
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
    os << "Last Time Linkage State Changed: " << ldet.time_of_link_change
       << std::endl;
    os << "Linkage Type: " << ldet.link_type << std::endl;
    os << "Times Linked: " << ldet.link_count << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, MOUDDetails const &mdet) {
    os << "MOUD State: " << mdet.moud_state << std::endl;
    os << "Time Started MOUD: " << mdet.time_started << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, PregnancyDetails const &pdet) {
    os << "Pregnancy State: " << pdet.pregnancy_state << std::endl;
    os << "Time of Pregnancy State Change: " << pdet.time_of_pregnancy_change
       << std::endl;
    os << "Number of Pregnancies: " << pdet.count << std::endl;
    os << "Infant Count: " << pdet.num_infants << std::endl;
    os << "Miscarriage Count: " << pdet.num_miscarriages << std::endl;
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
    os << "Antibody Positive: " << std::boolalpha << sdet.antibody_positive
       << std::endl;
    os << "Is Identified: " << std::boolalpha << sdet.identified << std::endl;
    os << "Time Identified: " << sdet.time_identified << std::endl;
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
    os << "Number of Retreatments: " << tdet.num_withdrawals << std::endl;
    os << "Person is in Retreatment: " << std::boolalpha << tdet.retreatment
       << std::endl;
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
} // namespace person
