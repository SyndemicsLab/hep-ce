////////////////////////////////////////////////////////////////////////////////
// File: types.hpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_DATA_TYPES_HPP_
#define HEPCE_DATA_TYPES_HPP_

#include <ostream>
#include <sstream>
#include <vector>

namespace hepce {
namespace data {
inline static const std::string
POPULATION_HEADERS(bool pregnancy = false, bool hcc = false,
                   bool overdose = false, bool hiv = false, bool moud = false) {
    std::stringstream headers;
    // basic characteristics
    headers << "sex,age,is_alive,boomer_classification,death_reason,";
    // BehaviorDetails
    headers << "drug_behavior,time_last_active_drug_use,";
    // HCVDetails
    headers << "hcv,fibrosis_state,is_genotype_three,seropositive,time_hcv_"
               "changed,time_fibrosis_state_changed,times_hcv_infected,times_"
               "acute_cleared,svrs,";
    // HIVDetails
    std::string hiv_details =
        hiv ? "hiv,time_hiv_changed,low_cd4_months_count," : "none,-1,0,";
    headers << hiv_details;
    // HCCDetails
    std::string hcc_details = hcc ? "hcc_state,hcc_diagnosed," : "none,false,";
    headers << hcc_details;
    // overdose characteristics
    std::string overdoses =
        overdose ? "currently_overdosing,num_overdoses," : "false,0,";
    headers << overdoses;
    // MOUDDetails
    std::string moud_details =
        moud ? "moud_state,time_started_moud,current_moud_state_concurrent_"
               "months,total_moud_months,"
             : "none,-1,0,0,";
    headers << moud_details;
    // PregnancyDetails
    std::string pregnancy_details =
        pregnancy ? "pregnancy_state,time_of_pregnancy_change,pregnancy_count,"
                    "num_infants,num_miscarriages,num_infant_hcv_exposures,num_"
                    "infant_hcv_infections,num_infant_hcv_tests,"
                  : "na,-1,0,0,0,0,0,0,";
    headers << pregnancy_details;
    // StagingDetails
    headers << "measured_fibrosis_state,had_second_test,time_of_last_staging,";
    // LinkageDetails
    headers << "hcv_link_state,time_of_hcv_link_change,hcv_link_type,hcv_link_"
               "count,";
    if (hiv) {
        headers << "hiv_link_state,time_of_hiv_link_change,hiv_link_type,hiv_"
                   "link_count,";
    } else {
        headers << "never,-1,na,0,";
    }
    // ScreeningDetails
    headers << "time_of_last_hcv_screening,num_hcv_ab_tests,num_hcv_rna_tests,"
               "hcv_antibody_positive,hcv_identified,time_hcv_identified,";
    if (hiv) {
        headers
            << "time_of_last_hiv_screening,num_hiv_ab_tests,num_hiv_rna_tests,"
               "hiv_antibody_positive,hiv_identified,time_hiv_identified,";
    } else {
        headers << "-1,0,0,false,false,-1,";
    }
    // TreatmentDetails
    headers << "initiated_hcv_treatment,time_of_hcv_treatment_initiation,num_"
               "hcv_treatment_starts,num_hcv_treatment_withdrawals,num_hcv_"
               "treatment_toxic_reactions,num_completed_hcv_treatments,num_hcv_"
               "retreatments,in_hcv_retreatment,";
    if (hiv) {
        headers << "initiated_hiv_treatment,time_of_hiv_treatment_initiation,"
                   "num_hiv_treatment_starts,num_hiv_treatment_withdrawals,num_"
                   "hiv_treatment_toxic_reactions,";
    } else {
        headers << "false,-1,0,0,0,";
    }
    // Utility - NOTE: While LifetimeUtility values are listed here, they cannot
    // be assigned when creating a new Person. They are effectively read-only.
    headers << "behavior_utility,liver_utility,treatment_utility,background_"
               "utility,";
    if (hiv) {
        headers << "hiv_utility,";
    } else {
        headers << "1.0,";
    }
    headers << "min_utility,mult_utility,discounted_min_utility,discounted_"
               "mult_utility,";
    // Lifespan
    headers << "life_span,discounted_life_span";
    return headers.str();
}

/// @brief Infection types tracked for all Persons
enum class InfectionType : int {
    /// Hepatitis C Virus
    kHcv = 0,
    /// Human Immunodeficiency Virus
    kHiv = 1,
    kCount = 2
};
std::ostream &operator<<(std::ostream &os, const InfectionType &inst);
InfectionType &operator<<(InfectionType &inst, const std::string &str);
InfectionType &operator++(InfectionType &inst);

/// @brief HEP-C Infection States
enum class HCV : int {
    /// No HCV infection
    kNone = 0,
    /// New HCV infection; sub-6 months infected
    kAcute = 1,
    /// Long-term HCV infection
    kChronic = 2,

