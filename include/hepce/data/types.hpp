////////////////////////////////////////////////////////////////////////////////
// File: containers.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_DATA_TYPES_HPP_
#define HEPCE_DATA_TYPES_HPP_

#include <ostream>

namespace hepce {
namespace data {
inline static const std::string POPULATION_HEADERS =
    "sex,age,isAlive,deathReason,identifiedHCV,timeInfectionIdentified,HCV,"
    "fibrosisState,isGenotypeThree,seropositive,timeHCVChanged,"
    "timeFibrosisStateChanged,drugBehavior,timeLastActiveDrugUse,linkageState,"
    "timeOfLinkChange,linkageType,linkCount,measuredFibrosisState,"
    "timeOfLastStaging,timeOfLastScreening,numABTests,numRNATests,"
    "timesInfected,timesAcuteCleared,initiatedTreatment,"
    "timeOfTreatmentInitiation,minUtility,multUtility,discountMinUtility,"
    "discountMultUtility,treatmentWithdrawals,treatmentToxicReactions,"
    "completedTreatments,svrs,behaviorUtility,liverUtility,treatmentUtility,"
    "backgroundUtility,hivUtility,lifeSpan,discountedLifeSpan,"
    "numberOfTreatmentStarts,numberOfRetreatments";

/// @brief Infection types tracked for all Persons
enum class InfectionType {
    /// Hepatitis C Virus
    HCV = 0,
    /// Human Immunodeficiency Virus
    HIV = 1,
    COUNT = 2
};
std::ostream &operator<<(std::ostream &os, const InfectionType &inst);
InfectionType &operator<<(InfectionType &inst, const std::string &str);
InfectionType &operator++(InfectionType &inst);

/// @brief HEP-C Infection States
enum class HCV {
    /// No HCV infection
    NONE = 0,
    /// New HCV infection; sub-6 months infected
    ACUTE = 1,
    /// Long-term HCV infection
    CHRONIC = 2,

    COUNT = 3
};
std::ostream &operator<<(std::ostream &os, const HCV &inst);
HCV &operator<<(HCV &inst, const std::string &str);

/// @brief HIV Infection States
enum class HIV {
    /// No HIV infection
    NONE = 0,
    /// High CD4 count, unsuppressed
    HIUN = 1,
    /// High CD4 count, suppressed
    HISU = 2,
    /// Low CD4 count, unsuppressed
    LOUN = 3,
    /// Low CD4 count, suppressed
    LOSU = 4,

    COUNT = 5
};
std::ostream &operator<<(std::ostream &os, const HIV &inst);
HIV &operator<<(HIV &inst, const std::string &str);

enum class DeathReason {
    /// @brief Not applicable, usually not dead
    NA = 0,
    BACKGROUND = 1,
    LIVER = 2,
    INFECTION = 3,
    AGE = 4,
    OVERDOSE = 5,
    HIV = 6,
    COUNT = 7
};
std::ostream &operator<<(std::ostream &os, const DeathReason &inst);
DeathReason &operator<<(DeathReason &inst, const std::string &str);

/// @brief Opioid Usage Behavior Classification
/// @details There are five possible possible usage classifications.
enum class Behavior {
    /// No history of opioid use
    NEVER = 0,
    /// Former non-injection opioid use
    FORMER_NONINJECTION = 1,
    /// Former injection opioid use
    FORMER_INJECTION = 2,
    /// Non-injection opioid use
    NONINJECTION = 3,
    /// Injection opioid use
    INJECTION = 4,

    COUNT = 5
};
std::ostream &operator<<(std::ostream &os, const Behavior &inst);
Behavior &operator<<(Behavior &inst, const std::string &str);

/// @brief Screening type that led to linkage
enum class LinkageType {
    /// Linked through background screening
    NA = -1,
    BACKGROUND = 0,
    /// Linked through intervention screening
    INTERVENTION = 1,
    COUNT = 2
};
std::ostream &operator<<(std::ostream &os, const LinkageType &inst);
LinkageType &operator<<(LinkageType &inst, const std::string &str);

/// @brief Status of Linkage
enum class LinkageState {
    /// Person has never been linked to care
    NEVER = 0,
    /// Person is currently linked to care
    LINKED = 1,
    /// Person was previously linked to care, but is not currently linked
    UNLINKED = 2,
    COUNT = 3
};
std::ostream &operator<<(std::ostream &os, const LinkageState &inst);
LinkageState &operator<<(LinkageState &inst, const std::string &str);

/// @brief Classification of Liver Fibrosis Stage
/// @details HCV infection causes liver fibrosis and cirrhosis and increases
/// the risk of development of hepatocellular carcinoma (HCC).
/// These states strictly increase, with the possibility of progressing to
/// HCC being possible at any time from stage F3 and higher.
enum class FibrosisState {
    /// No scarring
    F0 = 0,
    /// Mild liver scarring
    F1 = 1,
    /// Scarring has occurred and extends outside the liver area
    F2 = 2,
    /// Fibrosis spreading and forming bridges with other fibrotic liver
    /// areas
    F3 = 3,
    /// Cirrhosis or advanced scarring
    F4 = 4,
    /// Symptomatic cirrhosis; overt complications
    DECOMP = 5,
    /// No adverse liver effects
    NONE = 6,
    COUNT = 7
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
    NONE = 0,
    /// Early-stage hepatocellular carcinoma
    EARLY = 1,
    /// Late-stage hepatocellular carcinoma
    LATE = 2,
    COUNT = 3
};
std::ostream &operator<<(std::ostream &os, const HCCState &inst);
HCCState &operator<<(HCCState &inst, const std::string &str);

/// @brief Clinically staged liver fibrosis stage
enum class MeasuredFibrosisState {
    /// Person is measured to be either F0 or F1
    F01 = 0,
    /// Person is measured to be either F2 or F3
    F23 = 1,
    /// Person is measured to be F4 fibrosis, compensated cirrhosis
    F4 = 2,
    /// Person has decompensated liver cirrhosis
    DECOMP = 3,
    /// Person has never been staged before
    NONE = 4,

