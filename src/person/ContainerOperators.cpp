#include "Maps.hpp"

namespace Person {

    std::ostream &operator<<(std::ostream &os,
                             IdentificationStatus const &idst) {
        os << "Identified as a Positive Infection: "
           << idst.identifiedAsPositiveInfection << std::endl;
        os << "Time Identified: " << idst.timeIdentified << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, InfectionStatus const &inst) {
        os << "HCV State: " << Person::hepcStateEnumToStringMap[inst.hepcState]
           << std::endl;
        os << "Fibrosis State: "
           << Person::fibrosisStateEnumToStringMap[inst.fibrosisState]
           << std::endl;
        os << "Genotype 3: " << inst.isGenotypeThree << std::endl;
        os << "Seropositivity: " << inst.seropositivity << std::endl;
        os << "Last Time the HCV State Changed: " << inst.timeHEPCStateChanged
           << std::endl;
        os << "Last Time the Fibrosis State Changed: "
           << inst.timeFibrosisStateChanged << std::endl;
        os << "Number of Times Infected: " << inst.numInfections << std::endl;
        os << "Number of Times Cleared: " << inst.numClearances << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, HCCStatus const &stat) {
        // os << "HCC Status: " << stat.hccState << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, BehaviorDetails const &behav) {
        os << "Behavior Type: "
           << Person::behaviorClassificationEnumToStringMap
                  [behav.behaviorClassification]
           << std::endl;
        os << "Last Timestep Active: " << behav.timeLastActive << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, LinkageDetails const &ldet) {
        os << "Linkage State: "
           << Person::linkageStateEnumToStringMap[ldet.linkState] << std::endl;
        os << "Last Time Linkage State Changed: " << ldet.timeOfLinkChange
           << std::endl;
        os << "Linkage Type: "
           << Person::linkageTypeEnumToStringMap[ldet.linkType] << std::endl;
        os << "Times Linked: " << ldet.linkCount << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, MOUDDetails const &mdet) {
        os << "MOUD State: " << Person::moudEnumToStringMap[mdet.moudState]
           << std::endl;
        os << "Time Started MOUD: " << mdet.timeStartedMoud << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, PregnancyDetails const &pdet) {
        os << "Pregnancy State: "
           << Person::pregnancyStateEnumToStringMap[pdet.pregnancyState]
           << std::endl;
        os << "Time of Pregnancy State Change: " << pdet.timeOfPregnancyChange
           << std::endl;
        os << "Infant Count: " << pdet.infantCount << std::endl;
        os << "Miscarriage Count: " << pdet.miscarriageCount << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, StagingDetails const &sdet) {
        os << "Fibrosis State: "
           << Person::measuredFibrosisStateEnumToStringMap
                  [sdet.measuredFibrosisState]
           << std::endl;
        os << "Has a Second Fibrosis Test: " << sdet.hadFibTestTwo << std::endl;
        os << "Time of Last Staging: " << sdet.timeOfLastStaging << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, ScreeningDetails const &sdet) {
        os << "Time of Last Screening: " << sdet.timeOfLastScreening
           << std::endl;
        os << "AB Test Counts: " << sdet.abCount << std::endl;
        os << "RNA Testing Counts: " << sdet.rnaCount << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, TreatmentDetails const &tdet) {
        os << "Has the person Initiated Treatment: " << tdet.initiatedTreatment
           << std::endl;
        os << "Time Person Initiated Treatment: "
           << tdet.timeOfTreatmentInitiation << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, UtilityTracker const &utra) {
        os << "Multiplier Utility: " << utra.multUtil << std::endl;
        os << "Minimum Utility: " << utra.minUtil << std::endl;
        return os;
    }
} // namespace Person