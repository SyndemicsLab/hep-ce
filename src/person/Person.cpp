#include "Person.hpp"

namespace Person {
    int count = 0;

    std::vector<std::string> attributes = {"id",
                                           "sex",
                                           "age",
                                           "isAlive",
                                           "identifiedHCV",
                                           "timeInfectionIdentified",
                                           "HCV",
                                           "fibrosisState",
                                           "isGenotypeThree",
                                           "seropositive",
                                           "timeHCVChanged",
                                           "timeFibrosisStateChanged",
                                           "timesInfected",
                                           "timesCleared",
                                           "drugBehavior",
                                           "timeLastActiveDrugUse",
                                           "linkageState",
                                           "timeOfLinkChange",
                                           "linkageType",
                                           "linkCount",
                                           "measuredFibrosisState",
                                           "hadSecondTest",
                                           "timeOfLastStaging",
                                           "timeOfLastScreening",
                                           "numABTests",
                                           "numRNATests",
                                           "initiatedTreatment",
                                           "timeOfTreatmentInitiation"};

    enum class PersonAttribute {
        ID = 0,
        SEX = 1,
        AGE = 2,
        ISALIVE = 3,
        IDENTIFIEDHCV = 4,
        TIMEINFECTIONIDENTIFIED = 5,
        HCV = 6,
        FIBROSISSTATE = 7,
        ISGENOTYPETHREE = 8,
        SEROPOSITIVE = 9,
        TIMEHCVCHANGED = 10,
        TIMEFIBROSISSTATECHANGED = 11,
        TIMESINFECTED = 12,
        TIMESCLEARED = 13,
        DRUGBEHAVIOR = 14,
        TIMELASTACTIVEDRUGUSE = 15,
        LINKAGESTATE = 16,
        TIMEOFLINKCHANGE = 17,
        LINKAGETYPE = 18,
        LINKCOUNT = 19,
        MEASUREDFIBROSISSTATE = 20,
        HADSECONDTEST = 21,
        TIMEOFLASTSTAGING = 22,
        TIMEOFLASTSCREENING = 23,
        NUMABTESTS = 24,
        NUMRNATESTS = 25,
        INITIATEDTREATMENT = 26,
        TIMEOFTREATMENTINITIATION = 27,
        COUNT = 28
    };

    std::map<std::string, PersonAttribute> attributeMap = {
        {"id", PersonAttribute::ID},
        {"sex", PersonAttribute::SEX},
        {"age", PersonAttribute::AGE},
        {"isAlive", PersonAttribute::ISALIVE},
        {"identifiedHCV", PersonAttribute::IDENTIFIEDHCV},
        {"timeInfectionIdentified", PersonAttribute::TIMEINFECTIONIDENTIFIED},
        {"HCV", PersonAttribute::HCV},
        {"fibrosisState", PersonAttribute::FIBROSISSTATE},
        {"isGenotypeThree", PersonAttribute::ISGENOTYPETHREE},
        {"seropositive", PersonAttribute::SEROPOSITIVE},
        {"timeHCVChanged", PersonAttribute::TIMEHCVCHANGED},
        {"timeFibrosisStateChanged", PersonAttribute::TIMEFIBROSISSTATECHANGED},
        {"timesInfected", PersonAttribute::TIMESINFECTED},
        {"timesCleared", PersonAttribute::TIMESCLEARED},
        {"drugBehavior", PersonAttribute::DRUGBEHAVIOR},
        {"timeLastActiveDrugUse", PersonAttribute::TIMELASTACTIVEDRUGUSE},
        {"linkageState", PersonAttribute::LINKAGESTATE},
        {"timeOfLinkChange", PersonAttribute::TIMEOFLINKCHANGE},
        {"linkageType", PersonAttribute::LINKAGETYPE},
        {"linkCount", PersonAttribute::LINKCOUNT},
        {"measuredFibrosisState", PersonAttribute::MEASUREDFIBROSISSTATE},
        {"hadSecondTest", PersonAttribute::HADSECONDTEST},
        {"timeOfLastStaging", PersonAttribute::TIMEOFLASTSTAGING},
        {"timeOfLastScreening", PersonAttribute::TIMEOFLASTSCREENING},
        {"numABTests", PersonAttribute::NUMABTESTS},
        {"numRNATests", PersonAttribute::NUMRNATESTS},
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
            case PersonAttribute::IDENTIFIEDHCV:
                this->infectionStatus.identifiedHCV =
                    Utils::stobool((*dataTableRow)["identifiedHCV"][0]);
                break;
            case PersonAttribute::TIMEINFECTIONIDENTIFIED:
                this->infectionStatus.timeIdentified =
                    stoi((*dataTableRow)["timeInfectionIdentified"][0]);
                break;
            case PersonAttribute::HCV:
                this->infectionStatus.hcv =
                    Person::hcvMap[Utils::toLower((*dataTableRow)["HCV"][0])];
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
            case PersonAttribute::SEROPOSITIVE:
                this->infectionStatus.seropositive =
                    Utils::stobool((*dataTableRow)["seropositive"][0]);
                break;
            case PersonAttribute::TIMEHCVCHANGED:
                this->infectionStatus.timeHCVChanged =
                    stoi((*dataTableRow)["timeHCVChanged"][0]);
                break;
            case PersonAttribute::TIMEFIBROSISSTATECHANGED:
                this->infectionStatus.timeFibrosisStateChanged =
                    stoi((*dataTableRow)["timeFibrosisStateChanged"][0]);
                break;
            case PersonAttribute::TIMESINFECTED:
                this->infectionStatus.timesInfected =
                    stoi((*dataTableRow)["timesInfected"][0]);
                break;
            case PersonAttribute::TIMESCLEARED:
                this->infectionStatus.timesCleared =
                    stoi((*dataTableRow)["timesCleared"][0]);
                break;
            case PersonAttribute::DRUGBEHAVIOR:
                this->behaviorDetails.behavior =
                    Person::behaviorMap[Utils::toLower(
                        (*dataTableRow)["drugBehavior"][0])];
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
            case PersonAttribute::HADSECONDTEST:
                this->stagingDetails.hadSecondTest =
                    Utils::stobool((*dataTableRow)["hadSecondTest"][0]);
                break;
            case PersonAttribute::TIMEOFLASTSTAGING:
                this->stagingDetails.timeOfLastStaging =
                    stoi((*dataTableRow)["timeOfLastStaging"][0]);
                break;
            case PersonAttribute::TIMEOFLASTSCREENING:
                this->screeningDetails.timeOfLastScreening =
                    stoi((*dataTableRow)["timeOfLastScreening"][0]);
                break;
            case PersonAttribute::NUMABTESTS:
                this->screeningDetails.numABTests =
                    stoi((*dataTableRow)["numABTests"][0]);
                break;
            case PersonAttribute::NUMRNATESTS:
                this->screeningDetails.numRNATests =
                    stoi((*dataTableRow)["numRNATests"][0]);
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
        if (this->infectionStatus.hcv != HCV::NONE) {
            return;
        }
        this->infectionStatus.hcv = HCV::ACUTE;
        this->infectionStatus.timeHCVChanged = tstep;
        this->infectionStatus.seropositive = true;
        this->infectionStatus.timesInfected++;

        if (this->infectionStatus.fibrosisState != FibrosisState::NONE) {
            return;
        }
        // once infected, immediately enter F0
        this->infectionStatus.fibrosisState = FibrosisState::F0;
        this->infectionStatus.timeFibrosisStateChanged = tstep;
    }

