#include "Containers.hpp"

namespace person {
    std::ostream &operator<<(std::ostream &os, const HCV &inst) {
        switch (inst) {
        case HCV::ACUTE:
            os << "acute";
            break;
        case HCV::CHRONIC:
            os << "chronic";
            break;
        default:
            os << "none";
            break;
        }
        return os;
    }
    HCV &operator<<(HCV &inst, const std::string &str) {
        if (str == "acute") {
            inst = HCV::ACUTE;
        } else if (str == "chronic") {
            inst = HCV::CHRONIC;
        } else {
            inst = HCV::NONE;
        }
        return inst;
    }

    std::ostream &operator<<(std::ostream &os, const DeathReason &inst) {
        switch (inst) {
        case DeathReason::BACKGROUND:
            os << "background";
            break;
        case DeathReason::LIVER:
            os << "liver";
            break;
        case DeathReason::INFECTION:
            os << "infection";
            break;
        case DeathReason::AGE:
            os << "age";
            break;
        case DeathReason::OVERDOSE:
            os << "overdose";
            break;
        default:
            os << "N/A";
            break;
        }
        return os;
    }
    DeathReason &operator<<(DeathReason &inst, const std::string &str) {
        if (str == "background") {
            inst = DeathReason::BACKGROUND;
        } else if (str == "liver") {
            inst = DeathReason::LIVER;
        } else if (str == "infection") {
            inst = DeathReason::INFECTION;
        } else if (str == "age") {
            inst = DeathReason::AGE;
        } else if (str == "overdose") {
            inst = DeathReason::OVERDOSE;
        } else {
            inst = DeathReason::NA;
        }
        return inst;
    }

    std::ostream &operator<<(std::ostream &os, const Behavior &inst) {
        switch (inst) {
        case Behavior::FORMER_INJECTION:
            os << "former_injection";
            break;
        case Behavior::FORMER_NONINJECTION:
            os << "former_noninjection";
            break;
        case Behavior::INJECTION:
            os << "injection";
            break;
        case Behavior::NONINJECTION:
            os << "noninjection";
            break;
        default:
            os << "never";
            break;
        }
        return os;
    }
    Behavior &operator<<(Behavior &inst, const std::string &str) {
        if (str == "former_injection") {
            inst = Behavior::FORMER_INJECTION;
        } else if (str == "former_noninjection") {
            inst = Behavior::FORMER_NONINJECTION;
        } else if (str == "injection") {
            inst = Behavior::INJECTION;
        } else if (str == "noninjection") {
            inst = Behavior::NONINJECTION;
        } else {
            inst = Behavior::NEVER;
        }
        return inst;
    }

    std::ostream &operator<<(std::ostream &os, const LinkageType &inst) {
        switch (inst) {
        case LinkageType::BACKGROUND:
            os << "background";
            break;
        case LinkageType::INTERVENTION:
            os << "intervention";
            break;
        default:
            os << "NA";
            break;
        }
        return os;
    }
    LinkageType &operator<<(LinkageType &inst, const std::string &str) {
        if (str == "background") {
            inst = LinkageType::BACKGROUND;
        } else if (str == "intervention") {
            inst = LinkageType::INTERVENTION;
        } else {
            inst = LinkageType::NA;
        }
        return inst;
    }

    std::ostream &operator<<(std::ostream &os, const LinkageState &inst) {
        switch (inst) {
        case LinkageState::LINKED:
            os << "linked";
            break;
        case LinkageState::UNLINKED:
            os << "unlinked";
            break;
        default:
            os << "never";
            break;
        }
        return os;
    }
    LinkageState &operator<<(LinkageState &inst, const std::string &str) {
        if (str == "linked") {
            inst = LinkageState::LINKED;
        } else if (str == "unlinked") {
            inst = LinkageState::UNLINKED;
        } else {
            inst == LinkageState::NEVER;
        }
        return inst;
    }

    std::ostream &operator<<(std::ostream &os, const FibrosisState &inst) {
        switch (inst) {
        case FibrosisState::DECOMP:
            os << "decomp";
            break;
        case FibrosisState::F0:
            os << "f0";
            break;
        case FibrosisState::F1:
            os << "f1";
            break;
        case FibrosisState::F2:
            os << "f2";
            break;
        case FibrosisState::F3:
            os << "f3";
            break;
        case FibrosisState::F4:
            os << "f4";
            break;
        default:
            os << "none";
            break;
        }
        return os;
    }
    FibrosisState &operator<<(FibrosisState &inst, const std::string &str) {
        if (str == "decomp") {
            inst = FibrosisState::DECOMP;
        } else if (str == "f0") {
            inst = FibrosisState::F0;
        } else if (str == "f1") {
            inst = FibrosisState::F1;
        } else if (str == "f2") {
            inst = FibrosisState::F2;
        } else if (str == "f3") {
            inst = FibrosisState::F3;
        } else if (str == "f4") {
            inst = FibrosisState::F4;
        } else {
            inst = FibrosisState::NONE;
        }
        return inst;
    }

