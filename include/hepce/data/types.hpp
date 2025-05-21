////////////////////////////////////////////////////////////////////////////////
// File: types.hpp                                                            //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_DATA_TYPES_HPP_
#define HEPCE_DATA_TYPES_HPP_

#include <ostream>
#include <sstream>
#include <string>
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
    headers << "hcv_link_state,time_of_hcv_link_change,hcv_link_count,";
    if (hiv) {
        headers << "hiv_link_state,time_of_hiv_link_change,hiv_link_count,";
    } else {
        headers << "never,-1,0,";
    }
    // ScreeningDetails
    headers << "time_of_last_hcv_screening,num_hcv_ab_tests,num_hcv_rna_tests,"
               "hcv_antibody_positive,hcv_identified,time_hcv_identified,num_"
               "hcv_identifications,hcv_screening_type,";
    if (hiv) {
        headers
            << "time_of_last_hiv_screening,num_hiv_ab_tests,num_hiv_rna_tests,"
               "hiv_antibody_positive,hiv_identified,time_hiv_identified,num_"
               "hiv_identified,hiv_screening_type,";
    } else {
        headers << "-1,0,0,false,false,-1,0,na";
    }
    // TreatmentDetails
    headers << "initiated_hcv_treatment,time_of_hcv_treatment_initiation,num_"
               "hcv_treatment_starts,num_hcv_treatment_withdrawals,num_hcv_"
               "treatment_toxic_reactions,num_completed_hcv_treatments,num_hcv_"
               "salvages,in_hcv_salvage_treatment,";
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
    kHcv = 0,  /// Hepatitis C Virus
    kHiv = 1,  /// Human Immunodeficiency Virus
    kCount = 2 /// Count of InfectionType Enum
};
std::ostream &operator<<(std::ostream &os, const InfectionType &inst);
InfectionType &operator<<(InfectionType &inst, const std::string &str);
InfectionType &operator++(InfectionType &inst);

/// @brief HEP-C Infection States
enum class HCV : int {
    kNone = 0,    /// No HCV infection
    kAcute = 1,   /// New HCV infection; sub-6 months infected
    kChronic = 2, /// Long-term HCV infection
    kCount = 3    /// Count of HCV Enum
};
std::ostream &operator<<(std::ostream &os, const HCV &inst);
HCV &operator<<(HCV &inst, const std::string &str);

/// @brief HIV Infection States
enum class HIV : int {
    kNone = 0, /// No HIV infection
    kHiUn = 1, /// High CD4 count, unsuppressed
    kHiSu = 2, /// High CD4 count, suppressed
    kLoUn = 3, /// Low CD4 count, unsuppressed
    kLoSu = 4, /// Low CD4 count, suppressed
    kCount = 5 /// Count of HIV Enum
};
std::ostream &operator<<(std::ostream &os, const HIV &inst);
HIV &operator<<(HIV &inst, const std::string &str);

/// @brief Reason a Person Dies
enum class DeathReason : int {
    kNa = -1,        /// Not Dead
    kBackground = 0, /// Died from Background Mortality Causes
    kLiver = 1,      /// Died from Liver Related Diseases
    kInfection = 2,  /// Died from Infection Related Diseases
    kAge = 3,        /// Died from Age
    kOverdose = 4,   /// Died of Overdose
    kHiv = 5,        /// Died of HIV Infection Effects
    kCount = 6       /// Count of DeathReason Enum
};
std::ostream &operator<<(std::ostream &os, const DeathReason &inst);
DeathReason &operator<<(DeathReason &inst, const std::string &str);

/// @brief Opioid Usage Behavior Classification
/// @details There are five possible possible usage classifications.
enum class Behavior {
    kNever = 0,              /// No history of opioid use
    kFormerNoninjection = 1, /// Former non-injection opioid use
    kFormerInjection = 2,    /// Former injection opioid use
    kNoninjection = 3,       /// Non-injection opioid use
    kInjection = 4,          /// Injection opioid use
    kCount = 5               /// Count of Behavior Enum
};
std::ostream &operator<<(std::ostream &os, const Behavior &inst);
Behavior &operator<<(Behavior &inst, const std::string &str);

