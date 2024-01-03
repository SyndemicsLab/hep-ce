#ifndef PERSON_CONTAINERS_HPP_
#define PERSON_CONTAINERS_HPP_

namespace Person {
    /// @brief HEP-C Infection States
    enum class HEPCState {
        /// No HCV infection
        NONE = 0,
        /// New HCV infection; sub-6 months infected
        ACUTE = 1,
        /// Long-term HCV infection
        CHRONIC = 2
    };

    /// @brief Opioid Usage Behavior Classification
    /// @details There are five possible possible usage classifications.
    enum class BehaviorClassification {
        /// No history of opioid use
        NEVER = 0,
        /// Former non-injection opioid use
        FORMER_NONINJECTION = 1,
        /// Former injection opioid use
        FORMER_INJECTION = 2,
        /// Non-injection opioid use
        NONINJECTION = 3,
        /// Injection opioid use
        INJECTION = 4
    };

    /// @brief Screening type that led to linkage
    enum class LinkageType {
        /// Linked through background screening
        BACKGROUND = 0,
        /// Linked through intervention screening
        INTERVENTION = 1
    };

    /// @brief Status of Linkage
    enum class LinkageState {
        /// Person has never been linked to care
        NEVER = 0,
        /// Person is currently linked to care
        LINKED = 1,
        /// Person was previously linked to care, but is not currently linked
        UNLINKED = 2
    };

    /// @brief Classification of Liver Disease Stage
    /// @details HCV infection causes liver fibrosis and cirrhosis and increases
    /// the risk of development of hepatocellular carcinoma (HCC).
    /// These states strictly increase, with the possibility of progressing to
    /// HCC being possible at any time from stage F3 and higher.
    enum class LiverState {
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
        /// Early-stage hepatocellular carcinoma
        EHCC = 7,
        /// Late-stage hepatocellular carcinoma
        LHCC = 8
    };

    /// @brief Clinically staged liver fibrosis stage
    enum class MeasuredLiverState {
        /// Person has never been screened before
        NONE = 0,
        /// Person is measured to be either F0 or F1
        F01 = 1,
        /// Person is measured to be either F2 or F3
        F23 = 2,
        /// Person is measured to be F4
        F4 = 3,
        /// Person has decompensated liver
        DECOMP = 4
    };

    /// @brief Opioid Use Disorder Treatment States (MOUDs)
    enum class MOUD {
        /// Never in MOUD
        NONE = 0,
        /// Currently in MOUD
        CURRENT = 1,
        /// Recently dropped out of MOUD
        POST = 2
    };

    /// @brief Biological Sex
    enum class Sex {
        /// Assigned male at birth
        MALE = 0,
        /// Assigned female at birth
        FEMALE = 1
    };

    /// @brief Pregnancy Classification
    /// @details There are three possible pregnancy states.
    enum class PregnancyState {
        /// Never pregnant
        NONE = 0,
        /// Actively pregnant
        PREGNANT = 1,
        /// Post-pregnancy
        POSTPARTUM = 2
    };

    /// @brief Attributes describing Identification
    struct IdentificationStatus {
        bool identifiedAsPositiveInfection = false;
        uint32_t timeIdentified = 0;
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
        uint32_t timeLastActive = 0;
    };

    /// @brief Attributes describing Linkage
    struct LinkageDetails {
        LinkageState linkState = LinkageState::NEVER;
        uint32_t timeOfLinkChange = 0;
        LinkageType linkType = LinkageType::BACKGROUND;
    };

    /// @brief Attributes describing MOUD status
    struct MOUDDetails {
        MOUD moudState = MOUD::NONE;
        uint32_t timeStartedMoud = 0;
    };

    /// @brief Attributes describing pregnancy
    struct PregnancyDetails {
        PregnancyState pregnancyState = PregnancyState::NONE;
        uint32_t timeOfPregnancyChange = 0;
        int infantCount = 0;
        int miscarriageCount = 0;
    };

    /// @brief Person attributes describing clinically assessed liver stage
    struct StagingDetails {
        MeasuredLiverState measuredLiverState = MeasuredLiverState::NONE;
        uint32_t timeOfLastStaging = 0;
    };

    struct ScreeningDetails {
        // -1 if never screened, otherwise [0, currentTimestep-1)
        uint32_t timeOfLastScreening = 0;
        int screeningFrequency = -1; // -1 if screened only once and never again
        bool interventionScreening = false;
    };

} // namespace Person

#endif