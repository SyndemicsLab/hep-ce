#include "Containers.hpp"

namespace person {
    std::ostream &operator<<(std::ostream &os, const HCV &inst) {
        switch (inst) {
        case HCV::ACUTE:
            os << "Acute";
            break;
        case HCV::CHRONIC:
            os << "Chronic";
            break;
        default:
            os << "None";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const Behavior &inst) {
        switch (inst) {
        case Behavior::FORMER_INJECTION:
            os << "Former Injection";
            break;
        case Behavior::FORMER_NONINJECTION:
            os << "Former Noninjection";
            break;
        case Behavior::INJECTION:
            os << "Injection";
            break;
        case Behavior::NONINJECTION:
            os << "Noninjection";
            break;
        default:
            os << "Never";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const LinkageType &inst) {
        switch (inst) {
        case LinkageType::BACKGROUND:
            os << "Background";
            break;
        default:
            os << "Intervention";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const LinkageState &inst) {
        switch (inst) {
        case LinkageState::LINKED:
            os << "Linked";
            break;
        case LinkageState::UNLINKED:
            os << "Unlinked";
            break;
        default:
            os << "Never";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const FibrosisState &inst) {
        switch (inst) {
        case FibrosisState::DECOMP:
            os << "Decomp";
            break;
        case FibrosisState::F0:
            os << "F0";
            break;
        case FibrosisState::F1:
            os << "F1";
            break;
        case FibrosisState::F2:
            os << "F2";
            break;
        case FibrosisState::F3:
            os << "F3";
            break;
        case FibrosisState::F4:
            os << "F4";
            break;
        default:
            os << "None";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const HCCState &inst) {
        switch (inst) {
        case HCCState::EARLY:
            os << "Early";
            break;
        case HCCState::LATE:
            os << "Late";
            break;
        default:
            os << "None";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os,
                             const MeasuredFibrosisState &inst) {
        switch (inst) {
        case MeasuredFibrosisState::DECOMP:
            os << "Decomp";
            break;
        case MeasuredFibrosisState::F01:
            os << "F01";
            break;
        case MeasuredFibrosisState::F23:
            os << "F23";
            break;
        case MeasuredFibrosisState::F4:
            os << "F4";
            break;
        default:
            os << "None";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const MOUD &inst) {
        switch (inst) {
        case MOUD::CURRENT:
            os << "Current";
            break;
        case MOUD::POST:
            os << "Post";
            break;
        default:
            os << "None";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const Sex &inst) {
        switch (inst) {
        case Sex::MALE:
            os << "Male";
            break;
        case Sex::FEMALE:
            os << "Female";
            break;
        default:
            os << "";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const PregnancyState &inst) {
        switch (inst) {
        case PregnancyState::POSTPARTUM:
            os << "Postpartum";
            break;
        case PregnancyState::PREGNANT:
            os << "Pregnant";
            break;
        default:
            os << "None";
            break;
        }
        return os;
    }

    std::ostream &operator<<(std::ostream &os, Health const &inst) {
        os << "HCV State: " << inst.hcv << std::endl;
        os << "Fibrosis State: " << inst.fibrosisState << std::endl;
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
    }

    std::ostream &operator<<(std::ostream &os, BehaviorDetails const &behav) {
        os << "Behavior Type: " << behav.behavior << std::endl;
        os << "Last Timestep Active: " << behav.timeLastActive << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, LinkageDetails const &ldet) {
        os << "Linkage State: " << ldet.linkState << std::endl;
        os << "Last Time Linkage State Changed: " << ldet.timeOfLinkChange
           << std::endl;
        os << "Linkage Type: " << ldet.linkType << std::endl;
        os << "Times Linked: " << ldet.linkCount << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, MOUDDetails const &mdet) {
        os << "MOUD State: " << mdet.moudState << std::endl;
        os << "Time Started MOUD: " << mdet.timeStartedMoud << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, PregnancyDetails const &pdet) {
        os << "Pregnancy State: " << pdet.pregnancyState << std::endl;
        os << "Time of Pregnancy State Change: " << pdet.timeOfPregnancyChange
           << std::endl;
        os << "Infant Count: " << pdet.numInfants << std::endl;
        os << "Miscarriage Count: " << pdet.numMiscarriages << std::endl;
        return os;
    }

    std::ostream &operator<<(std::ostream &os, StagingDetails const &sdet) {
        os << "Fibrosis State: " << sdet.measuredFibrosisState << std::endl;
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
} // namespace person