    COUNT = 5
};
std::ostream &operator<<(std::ostream &os, const MeasuredFibrosisState &inst);
MeasuredFibrosisState &operator<<(MeasuredFibrosisState &inst,
                                  const std::string &str);
MeasuredFibrosisState &operator++(MeasuredFibrosisState &inst);
MeasuredFibrosisState operator++(MeasuredFibrosisState &inst, int);

/// @brief Opioid Use Disorder Treatment States (MOUDs)
enum class MOUD {
    /// Never in MOUD
    NONE = 0,
    /// Currently in MOUD
    CURRENT = 1,
    /// Recently dropped out of MOUD
    POST = 2,
    COUNT = 3
};
std::ostream &operator<<(std::ostream &os, const MOUD &inst);
MOUD &operator<<(MOUD &inst, const std::string &str);

/// @brief Biological Sex
enum class Sex {
    /// Assigned male at birth
    MALE = 0,
    /// Assigned female at birth
    FEMALE = 1,
    COUNT = 2
};
std::ostream &operator<<(std::ostream &os, const Sex &inst);
Sex &operator<<(Sex &inst, const std::string &str);

/// @brief Pregnancy Classification
/// @details There are three possible pregnancy states.
enum class PregnancyState {
    NA = -1,
    /// Never pregnant
    NONE = 0,
    /// Actively pregnant
    PREGNANT = 1,
    /// Post-pregnancy
    POSTPARTUM = 2,
    COUNT = 3
};
std::ostream &operator<<(std::ostream &os, const PregnancyState &inst);
PregnancyState &operator<<(PregnancyState &inst, const std::string &str);

/// @brief Attributes describing an HCV Infection
struct HCVDetails {
    // Active statuses
    HCV hcv = HCV::NONE;
    FibrosisState fibrosisState = FibrosisState::NONE;
    bool isGenotypeThree = false;
    bool seropositive = false;
    // Time step of change tracking
    int timeChanged = -1;
    int timeFibrosisStateChanged = -1;
    // Counters
    int timesInfected = 0;
    int timesAcuteCleared = 0;
};
std::ostream &operator<<(std::ostream &os, const HCVDetails &inst);

struct HIVDetails {
    // Active statuses
    HIV hiv = HIV::NONE;
    // Time step of change tracking
    int timeChanged = -1;
};
std::ostream &operator<<(std::ostream &os, const HIVDetails &inst);

struct HCCDetails {
    HCCState hccState = HCCState::NONE;
};
std::ostream &operator<<(std::ostream &os, const HCCDetails &inst);

/// @brief Attributes describing drug use behavior
struct BehaviorDetails {
    Behavior behavior = Behavior::NEVER;
    int timeLastActive = -1;
};
std::ostream &operator<<(std::ostream &os, BehaviorDetails const &behav);

/// @brief Attributes describing Linkage
struct LinkageDetails {
    LinkageState linkState = LinkageState::NEVER;
    int timeOfLinkChange = -1;
    LinkageType linkType = LinkageType::NA;
    int linkCount = 0;
};
std::ostream &operator<<(std::ostream &os, LinkageDetails const &ldet);

/// @brief Attributes describing MOUD status
struct MOUDDetails {
    MOUD moudState = MOUD::NONE;
    int timeStartedMoud = -1;
    int currentStateConcurrentMonths = 0;
    int totalMOUDMonths = 0;
};
std::ostream &operator<<(std::ostream &os, MOUDDetails const &mdet);

/// @brief Attributes describing pregnancy
struct PregnancyDetails {
    PregnancyState pregnancyState = PregnancyState::NA;
    int timeOfPregnancyChange = -1;
    int numInfants = 0;
    int numMiscarriages = 0;
};
std::ostream &operator<<(std::ostream &os, PregnancyDetails const &pdet);

struct Child {
    HCV hcv = HCV::NONE;
    bool tested = false;
};
std::ostream &operator<<(std::ostream &os, Child const &inst);

/// @brief Person attributes describing clinically assessed liver stage
struct StagingDetails {
    MeasuredFibrosisState measuredFibrosisState = MeasuredFibrosisState::NONE;
    bool hadSecondTest = false;
    int timeOfLastStaging = -1;
};
std::ostream &operator<<(std::ostream &os, StagingDetails const &sdet);

/// @brief Attributes describing screening status
struct ScreeningDetails {
    // -1 if never screened, otherwise [0, currentTimestep-1)
    int timeOfLastScreening = -1;
    int numABTests = 0;
    int numRNATests = 0;
    bool antibodyPositive = false;
    bool identified = false;
    int timeIdentified = -1;
};
std::ostream &operator<<(std::ostream &os, ScreeningDetails const &sdet);

/// @brief Attributes describing treatment state
struct TreatmentDetails {
    bool initiatedTreatment = false;
    int timeOfTreatmentInitiation = 0;
    bool retreatment = false;
    int numberOfTreatmentStarts = 0;
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