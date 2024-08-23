#include "Person.hpp"

namespace Person {
    int count = 0;

    std::vector<std::string> attributes = {"id",
                                           "sex",
                                           "age",
                                           "isAlive",
                                           "identifiedAsPositiveInfection",
                                           "timeInfectionIdentified",
                                           "HEPCState",
                                           "fibrosisState",
                                           "isGenotypeThree",
                                           "seropositivity",
                                           "timeHEPCStateChanged",
                                           "timeFibrosisStateChanged",
                                           "numInfections",
                                           "numClearances",
                                           "drugBehaviorClassification",
                                           "timeLastActiveDrugUse",
                                           "linkageState",
                                           "timeOfLinkChange",
                                           "linkageType",
                                           "linkCount",
                                           "measuredFibrosisState",
                                           "hadFibTestTwo",
                                           "timeOfLastStaging",
                                           "timeOfLastScreening",
                                           "abCount",
                                           "rnaCount",
                                           "initiatedTreatment",
                                           "timeOfTreatmentInitiation"};

    enum class PersonAttribute {
        ID = 0,
        SEX = 1,
        AGE = 2,
        ISALIVE = 3,
        IDENTIFIEDASPOSITIVEINFECTION = 4,
        TIMEINFECTIONIDENTIFIED = 5,
        HEPCSTATE = 6,
        FIBROSISSTATE = 7,
        ISGENOTYPETHREE = 8,
        SEROPOSITIVITY = 9,
        TIMEHEPCSTATECHANGED = 10,
        TIMEFIBROSISSTATECHANGED = 11,
        NUMINFECTIONS = 12,
        NUMCLEARANCES = 13,
        DRUGBEHAVIORCLASSIFICATION = 14,
        TIMELASTACTIVEDRUGUSE = 15,
        LINKAGESTATE = 16,
        TIMEOFLINKCHANGE = 17,
        LINKAGETYPE = 18,
        LINKCOUNT = 19,
        MEASUREDFIBROSISSTATE = 20,
        HADFIBTESTTWO = 21,
        TIMEOFLASTSTAGING = 22,
        TIMEOFLASTSCREENING = 23,
        ABCOUNT = 24,
        RNACOUNT = 25,
        INITIATEDTREATMENT = 26,
        TIMEOFTREATMENTINITIATION = 27,
        COUNT = 28
    };

    std::map<std::string, PersonAttribute> attributeMap = {
        {"id", PersonAttribute::ID},
        {"sex", PersonAttribute::SEX},
        {"age", PersonAttribute::AGE},
        {"isAlive", PersonAttribute::ISALIVE},
        {"identifiedAsPositiveInfection",
         PersonAttribute::IDENTIFIEDASPOSITIVEINFECTION},
        {"timeInfectionIdentified", PersonAttribute::TIMEINFECTIONIDENTIFIED},
        {"HEPCState", PersonAttribute::HEPCSTATE},
        {"fibrosisState", PersonAttribute::FIBROSISSTATE},
        {"isGenotypeThree", PersonAttribute::ISGENOTYPETHREE},
        {"seropositivity", PersonAttribute::SEROPOSITIVITY},
        {"timeHEPCStateChanged", PersonAttribute::TIMEHEPCSTATECHANGED},
        {"timeFibrosisStateChanged", PersonAttribute::TIMEFIBROSISSTATECHANGED},
        {"numInfections", PersonAttribute::NUMINFECTIONS},
        {"numClearances", PersonAttribute::NUMCLEARANCES},
        {"drugBehaviorClassification",
         PersonAttribute::DRUGBEHAVIORCLASSIFICATION},
        {"timeLastActiveDrugUse", PersonAttribute::TIMELASTACTIVEDRUGUSE},
        {"linkageState", PersonAttribute::LINKAGESTATE},
        {"timeOfLinkChange", PersonAttribute::TIMEOFLINKCHANGE},
        {"linkageType", PersonAttribute::LINKAGETYPE},
        {"linkCount", PersonAttribute::LINKCOUNT},
        {"measuredFibrosisState", PersonAttribute::MEASUREDFIBROSISSTATE},
        {"hadFibTestTwo", PersonAttribute::HADFIBTESTTWO},
        {"timeOfLastStaging", PersonAttribute::TIMEOFLASTSTAGING},
        {"timeOfLastScreening", PersonAttribute::TIMEOFLASTSCREENING},
        {"abCount", PersonAttribute::ABCOUNT},
        {"rnaCount", PersonAttribute::RNACOUNT},
        {"initiatedTreatment", PersonAttribute::INITIATEDTREATMENT},
        {"timeOfTreatmentInitiation",
         PersonAttribute::TIMEOFTREATMENTINITIATION}};