    kCount = 3
};
std::ostream &operator<<(std::ostream &os, const HCV &inst);
HCV &operator<<(HCV &inst, const std::string &str);

/// @brief HIV Infection States
enum class HIV : int {
    /// No HIV infection
    kNone = 0,
    /// High CD4 count, unsuppressed
    kHiUn = 1,
    /// High CD4 count, suppressed
    kHiSu = 2,
    /// Low CD4 count, unsuppressed
    kLoUn = 3,
    /// Low CD4 count, suppressed
    kLoSu = 4,

    kCount = 5
};
std::ostream &operator<<(std::ostream &os, const HIV &inst);
HIV &operator<<(HIV &inst, const std::string &str);

enum class DeathReason : int {
    /// @brief Not applicable, usually not dead
    kNa = -1,
    kBackground = 0,
    kLiver = 1,
    kInfection = 2,
    kAge = 3,
    kOverdose = 4,
    kHiv = 5,
    kCount = 6
};
std::ostream &operator<<(std::ostream &os, const DeathReason &inst);
DeathReason &operator<<(DeathReason &inst, const std::string &str);

/// @brief Opioid Usage Behavior Classification
/// @details There are five possible possible usage classifications.
enum class Behavior {
    /// No history of opioid use
    kNever = 0,
    /// Former non-injection opioid use
    kFormerNoninjection = 1,
    /// Former injection opioid use
    kFormerInjection = 2,
    /// Non-injection opioid use
    kNoninjection = 3,
    /// Injection opioid use
    kInjection = 4,

