#ifndef PERSON_CONTAINERS_HPP_
#define PERSON_CONTAINERS_HPP_

namespace Person {
    /// @brief HEP-C Infection States
    enum class HEPCState {
        /// No HCV infection
        NONE,
        /// New HCV infection; sub-6 months infected
        ACUTE,
        /// Long-term HCV infection
        CHRONIC
    };

    /// @brief Opioid Usage Behavior Classification
    /// @details There are five possible possible usage classifications.
    enum class BehaviorClassification {
        /// No history of opioid use
        NEVER,
        /// Former non-injection opioid use
        FORMER_NONINJECTION,
        /// Former injection opioid use
        FORMER_INJECTION,
        /// Non-injection opioid use
        NONINJECTION,
        /// Injection opioid use
        INJECTION
    };

    /// @brief Screening type that led to linkage
    enum class LinkageType {
        /// Linked through background screening
        BACKGROUND,
        /// Linked through intervention screening
        INTERVENTION
    };

    /// @brief Status of Linkage
    enum class LinkageState {
        /// Person has never been linked to care
        NEVER,
        /// Person is currently linked to care
        LINKED,
        /// Person was previously linked to care, but is not currently linked
        UNLINKED
    };

    /// @brief Classification of Liver Disease Stage
    /// @details HCV infection causes liver fibrosis and cirrhosis and increases
    /// the risk of development of hepatocellular carcinoma (HCC).
    /// These states strictly increase, with the possibility of progressing to
    /// HCC being possible at any time from stage F3 and higher.
    enum class LiverState {
        /// No adverse liver effects
        NONE,
        /// No scarring
        F0,
        /// Mild liver scarring
        F1,
        /// Scarring has occurred and extends outside the liver area
        F2,
        /// Fibrosis spreading and forming bridges with other fibrotic liver
        /// areas
        F3,
        /// Cirrhosis or advanced scarring
        F4,
        /// Symptomatic cirrhosis; overt complications
        DECOMP,
        /// Early-stage hepatocellular carcinoma
        EHCC,
        /// Late-stage hepatocellular carcinoma
        LHCC
    };

    /// @brief Clinically staged liver fibrosis stage
    enum class MeasuredLiverState {
        /// Person has never been screened before
        NONE,
        /// Person is measured to be either F0 or F1
        F01,
        /// Person is measured to be either F2 or F3
        F23,
        /// Person is measured to be F4
        F4,
        /// Person has decompensated liver
        DECOMP
    };

    /// @brief Opioid Use Disorder Treatment States (MOUDs)
    enum class MOUD {
        /// Never in MOUD
        NONE,
        /// Currently in MOUD
        CURRENT,
        /// Recently dropped out of MOUD
        POST
    };

    /// @brief Biological Sex
    enum class Sex {
        /// Assigned male at birth
        MALE,
        /// Assigned female at birth
        FEMALE
    };

    /// @brief Pregnancy Classification
    /// @details There are three possible pregnancy states.
    enum class PregnancyState {
        /// Never pregnant
        NONE,
        /// Actively pregnant
        PREGNANT,
        /// Post-pregnancy
        POSTPARTUM
    };

    /// @brief Attributes describing Identification
    struct IdentificationStatus {
        bool identifiedAsPositiveInfection = false;
        int timeIdentified = -1;
    };

    /// @brief Attributes describing an Infection
    struct InfectionStatus {
        HEPCState hepcState = HEPCState::NONE;
        LiverState liverState = LiverState::NONE;
        bool seropositivity = false;
        int timeHEPCStateChanged = 0;
        int timeLiverStateChanged = 0;
    };

    /// @brief Attributes describing drug use behavior
    struct BehaviorDetails {
        BehaviorClassification behaviorClassification =
            BehaviorClassification::NEVER;
        int timeLastActive = -1;
    };

    /// @brief Attributes describing Linkage
    struct LinkageDetails {
        LinkageState linkState = LinkageState::NEVER;
        int timeOfLinkChange = -1;
        LinkageType linkType = LinkageType::BACKGROUND;
    };

    /// @brief Attributes describing MOUD status
    struct MOUDDetails {
        MOUD moudState = MOUD::NONE;
        int timeStartedMoud = -1;
    };

    /// @brief Attributes describing pregnancy
    struct PregnancyDetails {
        PregnancyState pregnancyState = PregnancyState::NONE;
        int timeOfPregnancyChange = -1;
        int infantCount = 0;
        int miscarriageCount = 0;
    };

    /// @brief Person attributes describing clinically assessed liver stage
    struct StagingDetails {
        MeasuredLiverState measuredLiverState = MeasuredLiverState::NONE;
        int timeOfLastStaging = -1;
    };

    struct ScreeningDetails {
        // -1 if never screened, otherwise [0, currentTimestep-1)
        int timeOfLastScreening = -1;
        int screeningFrequency = -1; // -1 if screened only once and never again
        bool interventionScreening = false;
    };

} // namespace Person

#endif