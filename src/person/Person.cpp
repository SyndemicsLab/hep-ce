#include "Person.hpp"

namespace Person {
    int count = 0;

    Person::Person(Data::IDataTablePtr dataTableRow) {
        count++;

        if (dataTableRow->empty() || dataTableRow->ncols() < 37) {
            return;
        }

        this->id = stoi((*dataTableRow)["id"][0]);
        this->sex = Person::sexMap[Utils::toLower((*dataTableRow)["sex"][0])];
        this->age = stoul((*dataTableRow)["age"][0]);
        this->isAlive = Utils::stobool((*dataTableRow)["isAlive"][0]);
        this->idStatus.identifiedAsPositiveInfection =
            Utils::stobool((*dataTableRow)["identifiedAsPositiveInfection"][0]);
        this->idStatus.timeIdentified =
            stoi((*dataTableRow)["timeInfectionIdentified"][0]);
        this->infectionStatus.hepcState = Person::hepcStateMap[Utils::toLower(
            (*dataTableRow)["HEPCState"][0])];
        this->infectionStatus.fibrosisState =
            Person::fibrosisStateMap[Utils::toLower(
                (*dataTableRow)["fibrosisState"][0])];
        this->infectionStatus.isGenotypeThree =
            Utils::stobool((*dataTableRow)["isGenotypeThree"][0]);
        this->infectionStatus.seropositivity =
            Utils::stobool((*dataTableRow)["seropositivity"][0]);
        this->infectionStatus.timeHEPCStateChanged =
            stoi((*dataTableRow)["timeHEPCStateChanged"][0]);
        this->infectionStatus.timeFibrosisStateChanged =
            stoi((*dataTableRow)["timeFibrosisStateChanged"][0]);
        this->infectionStatus.numInfections =
            stoi((*dataTableRow)["numInfections"][0]);
        this->infectionStatus.numClearances =
            stoi((*dataTableRow)["numClearances"][0]);
        this->behaviorDetails.behaviorClassification =
            Person::behaviorClassificationMap[Utils::toLower(
                (*dataTableRow)["drugBehaviorClassification"][0])];
        this->behaviorDetails.timeLastActive =
            stoul((*dataTableRow)["timeLastActiveDrugUse"][0]);
        this->linkStatus.linkState = Person::linkageStateMap[Utils::toLower(
            (*dataTableRow)["linkageState"][0])];
        this->linkStatus.timeOfLinkChange =
            stoi((*dataTableRow)["timeOfLinkChange"][0]);
        this->linkStatus.linkType = Person::linkageTypeMap[Utils::toLower(
            (*dataTableRow)["linkageType"][0])];
        this->linkStatus.linkCount = stoi((*dataTableRow)["linkCount"][0]);
        this->stagingDetails.measuredFibrosisState =
            Person::measuredFibrosisStateMap[Utils::toLower(
                (*dataTableRow)["measuredFibrosisState"][0])];
        this->stagingDetails.timeOfLastStaging =
            stoi((*dataTableRow)["timeOfLastStaging"][0]);
        this->screeningDetails.timeOfLastScreening =
            stoi((*dataTableRow)["timeOfLastScreening"][0]);
        this->screeningDetails.abCount = stoi((*dataTableRow)["abCount"][0]);
        this->screeningDetails.rnaCount = stoi((*dataTableRow)["rnaCount"][0]);
        this->treatmentDetails.incompleteTreatment =
            Utils::stobool((*dataTableRow)["hasIncompleteTreatment"][0]);
        this->treatmentDetails.initiatedTreatment =
            Utils::stobool((*dataTableRow)["initiatedTreatment"][0]);
        this->treatmentDetails.timeOfTreatmentInitiation =
            std::stoi((*dataTableRow)["timeOfTreatmentInitiation"][0]);
        this->treatmentDetails.treatmentCount =
            std::stoi((*dataTableRow)["treatmentCount"][0]);
        this->treatmentDetails.numEOT = std::stoi((*dataTableRow)["numEOT"][0]);
        this->treatmentDetails.numSVR = std::stoi((*dataTableRow)["numSVR"][0]);
        this->treatmentDetails.numTox = std::stoi((*dataTableRow)["numTox"][0]);
        this->treatmentDetails.numWithdrawals =
            std::stoi((*dataTableRow)["numWithdrawals"][0]);
        this->utility.background =
            std::stod((*dataTableRow)["backgroundUtility"][0]);
        this->utility.behavior =
            std::stod((*dataTableRow)["behaviorUtility"][0]);
        this->utility.treatment =
            std::stod((*dataTableRow)["treatmentUtility"][0]);
        this->utility.liver = std::stod((*dataTableRow)["liverUtility"][0]);
    }

