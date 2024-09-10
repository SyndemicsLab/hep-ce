#include "Maps.hpp"

namespace Person {

    std::ostream &operator<<(std::ostream &os, Health const &inst) {
        os << "HCV State: " << person::hcvEnumToStringMap[inst.hcv]
           << std::endl;
        os << "Fibrosis State: "
           << person::fibrosisStateEnumToStringMap[inst.fibrosisState]
           << std::endl;
        os << "Genotype 3: " << inst.isGenotypeThree << std::endl;
        os << "Seropositive: " << inst.seropositive << std::endl;
        os << "Last Time the HCV State Changed: " << inst.timeHCVChanged
           << std::endl;
        os << "Last Time the Fibrosis State Changed: "
           << inst.timeFibrosisStateChanged << std::endl;
        os << "Number of Times Infected: " << inst.timesInfected << std::endl;
        os << "Number of Times Cleared: " << inst.timesCleared << std::endl;
        os << "Identified as a Positive Infection: " << inst.identifiedHCV
           << std::endl;
        os << "Time Identified: " << inst.timeIdentified << std::endl;
        return os;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, BehaviorDetails const &behav) {
        os << "Behavior Type: "
           << person::behaviorEnumToStringMap[behav.behavior] << std::endl;
        os << "Last Timestep Active: " << behav.timeLastActive << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, LinkageDetails const &ldet) {
        os << "Linkage State: "
           << person::linkageStateEnumToStringMap[ldet.linkState] << std::endl;
        os << "Last Time Linkage State Changed: " << ldet.timeOfLinkChange
           << std::endl;
        os << "Linkage Type: "
           << person::linkageTypeEnumToStringMap[ldet.linkType] << std::endl;
        os << "Times Linked: " << ldet.linkCount << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, MOUDDetails const &mdet) {
        os << "MOUD State: " << person::moudEnumToStringMap[mdet.moudState]
           << std::endl;
        os << "Time Started MOUD: " << mdet.timeStartedMoud << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, PregnancyDetails const &pdet) {
        os << "Pregnancy State: "
           << person::pregnancyStateEnumToStringMap[pdet.pregnancyState]
           << std::endl;
        os << "Time of Pregnancy State Change: " << pdet.timeOfPregnancyChange
           << std::endl;
        os << "Infant Count: " << pdet.numInfants << std::endl;
        os << "Miscarriage Count: " << pdet.numMiscarriages << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, StagingDetails const &sdet) {
        os << "Fibrosis State: "
           << person::measuredFibrosisStateEnumToStringMap
                  [sdet.measuredFibrosisState]
           << std::endl;
        os << "Has a Second Fibrosis Test: " << sdet.hadSecondTest << std::endl;
        os << "Time of Last Staging: " << sdet.timeOfLastStaging << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, ScreeningDetails const &sdet) {
        os << "Time of Last Screening: " << sdet.timeOfLastScreening
           << std::endl;
        os << "AB Test Counts: " << sdet.numABTests << std::endl;
        os << "RNA Testing Counts: " << sdet.numRNATests << std::endl;
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