/// @brief Screening type that led to linkage
enum class ScreeningType {
    kNa = -1,          /// No Fibrosis Screening Applicable
    kBackground = 0,   /// Background Fibrosis Screening
    kIntervention = 1, /// Intervention Fibrosis Screening
    kCount = 2         /// Count of ScreeningType Enum
};
std::ostream &operator<<(std::ostream &os, const ScreeningType &inst);
ScreeningType &operator<<(ScreeningType &inst, const std::string &str);

/// @brief Screening type that led to linkage
enum class ScreeningTest : int {
    kNa = -1,  /// No Screening Test Applicable
    kAb = 0,   /// Antibody Screening Test
    kRna = 1,  /// RNA Screening Test
    kCount = 2 /// Count of ScreeningTest Enum
};
std::ostream &operator<<(std::ostream &os, const ScreeningTest &inst);
ScreeningTest &operator<<(ScreeningTest &inst, const std::string &str);

/// @brief Status of Linkage
enum class LinkageState {
    kNever = 0,    /// Person has never been linked to care
    kLinked = 1,   /// Person is currently linked to care
    kUnlinked = 2, /// Person was previously linked to care, but not currently
    kCount = 3     /// Count of LinkageState Enum
};
std::ostream &operator<<(std::ostream &os, const LinkageState &inst);
LinkageState &operator<<(LinkageState &inst, const std::string &str);

/// @brief Classification of Liver Fibrosis Stage
/// @details HCV infection causes liver fibrosis and cirrhosis and increases
/// the risk of development of hepatocellular carcinoma (HCC).
/// These states strictly increase, with the possibility of progressing to
/// HCC being possible at any time from stage F3 and higher.
enum class FibrosisState {
    kF0 = 0, /// No Scarring
    kF1 = 1, /// Mild Liver Scarring
    kF2 = 2, /// Scarring has occurred and extends outside the liver area
    kF3 =
        3, /// Fibrosis spreading and forming bridges with other fibrotic liver areas
    kF4 = 4,     /// Cirrhosis or advanced scarring
    kDecomp = 5, /// Symptomatic cirrhosis; over complications
    kNone = 6,   /// No adverse liver effects
    kCount = 7   /// Count of FibrosisState Enum
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
    kNone = 0,  /// No hepatocellular carcinoma
    kEarly = 1, /// Early-stage hepatocellular carcinoma
    kLate = 2,  /// Late-stage hepatocellular carcinoma
    kCount = 3  /// Count of HCCState Enum
};
std::ostream &operator<<(std::ostream &os, const HCCState &inst);
HCCState &operator<<(HCCState &inst, const std::string &str);

/// @brief Clinically staged liver fibrosis stage
enum class MeasuredFibrosisState {
    kF01 = 0,    /// Person is measured to be either F0 or F1
    kF23 = 1,    /// Person is measured to be either F2 or F3
    kF4 = 2,     /// Person is measured to be F4 fibrosis, compensated cirrhosis
    kDecomp = 3, /// Person has decompensated liver cirrhosis
    kNone = 4,   /// Person has never been staged before
    kCount = 5   /// Count of MeasuredFibrosisState Enum
};
std::ostream &operator<<(std::ostream &os, const MeasuredFibrosisState &inst);
MeasuredFibrosisState &operator<<(MeasuredFibrosisState &inst,
                                  const std::string &str);
MeasuredFibrosisState &operator++(MeasuredFibrosisState &inst);
MeasuredFibrosisState operator++(MeasuredFibrosisState &inst, int);

/// @brief Opioid Use Disorder Treatment States (MOUDs)
enum class MOUD {
    kNone = 0,    /// Never in MOUD
    kCurrent = 1, /// Currently in MOUD
    kPost = 2,    /// Recently dropped out of MOUD
    kCount = 3    /// Count of MOUD Enum
};
std::ostream &operator<<(std::ostream &os, const MOUD &inst);
MOUD &operator<<(MOUD &inst, const std::string &str);

/// @brief Biological Sex
enum class Sex {
    kMale = 0,   /// Assigned male at birth
    kFemale = 1, /// Assigned female at birth
    kCount = 2   /// Count of Sex Enum
};
std::ostream &operator<<(std::ostream &os, const Sex &inst);
Sex &operator<<(Sex &inst, const std::string &str);