    void Person::die() { this->isAlive = false; }

    void Person::grow() {
        if (this->isAlive) {
            this->age++;
        }
    }

    void Person::infect(int tstep) {
        // cannot be multiply infected
        if (this->infectionStatus.hepcState != HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        this->infectionStatus.timeHEPCStateChanged = tstep;
        this->infectionStatus.seropositivity = true;
        this->infectionStatus.numInfections++;

        if (this->infectionStatus.fibrosisState != FibrosisState::NONE) {
            return;
        }
        // once infected, immediately enter F0
        this->infectionStatus.fibrosisState = FibrosisState::F0;
        this->infectionStatus.timeFibrosisStateChanged = tstep;
    }

    void Person::clearHCV(int tstep) {
        // cannot clear if the person is not infected
        if (this->infectionStatus.hepcState == HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::NONE;
        this->infectionStatus.timeHEPCStateChanged = tstep;
        this->addClearance();
    }

    void Person::updateFibrosis(const FibrosisState &ls, int tstep) {
        // nothing to do -- can only advance fibrosis state
        if (ls <= this->infectionStatus.fibrosisState) {
            return;
        }
        this->infectionStatus.fibrosisState = ls;
        this->infectionStatus.timeFibrosisStateChanged = tstep;
    }

    void Person::updateBehavior(const BehaviorClassification &bc, int tstep) {
        // nothing to do -- cannot go back to NEVER
        if (bc == this->behaviorDetails.behaviorClassification ||
            bc == BehaviorClassification::NEVER) {
            return;
        }
        // count for timeSinceActive if switching from active to non-active use
        if ((bc == BehaviorClassification::FORMER_NONINJECTION) ||
            (bc == BehaviorClassification::FORMER_INJECTION)) {
            this->behaviorDetails.timeLastActive = tstep;
        }
        // update the behavior classification
        this->behaviorDetails.behaviorClassification = bc;
    }

    FibrosisState Person::diagnoseFibrosis(int tstep) {
        // need to add functionality here
        this->infectionStatus.fibrosisState = FibrosisState::F0;
        return this->infectionStatus.fibrosisState;
    }

    HEPCState Person::diagnoseHEPC(int tstep) {
        // need to add functionality here
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        return this->infectionStatus.hepcState;
    }

    void Person::setUtility(UtilityCategory category, double value) {
        if ((value > 1) || (value < 0)) {
            // log error
            return;
        }
        switch (category) {
        case UtilityCategory::BACKGROUND:
            this->utility.background = value;
            break;
        case UtilityCategory::BEHAVIOR:
            this->utility.behavior = value;
            break;
        case UtilityCategory::TREATMENT:
            this->utility.treatment = value;
            break;
        case UtilityCategory::LIVER:
            this->utility.liver = value;
            break;
        }
    }

    std::pair<double, double> Person::getUtilities() const {
        using std::min;
        const auto &util = this->utility;
        // utilities presented as {min, mult}
        std::pair<double, double> utilities = {
            min(min(util.background, util.behavior),
                min(util.treatment, util.liver)),
            util.background * util.behavior * util.treatment * util.liver};
        return utilities;
    }

    void Person::addCost(Cost::Cost cost, int timestep) {
        this->costs.addCost(cost, timestep);
    }
} // namespace Person