    void Person::clearHCV(int tstep) {
        // cannot clear if the person is not infected
        if (this->infectionStatus.hcv == HCV::NONE) {
            return;
        }
        this->infectionStatus.hcv = HCV::NONE;
        this->infectionStatus.timeHCVChanged = tstep;
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

    void Person::updateBehavior(const Behavior &bc, int tstep) {
        // nothing to do -- cannot go back to NEVER
        if (bc == this->behaviorDetails.behavior || bc == Behavior::NEVER) {
            return;
        }
        // count for timeSinceActive if switching from active to non-active use
        if ((bc == Behavior::FORMER_NONINJECTION) ||
            (bc == Behavior::FORMER_INJECTION)) {
            this->behaviorDetails.timeLastActive = tstep;
        }
        // update the behavior classification
        this->behaviorDetails.behavior = bc;
    }

    FibrosisState Person::diagnoseFibrosis(int tstep) {
        // need to add functionality here
        this->infectionStatus.fibrosisState = FibrosisState::F0;
        return this->infectionStatus.fibrosisState;
    }

    HCV Person::diagnoseHEPC(int tstep) {
        // need to add functionality here
        this->infectionStatus.hcv = HCV::ACUTE;
        return this->infectionStatus.hcv;
    }

    void Person::setUtility(double util) {
        this->utilityTracker.minUtil =
            std::min(this->utilityTracker.minUtil, util);
        this->utilityTracker.multUtil *= util;
    }

    void Person::addCost(Cost::Cost cost, int timestep) {
        this->costs.addCost(cost, timestep);
    }

    std::ostream &operator<<(std::ostream &os, const Person &person) {
        os << "id: " << person.getID() << std::endl;
        os << "sex: " << Person::sexEnumToStringMap[person.getSex()]
           << std::endl;
        os << "alive: " << std::boolalpha << person.getIsAlive() << std::endl;
        os << person.getHealth() << std::endl;
        os << person.getBehaviorDetails() << std::endl;
        os << person.getLinkStatus() << std::endl;
        os << "Has Overdosed: " << std::boolalpha << person.getOverdose()
           << std::endl;
        os << person.getMOUDDetails() << std::endl;
        os << person.getPregnancyDetails() << std::endl;
        os << person.getStagingDetails() << std::endl;
        os << person.getScreeningDetails() << std::endl;
        os << person.getTreatmentDetails() << std::endl;
        os << person.getUtility() << std::endl;
        // os << person.getCosts() << std::endl;
        os << "Is a Baby Boomer: " << std::boolalpha << person.isBoomer()
           << std::endl;
        os << "Age: " << person.age << std::endl;
        return os;
    }
} // namespace Person