/// @brief Pregnancy Classification
/// @details There are three possible pregnancy states.
enum class PregnancyState {
    kNa = -1,                  /// Pregnancy is not active
    kNone = 0,                 /// Never pregnant
    kPregnant = 1,             /// Actively pregnant
    kRestrictedPostpartum = 2, /// Unable to get Pregnant Period
    kYearOnePostpartum = 3,    /// Remaining Year 1 Postpartum
    kYearTwoPostpartum = 4,    /// Year 2 Postpartum
    kCount = 5                 /// Count of PregnancyState Enum
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
    int times_identified = 0;
    ScreeningType screen_type = ScreeningType::kNa;
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
    int num_salvages = 0;
    bool in_salvage_treatment = false;
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

struct PersonSelect {
    // basic characteristics
    Sex sex = Sex::kMale;
    int age = 0;
    bool is_alive = true;
    bool boomer_classification = false;
    DeathReason death_reason = DeathReason::kNa;
    // BehaviorDetails
    Behavior drug_behavior = Behavior::kNever;
    int time_last_active_drug_use = -1;
    // HCVDetails
    HCV hcv = HCV::kNone;
    FibrosisState fibrosis_state = FibrosisState::kNone;
    bool is_genotype_three = false;
    bool seropositive = false;
    int time_hcv_changed = -1;
    int time_fibrosis_state_changed = -1;
    int times_hcv_infected = 0;
    int times_acute_cleared = 0;
    int svrs = 0;
    // HIVDetails
    HIV hiv = HIV::kNone;
    int time_hiv_changed = -1;
    int low_cd4_months_count = 0;
    // HCCDetails
    HCCState hcc_state = HCCState::kNone;
    bool hcc_diagnosed = false;
    // Overdoses
    int num_overdoses = 0;
    bool currently_overdosing = false;
    // MOUDDetails
    MOUD moud_state = MOUD::kNone;
    int time_started_moud = -1;
    int current_moud_concurrent_months = 0;
    int total_moud_months = 0;
    // PregnancyDetails - NOTE: No instantiation of the children born prior to
    // this simulation run in the `children' vector.
    PregnancyState pregnancy_state = PregnancyState::kNa;
    int time_of_pregnancy_change = -1;
    int pregnancy_count = 0;
    int num_infants = 0;
    int num_miscarriages = 0;
    int num_infant_hcv_exposures = 0;
    int num_infant_hcv_infections = 0;
    int num_infant_hcv_tests = 0;
    // StagingDetails
    MeasuredFibrosisState measured_fibrosis_state =
        MeasuredFibrosisState::kNone;
    bool had_second_test = false;
    int time_of_last_staging = -1;
    // LinkageDetails
    // HCV
    LinkageState hcv_link_state = LinkageState::kNever;
    int time_of_hcv_link_change = -1;
    ScreeningType hcv_link_type = ScreeningType::kNa;
    int hcv_link_count = 0;
    // HIV
    LinkageState hiv_link_state = LinkageState::kNever;
    int time_of_hiv_link_change = -1;
    ScreeningType hiv_link_type = ScreeningType::kNa;
    int hiv_link_count = 0;
    // ScreeningDetails
    // HCV
    int time_of_last_hcv_screening = -1;
    int num_hcv_ab_tests = 0;
    int num_hcv_rna_tests = 0;
    bool hcv_antibody_positive = false;
    int hcv_identified = false;
    int time_hcv_identified = -1;
    int times_hcv_identified = 0;
    // HIV
    int time_of_last_hiv_screening = -1;
    int num_hiv_ab_tests = 0;
    int num_hiv_rna_tests = 0;
    bool hiv_antibody_positive = false;
    int hiv_identified = false;
    int time_hiv_identified = -1;
    int times_hiv_identified = 0;
    // TreatmentDetails
    // HCV
    bool initiated_hcv_treatment = false;
    int time_of_hcv_treatment_initiation = -1;
    int num_hcv_treatment_starts = 0;
    int num_hcv_treatment_withdrawals = 0;
    int num_hcv_treatment_toxic_reactions = 0;
    int num_completed_hcv_treatments = 0;
    int num_hcv_salvages = 0;
    bool in_hcv_salvage = false;
    // HIV - NOTE: No salvage analogue for HIV, so values stay default.
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
} // namespace data
} // namespace hepce

#endif // HEPCE_DATA_TYPES_HPP_