    FibrosisState &operator++(FibrosisState &inst) {
        switch (inst) {
        case FibrosisState::F0:
            inst = FibrosisState::F1;
            break;
        case FibrosisState::F1:
            inst = FibrosisState::F2;
            break;
        case FibrosisState::F2:
            inst = FibrosisState::F3;
            break;
        case FibrosisState::F3:
            inst = FibrosisState::F4;
            break;
        case FibrosisState::F4:
            inst = FibrosisState::DECOMP;
            break;
        case FibrosisState::DECOMP:
            inst = FibrosisState::DECOMP;
            break;
        default:
            break;
        }
        return inst;
    }
    FibrosisState operator++(FibrosisState &inst, int) {
        FibrosisState rValue = inst;
        ++inst;
        return rValue;
    }

    std::ostream &operator<<(std::ostream &os, const HCCState &inst) {
        switch (inst) {
        case HCCState::EARLY:
            os << "early";
            break;
        case HCCState::LATE:
            os << "late";
            break;
        default:
            os << "none";
            break;
        }
        return os;
    }
    HCCState &operator<<(HCCState &inst, const std::string &str) {
        if (str == "early") {
            inst = HCCState::EARLY;
        } else if (str == "late") {
            inst = HCCState::LATE;
        } else {
            inst = HCCState::NONE;
        }
        return inst;
    }

    std::ostream &operator<<(std::ostream &os,
                             const MeasuredFibrosisState &inst) {
        switch (inst) {
        case MeasuredFibrosisState::DECOMP:
            os << "decomp";
            break;
        case MeasuredFibrosisState::F01:
            os << "f01";
            break;
        case MeasuredFibrosisState::F23:
            os << "f23";
            break;
        case MeasuredFibrosisState::F4:
            os << "f4";
            break;
        default:
            os << "none";
            break;
        }
        return os;
    }
    MeasuredFibrosisState &operator<<(MeasuredFibrosisState &inst,
                                      const std::string &str) {
        if (str == "decomp") {
            inst = MeasuredFibrosisState::DECOMP;
        } else if (str == "f01") {
            inst = MeasuredFibrosisState::F01;
        } else if (str == "f23") {
            inst = MeasuredFibrosisState::F23;
        } else if (str == "f4") {
            inst = MeasuredFibrosisState::F4;
        } else {
            inst = MeasuredFibrosisState::NONE;
        }
        return inst;
    }

    MeasuredFibrosisState &operator++(MeasuredFibrosisState &inst) {
        switch (inst) {
        case MeasuredFibrosisState::F01:
            inst = MeasuredFibrosisState::F23;
            break;
        case MeasuredFibrosisState::F23:
            inst = MeasuredFibrosisState::F4;
            break;
        case MeasuredFibrosisState::F4:
            inst = MeasuredFibrosisState::DECOMP;
            break;
        case MeasuredFibrosisState::DECOMP:
            inst = MeasuredFibrosisState::DECOMP;
            break;
        default:
            break;
        }
        return inst;
    }
    MeasuredFibrosisState operator++(MeasuredFibrosisState &inst, int) {
        MeasuredFibrosisState rValue = inst;
        ++inst;
        return rValue;
    }

    std::ostream &operator<<(std::ostream &os, const MOUD &inst) {
        switch (inst) {
        case MOUD::CURRENT:
            os << "current";
            break;
        case MOUD::POST:
            os << "post";
            break;
        default:
            os << "none";
            break;
        }
        return os;
    }
    MOUD &operator<<(MOUD &inst, const std::string &str) {
        if (str == "current") {
            inst = MOUD::CURRENT;
        } else if (str == "post") {
            inst = MOUD::POST;
        } else {
            inst = MOUD::NONE;
        }
        return inst;
    }

    std::ostream &operator<<(std::ostream &os, const Sex &inst) {
        switch (inst) {
        case Sex::MALE:
            os << "male";
            break;
        case Sex::FEMALE:
            os << "female";
            break;
        default:
            os << "";
            break;
        }
        return os;
    }
    Sex &operator<<(Sex &inst, const std::string &str) {
        if (str == "male") {
            inst = Sex::MALE;
        } else {
            inst = Sex::FEMALE;
        }
        return inst;
    }

    std::ostream &operator<<(std::ostream &os, const PregnancyState &inst) {
        switch (inst) {
        case PregnancyState::POSTPARTUM:
            os << "postpartum";
            break;
        case PregnancyState::PREGNANT:
            os << "pregnant";
            break;
        default:
            os << "none";
            break;
        }
        return os;
    }
    PregnancyState &operator<<(PregnancyState &inst, const std::string &str) {
        if (str == "postpartum") {
            inst = PregnancyState::POSTPARTUM;
        } else if (str == "pregnant") {
            inst = PregnancyState::PREGNANT;
        } else {
            inst = PregnancyState::NONE;
        }
        return inst;
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

    std::ostream &operator<<(std::ostream &os, Child const &inst) {
        os << "HCV State: " << inst.hcv << std::endl;
        os << "Tested for HCV: " << inst.tested << std::endl;
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