    kCount = 5
};
std::ostream &operator<<(std::ostream &os, const Behavior &inst);
Behavior &operator<<(Behavior &inst, const std::string &str);

/// @brief Screening type that led to linkage
enum class LinkageType {
    /// Linked through background screening
    kNa = -1,
    kBackground = 0,
    /// Linked through intervention screening
    kIntervention = 1,
    kCount = 2
};
std::ostream &operator<<(std::ostream &os, const LinkageType &inst);
LinkageType &operator<<(LinkageType &inst, const std::string &str);

/// @brief Status of Linkage
enum class LinkageState {
    /// Person has never been linked to care
    kNever = 0,
    /// Person is currently linked to care
    kLinked = 1,
    /// Person was previously linked to care, but is not currently linked
    kUnlinked = 2,
    kCount = 3
};
std::ostream &operator<<(std::ostream &os, const LinkageState &inst);
LinkageState &operator<<(LinkageState &inst, const std::string &str);

/// @brief Classification of Liver Fibrosis Stage
/// @details HCV infection causes liver fibrosis and cirrhosis and increases
/// the risk of development of hepatocellular carcinoma (HCC).
/// These states strictly increase, with the possibility of progressing to
/// HCC being possible at any time from stage F3 and higher.
enum class FibrosisState {
    kF0 = 0,
    kF1 = 1,
    kF2 = 2,
    kF3 = 3,
    kF4 = 4,
    kDecomp = 5,
    kNone = 6,
    kCount = 7
};
std::ostream &operator<<(std::ostream &os, const FibrosisState &inst);
FibrosisState &operator<<(FibrosisState &inst, const std::string &str);
FibrosisState &operator++(FibrosisState &inst);
FibrosisState operator++(FibrosisState &inst, int);

/// @brief Classification of hepatocellular carcinoma (HCC) state
/// @details Advanced liver fibrosis and cirrhosis significantly increase
/// risk of development of HCC, a major complication associated with HCV
/// infection that has significant mortality and morbidity rates.
/// <a
/// href="https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5863002/">Reference</a>
enum class HCCState {
    /// No hepatocellular carcinoma
    kNone = 0,
    /// Early-stage hepatocellular carcinoma
    kEarly = 1,
    /// Late-stage hepatocellular carcinoma
    kLate = 2,
    kCount = 3
};
std::ostream &operator<<(std::ostream &os, const HCCState &inst);
HCCState &operator<<(HCCState &inst, const std::string &str);

/// @brief Clinically staged liver fibrosis stage
enum class MeasuredFibrosisState {
    /// Person is measured to be either F0 or F1
    kF01 = 0,
    /// Person is measured to be either F2 or F3
    kF23 = 1,
    /// Person is measured to be F4 fibrosis, compensated cirrhosis
    kF4 = 2,
    /// Person has decompensated liver cirrhosis
    kDecomp = 3,
    /// Person has never been staged before
    kNone = 4,

