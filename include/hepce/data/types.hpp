////////////////////////////////////////////////////////////////////////////////
// File: containers.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_DATA_TYPES_HPP_
#define HEPCE_DATA_TYPES_HPP_

#include <ostream>
#include <vector>

namespace hepce {
namespace data {
inline static const std::string POPULATION_HEADERS =
    "sex,age,isAlive,deathReason,identifiedHCV,timeInfectionIdentified,HCV,"
    "fibrosis_state,is_genotype_three,seropositive,timeHCVChanged,"
    "time_fibrosis_state_changed,drugBehavior,timeLastActiveDrugUse,"
    "linkageState,"
    "time_link_change,linkageType,link_count,measured_fibrosis_state,"
    "time_of_last_staging,time_of_last_screening,number_ab_tests,number_rna_"
    "tests,"
    "times_infected,times_acute_cleared,initiated_treatment,"
    "time_treatment_initiation,minUtility,multUtility,discountMinUtility,"
    "discountMultUtility,treatmentWithdrawals,treatmentToxicReactions,"
    "completedTreatments,svrs,behaviorUtility,liverUtility,treatmentUtility,"
    "backgroundUtility,hivUtility,lifeSpan,discountedLifeSpan,"
    "number_treatment_starts,numberOfRetreatments";

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
    int number_ab_tests = 0;
    int number_rna_tests = 0;
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