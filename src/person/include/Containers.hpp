#ifndef PERSON_CONTAINERS_HPP_
#define PERSON_CONTAINERS_HPP_

#include <ostream>

namespace Person {
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

    /// @brief Screening type that led to linkage
    enum class LinkageType {
        /// Linked through background screening
        BACKGROUND = 0,
        /// Linked through intervention screening
        INTERVENTION = 1,
        COUNT = 2
    };

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

    /// @brief Classification of Liver Fibrosis Stage
    /// @details HCV infection causes liver fibrosis and cirrhosis and increases
    /// the risk of development of hepatocellular carcinoma (HCC).
    /// These states strictly increase, with the possibility of progressing to
    /// HCC being possible at any time from stage F3 and higher.
    enum class FibrosisState {
        /// No adverse liver effects
        NONE = 0,
        /// No scarring
        F0 = 1,
        /// Mild liver scarring
        F1 = 2,
        /// Scarring has occurred and extends outside the liver area
        F2 = 3,
        /// Fibrosis spreading and forming bridges with other fibrotic liver
        /// areas
        F3 = 4,
        /// Cirrhosis or advanced scarring
        F4 = 5,
        /// Symptomatic cirrhosis; overt complications
        DECOMP = 6,
        COUNT = 7
    };

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

    /// @brief Clinically staged liver fibrosis stage
    enum class MeasuredFibrosisState {
        /// Person has never been screened before
        NONE = 0,
        /// Person is measured to be either F0 or F1
        F01 = 1,
        /// Person is measured to be either F2 or F3
        F23 = 2,
        /// Person is measured to be F4
        F4 = 3,
        /// Person has decompensated liver
        DECOMP = 4,
        COUNT = 5
    };

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

    /// @brief Biological Sex
    enum class Sex {
        /// Assigned male at birth
        MALE = 0,
        /// Assigned female at birth
        FEMALE = 1,
        COUNT = 2
    };

    /// @brief Pregnancy Classification
    /// @details There are three possible pregnancy states.
    enum class PregnancyState {
        /// Never pregnant
        NONE = 0,
        /// Actively pregnant
        PREGNANT = 1,
        /// Post-pregnancy
        POSTPARTUM = 2,
        COUNT = 3
    };

    /// @brief Attributes describing an Infection
    struct Health {
        HCV hcv = HCV::NONE;
        FibrosisState fibrosisState = FibrosisState::NONE;
        bool isGenotypeThree = false;
        bool seropositive = false;
        int timeHCVChanged = 0;
        int timeFibrosisStateChanged = 0;
        int timesInfected = 0;
        int timesCleared = 0;
        bool identifiedHCV = false;
        int timeIdentified = 0;
        HCCState hccState = HCCState::NONE;
    };
    std::ostream &operator<<(std::ostream &os, const Health &inst);

    /// @brief Attributes describing drug use behavior
    struct BehaviorDetails {
        Behavior behavior = Behavior::NEVER;
        int timeLastActive = 0;
    };
    std::ostream &operator<<(std::ostream &os, BehaviorDetails const &behav);

    /// @brief Attributes describing Linkage
    struct LinkageDetails {
        LinkageState linkState = LinkageState::NEVER;
        int timeOfLinkChange = 0;
        LinkageType linkType = LinkageType::BACKGROUND;
        int linkCount = 0;
    };
    std::ostream &operator<<(std::ostream &os, LinkageDetails const &ldet);

    /// @brief Attributes describing MOUD status
    struct MOUDDetails {
        MOUD moudState = MOUD::NONE;
        int timeStartedMoud = 0;
    };
    std::ostream &operator<<(std::ostream &os, MOUDDetails const &mdet);

    /// @brief Attributes describing pregnancy
    struct PregnancyDetails {
        PregnancyState pregnancyState = PregnancyState::NONE;
        int timeOfPregnancyChange = 0;
        int numInfants = 0;
        int numMiscarriages = 0;
    };
    std::ostream &operator<<(std::ostream &os, PregnancyDetails const &pdet);

    /// @brief Person attributes describing clinically assessed liver stage
    struct StagingDetails {
        MeasuredFibrosisState measuredFibrosisState =
            MeasuredFibrosisState::NONE;
        bool hadSecondTest = false;
        int timeOfLastStaging = 0;
    };
    std::ostream &operator<<(std::ostream &os, StagingDetails const &sdet);

    /// @brief Attributes describing screening status
    struct ScreeningDetails {
        // -1 if never screened, otherwise [0, currentTimestep-1)
        int timeOfLastScreening = 0;
        int numABTests = 0;
        int numRNATests = 0;
    };
    std::ostream &operator<<(std::ostream &os, ScreeningDetails const &sdet);

    /// @brief Attributes describing treatment state
    struct TreatmentDetails {
        bool initiatedTreatment = false;
        int timeOfTreatmentInitiation = 0;
    };
    std::ostream &operator<<(std::ostream &os, TreatmentDetails const &tdet);

    /// @brief Attributes describing a person's quality of life
    struct UtilityTracker {
        double multUtil = 1.0;
        double minUtil = 1.0;
    };
    std::ostream &operator<<(std::ostream &os, UtilityTracker const &utra);
} // namespace Person

#endif