    Person::Person(Data::IDataTablePtr dataTableRow) {
        count++;

        if (dataTableRow->empty()) {
            return;
        }

        for (auto &attribute : attributes) {
            // skip missing columns
            if (!dataTableRow->checkColumnExists(attribute)) {
                continue;
            }

            switch (attributeMap[attribute]) {
            case PersonAttribute::ID:
                this->id = stoi((*dataTableRow)["id"][0]);
                break;
            case PersonAttribute::SEX:
                this->sex =
                    Person::sexMap[Utils::toLower((*dataTableRow)["sex"][0])];
                break;
            case PersonAttribute::AGE:
                this->age = stoul((*dataTableRow)["age"][0]);
                break;
            case PersonAttribute::ISALIVE:
                this->isAlive = Utils::stobool((*dataTableRow)["isAlive"][0]);
                break;
            case PersonAttribute::IDENTIFIEDASPOSITIVEINFECTION:
                this->idStatus.identifiedAsPositiveInfection = Utils::stobool(
                    (*dataTableRow)["identifiedAsPositiveInfection"][0]);
                break;
            case PersonAttribute::TIMEINFECTIONIDENTIFIED:
                this->idStatus.timeIdentified =
                    stoi((*dataTableRow)["timeInfectionIdentified"][0]);
                break;
            case PersonAttribute::HEPCSTATE:
                this->infectionStatus.hepcState =
                    Person::hepcStateMap[Utils::toLower(
                        (*dataTableRow)["HEPCState"][0])];
                break;
            case PersonAttribute::FIBROSISSTATE:
                this->infectionStatus.fibrosisState =
                    Person::fibrosisStateMap[Utils::toLower(
                        (*dataTableRow)["fibrosisState"][0])];
                break;
            case PersonAttribute::ISGENOTYPETHREE:
                this->infectionStatus.isGenotypeThree =
                    Utils::stobool((*dataTableRow)["isGenotypeThree"][0]);
                break;
            case PersonAttribute::SEROPOSITIVITY:
                this->infectionStatus.seropositivity =
                    Utils::stobool((*dataTableRow)["seropositivity"][0]);
                break;
            case PersonAttribute::TIMEHEPCSTATECHANGED:
                this->infectionStatus.timeHEPCStateChanged =
                    stoi((*dataTableRow)["timeHEPCStateChanged"][0]);
                break;
            case PersonAttribute::TIMEFIBROSISSTATECHANGED:
                this->infectionStatus.timeFibrosisStateChanged =
                    stoi((*dataTableRow)["timeFibrosisStateChanged"][0]);
                break;
            case PersonAttribute::NUMINFECTIONS:
                this->infectionStatus.numInfections =
                    stoi((*dataTableRow)["numInfections"][0]);
                break;
            case PersonAttribute::NUMCLEARANCES:
                this->infectionStatus.numClearances =
                    stoi((*dataTableRow)["numClearances"][0]);
                break;
            case PersonAttribute::DRUGBEHAVIORCLASSIFICATION:
                this->behaviorDetails.behaviorClassification =
                    Person::behaviorClassificationMap[Utils::toLower(
                        (*dataTableRow)["drugBehaviorClassification"][0])];
                break;
            case PersonAttribute::TIMELASTACTIVEDRUGUSE:
                this->behaviorDetails.timeLastActive =
                    stoul((*dataTableRow)["timeLastActiveDrugUse"][0]);
                break;
            case PersonAttribute::LINKAGESTATE:
                this->linkStatus.linkState =
                    Person::linkageStateMap[Utils::toLower(
                        (*dataTableRow)["linkageState"][0])];
                break;
            case PersonAttribute::TIMEOFLINKCHANGE:
                this->linkStatus.timeOfLinkChange =
                    stoi((*dataTableRow)["timeOfLinkChange"][0]);
                break;
            case PersonAttribute::LINKAGETYPE:
                this->linkStatus.linkType =
                    Person::linkageTypeMap[Utils::toLower(
                        (*dataTableRow)["linkageType"][0])];
                break;
            case PersonAttribute::LINKCOUNT:
                this->linkStatus.linkCount =
                    stoi((*dataTableRow)["linkCount"][0]);
                break;
            case PersonAttribute::MEASUREDFIBROSISSTATE:
                this->stagingDetails.measuredFibrosisState =
                    Person::measuredFibrosisStateMap[Utils::toLower(
                        (*dataTableRow)["measuredFibrosisState"][0])];
                break;
            case PersonAttribute::HADFIBTESTTWO:
                this->stagingDetails.hadFibTestTwo =
                    Utils::stobool((*dataTableRow)["hadFibTestTwo"][0]);
                break;
            case PersonAttribute::TIMEOFLASTSTAGING:
                this->stagingDetails.timeOfLastStaging =
                    stoi((*dataTableRow)["timeOfLastStaging"][0]);
                break;
            case PersonAttribute::TIMEOFLASTSCREENING:
                this->screeningDetails.timeOfLastScreening =
                    stoi((*dataTableRow)["timeOfLastScreening"][0]);
                break;
            case PersonAttribute::ABCOUNT:
                this->screeningDetails.abCount =
                    stoi((*dataTableRow)["abCount"][0]);
                break;
            case PersonAttribute::RNACOUNT:
                this->screeningDetails.rnaCount =
                    stoi((*dataTableRow)["rnaCount"][0]);
                break;
            case PersonAttribute::INITIATEDTREATMENT:
                this->treatmentDetails.initiatedTreatment =
                    Utils::stobool((*dataTableRow)["initiatedTreatment"][0]);
                break;
            case PersonAttribute::TIMEOFTREATMENTINITIATION:
                this->treatmentDetails.timeOfTreatmentInitiation =
                    std::stoi((*dataTableRow)["timeOfTreatmentInitiation"][0]);
                break;
            }
        }
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

    void Person::setUtility(double util) {
        this->utilityTracker.minUtil =
            std::min(this->utilityTracker.minUtil, util);
        this->utilityTracker.multUtil *= util;
    }

    void Person::addCost(Cost::Cost cost, int timestep) {
        this->costs.addCost(cost, timestep);
    }
} // namespace Person