    kCount = 5
};
std::ostream &operator<<(std::ostream &os, const MeasuredFibrosisState &inst);
MeasuredFibrosisState &operator<<(MeasuredFibrosisState &inst,
                                  const std::string &str);
MeasuredFibrosisState &operator++(MeasuredFibrosisState &inst);
MeasuredFibrosisState operator++(MeasuredFibrosisState &inst, int);

/// @brief Opioid Use Disorder Treatment States (MOUDs)
enum class MOUD {
    /// Never in MOUD
    kNone = 0,
    /// Currently in MOUD
    kCurrent = 1,
    /// Recently dropped out of MOUD
    kPost = 2,
    kCount = 3
};
std::ostream &operator<<(std::ostream &os, const MOUD &inst);
MOUD &operator<<(MOUD &inst, const std::string &str);

/// @brief Biological Sex
enum class Sex {
    /// Assigned male at birth
    kMale = 0,
    /// Assigned female at birth
    kFemale = 1,
    kCount = 2
};
std::ostream &operator<<(std::ostream &os, const Sex &inst);
Sex &operator<<(Sex &inst, const std::string &str);

/// @brief Pregnancy Classification
/// @details There are three possible pregnancy states.
enum class PregnancyState {
    kNa = -1,
    /// Never pregnant
    kNone = 0,
    /// Actively pregnant
    kPregnant = 1,
    /// Post-pregnancy
    kPostpartum = 2,
    kCount = 3
};
std::ostream &operator<<(std::ostream &os, const PregnancyState &inst);
PregnancyState &operator<<(PregnancyState &inst, const std::string &str);

enum class UtilityType : int { kMin = 0, kMult = 1, kCount = 2 };

/// @brief Attributes describing an HCV Infection
struct HCVDetails {
    // Active statuses
    HCV hcv = HCV::kNone;
    FibrosisState fibrosis_state = FibrosisState::kNone;
    bool is_genotype_three = false;
    bool seropositive = false;
    // Time step of change tracking
    int time_changed = -1;
    int time_fibrosis_state_changed = -1;
    // Counters
    int times_infected = 0;
    int times_acute_cleared = 0;
    int svrs = 0;
};
std::ostream &operator<<(std::ostream &os, const HCVDetails &inst);

struct HIVDetails {
    HIV hiv = HIV::kNone;
    int time_changed = -1;
    int low_cd4_months_count = 0;
};
std::ostream &operator<<(std::ostream &os, const HIVDetails &inst);

struct HCCDetails {
    HCCState hcc_state = HCCState::kNone;
    bool hcc_diagnosed = false;
};
std::ostream &operator<<(std::ostream &os, const HCCDetails &inst);

/// @brief Attributes describing drug use behavior
struct BehaviorDetails {
    Behavior behavior = Behavior::kNever;
    int time_last_active = -1;
};
std::ostream &operator<<(std::ostream &os, BehaviorDetails const &behav);

/// @brief Attributes describing Linkage
struct LinkageDetails {
    LinkageState link_state = LinkageState::kNever;
    int time_link_change = -1;
    LinkageType link_type = LinkageType::kNa;
    int link_count = 0;
};
std::ostream &operator<<(std::ostream &os, LinkageDetails const &ldet);

/// @brief Attributes describing MOUD status
struct MOUDDetails {
    MOUD moud_state = MOUD::kNone;
    int time_started_moud = -1;
    int current_state_concurrent_months = 0;
    int total_moud_months = 0;
};
std::ostream &operator<<(std::ostream &os, MOUDDetails const &mdet);

/// @brief Attributes describing pregnancy
struct Child {
    HCV hcv = HCV::kNone;
    bool tested = false;
};
std::ostream &operator<<(std::ostream &os, Child const &inst);
struct PregnancyDetails {
    PregnancyState pregnancy_state = PregnancyState::kNa;
    int time_of_pregnancy_change = -1;
    int count = 0;
    int num_infants = 0;
    int num_miscarriages = 0;
    int num_hcv_exposures = 0;
    int num_hcv_infections = 0;
    int num_hcv_tests = 0;
    std::vector<Child> children = {};
};
std::ostream &operator<<(std::ostream &os, PregnancyDetails const &pdet);

/// @brief Person attributes describing clinically assessed liver stage
struct StagingDetails {
    MeasuredFibrosisState measured_fibrosis_state =
        MeasuredFibrosisState::kNone;
    bool had_second_test = false;
    int time_of_last_staging = -1;
};
std::ostream &operator<<(std::ostream &os, StagingDetails const &sdet);

/// @brief Attributes describing screening status
struct ScreeningDetails {
    // -1 if never screened, otherwise [0, currentTimestep-1)
    int time_of_last_screening = -1;
    int num_ab_tests = 0;
    int num_rna_tests = 0;
    bool ab_positive = false;
    bool identified = false;
    int time_identified = -1;
};
std::ostream &operator<<(std::ostream &os, ScreeningDetails const &sdet);

/// @brief Attributes describing treatment state
struct TreatmentDetails {
    bool initiated_treatment = false;
    int time_of_treatment_initiation = -1;
    int num_starts = 0;
    int num_withdrawals = 0;
    int num_toxic_reactions = 0;
    int num_completed = 0;
    int num_retreatments = 0;
    bool retreatment = false;
};
std::ostream &operator<<(std::ostream &os, TreatmentDetails const &tdet);

/// @brief Attributes describing a person's quality of life across the
/// course of their life
struct LifetimeUtility {
    double mult_util = 0.0;
    double min_util = 0.0;
    double discount_min_util = 0.0;
    double discount_mult_util = 0.0;
};
std::ostream &operator<<(std::ostream &os, LifetimeUtility const &lu);
bool operator==(LifetimeUtility const &lhs, LifetimeUtility const &rhs);

/// @brief Attributes for an associated cost and utility
struct CostUtil {
    double cost = 0.0;
    double util = 0.0;
};
std::ostream &operator<<(std::ostream &os, CostUtil const &lu);
bool operator==(CostUtil const &lhs, CostUtil const &rhs);
} // namespace data
} // namespace hepce

#endif // HEPCE_DATA_TYPES_HPP_