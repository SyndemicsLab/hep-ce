#include "Person.hpp"

namespace Person {
    int count = 0;
    std::unordered_map<std::string, HEPCState> Person::hepcStateMap = {
        {"none", HEPCState::NONE},
        {"acute", HEPCState::ACUTE},
        {"chronic", HEPCState::CHRONIC}};

    std::unordered_map<std::string, BehaviorClassification>
        Person::behaviorClassificationMap = {
            {"never", BehaviorClassification::NEVER},
            {"former_noninjection",
             BehaviorClassification::FORMER_NONINJECTION},
            {"former_injection", BehaviorClassification::FORMER_INJECTION},
            {"noninjection", BehaviorClassification::NONINJECTION},
            {"injection", BehaviorClassification::INJECTION}};

    std::unordered_map<std::string, LinkageType> Person::linkageTypeMap = {
        {"background", LinkageType::BACKGROUND},
        {"intervention", LinkageType::INTERVENTION}};

    std::unordered_map<std::string, LinkageState> Person::linkageStateMap = {
        {"never", LinkageState::NEVER},
        {"linked", LinkageState::LINKED},
        {"unlinked", LinkageState::UNLINKED}};

    std::unordered_map<std::string, LiverState> Person::liverStateMap = {
        {"none", LiverState::NONE},     {"f0", LiverState::F0},
        {"f1", LiverState::F1},         {"f2", LiverState::F2},
        {"f3", LiverState::F3},         {"f4", LiverState::F4},
        {"decomp", LiverState::DECOMP}, {"ehcc", LiverState::EHCC},
        {"lhcc", LiverState::LHCC}};

    std::unordered_map<std::string, MeasuredLiverState>
        Person::measuredLiverStateMap = {
            {"none", MeasuredLiverState::NONE},
            {"f01", MeasuredLiverState::F01},
            {"f23", MeasuredLiverState::F23},
            {"f4", MeasuredLiverState::F4},
            {"decomp", MeasuredLiverState::DECOMP}};

    std::unordered_map<std::string, MOUD> Person::moudMap = {
        {"none", MOUD::NONE}, {"current", MOUD::CURRENT}, {"post", MOUD::POST}};

    std::unordered_map<std::string, Sex> Person::sexMap = {
        {"male", Sex::MALE}, {"female", Sex::FEMALE}};

    std::unordered_map<std::string, PregnancyState> Person::pregnancyStateMap =
        {{"none", PregnancyState::NONE},
         {"pregnant", PregnancyState::PREGNANT},
         {"postpartum", PregnancyState::POSTPARTUM}};

    std::unordered_map<HEPCState, std::string>
        Person::hepcStateEnumToStringMap = {{HEPCState::NONE, "none"},
                                            {HEPCState::ACUTE, "acute"},
                                            {HEPCState::CHRONIC, "chronic"}};
    ;
    std::unordered_map<BehaviorClassification, std::string>
        Person::behaviorClassificationEnumToStringMap = {
            {BehaviorClassification::NEVER, "never"},
            {BehaviorClassification::FORMER_NONINJECTION,
             "former_noninjection"},
            {BehaviorClassification::FORMER_INJECTION, "former_injection"},
            {BehaviorClassification::NONINJECTION, "noninjection"},
            {BehaviorClassification::INJECTION, "injection"}};
    std::unordered_map<LinkageType, std::string>
        Person::linkageTypeEnumToStringMap = {
            {LinkageType::BACKGROUND, "background"},
            {LinkageType::INTERVENTION, "intervention"}};
    std::unordered_map<LinkageState, std::string>
        Person::linkageStateEnumToStringMap = {
            {LinkageState::NEVER, "never"},
            {LinkageState::LINKED, "linked"},
            {LinkageState::UNLINKED, "unlinked"}};

    std::unordered_map<LiverState, std::string>
        Person::liverStateEnumToStringMap = {
            {LiverState::NONE, "none"},     {LiverState::F0, "f0"},
            {LiverState::F1, "f1"},         {LiverState::F2, "f2"},
            {LiverState::F3, "f3"},         {LiverState::F4, "f4"},
            {LiverState::DECOMP, "decomp"}, {LiverState::EHCC, "ehcc"},
            {LiverState::LHCC, "lhcc"}};
    std::unordered_map<MeasuredLiverState, std::string>
        Person::measuredLiverStateEnumToStringMap = {
            {MeasuredLiverState::NONE, "none"},
            {MeasuredLiverState::F01, "f01"},
            {MeasuredLiverState::F23, "f23"},
            {MeasuredLiverState::F4, "f4"},
            {MeasuredLiverState::DECOMP, "decomp"}};
    std::unordered_map<MOUD, std::string> Person::moudEnumToStringMap = {
        {MOUD::NONE, "none"}, {MOUD::CURRENT, "current"}, {MOUD::POST, "post"}};
    std::unordered_map<Sex, std::string> Person::sexEnumToStringMap = {
        {Sex::MALE, "male"}, {Sex::FEMALE, "female"}};

    std::unordered_map<PregnancyState, std::string>
        Person::pregnancyStateEnumToStringMap = {
            {PregnancyState::NONE, "none"},
            {PregnancyState::PREGNANT, "pregnant"},
            {PregnancyState::POSTPARTUM, "postpartum"}};

    Person::Person(Data::IDataTablePtr dataTableRow, int simCycle) {
        count++;
        if (dataTableRow->empty() || dataTableRow->ncols() < 29) {
            return;
        }

        this->id = stoi((*dataTableRow)["id"][0]);

        this->sex = Person::sexMap[Utils::toLower((*dataTableRow)["sex"][0])];
        this->age = stod((*dataTableRow)["age"][0]);

        this->isAlive = Utils::stobool((*dataTableRow)["isAlive"][0]);

        this->screeningDetails.timeOfLastScreening =
            stoi((*dataTableRow)["timeOfLastScreening"][0]);
        this->screeningDetails.screeningFrequency =
            stoi((*dataTableRow)["screeningFrequency"][0]);
        this->screeningDetails.interventionScreening =
            Utils::stobool((*dataTableRow)["hasInterventionScreening"][0]);

        this->idStatus.timeIdentified =
            stoi((*dataTableRow)["timeIdentified"][0]);
        this->idStatus.identifiedAsPositiveInfection =
            Utils::stobool((*dataTableRow)["identifiedAsPositive"][0]);

        this->infectionStatus.hepcState = Person::hepcStateMap[Utils::toLower(
            (*dataTableRow)["hepcState"][0])];
        this->infectionStatus.timeHEPCStateChanged =
            stoi((*dataTableRow)["timeHEPCStateChanged"][0]);
        this->infectionStatus.seropositivity =
            Utils::stobool((*dataTableRow)["seropositivity"][0]);
        this->infectionStatus.liverState = Person::liverStateMap[Utils::toLower(
            (*dataTableRow)["liverState"][0])];
        this->infectionStatus.timeLiverStateChanged =
            stoi((*dataTableRow)["timeLiverStateChanged"][0]);

        this->stagingDetails.measuredLiverState =
            Person::measuredLiverStateMap[Utils::toLower(
                (*dataTableRow)["measuredLiverState"][0])];
        this->stagingDetails.timeOfLastStaging =
            stoi((*dataTableRow)["timeOfLastStaging"][0]);

        this->behaviorDetails.behaviorClassification =
            Person::behaviorClassificationMap[Utils::toLower(
                (*dataTableRow)["drugBehavior"][0])];
        this->behaviorDetails.timeLastActive =
            stoi((*dataTableRow)["timeLastActiveDrugUse"][0]);

        this->linkStatus.linkState = Person::linkageStateMap[Utils::toLower(
            (*dataTableRow)["linkageState"][0])];
        this->linkStatus.timeOfLinkChange =
            stoi((*dataTableRow)["timeOfLinkChange"][0]);
        this->linkStatus.linkType = Person::linkageTypeMap[Utils::toLower(
            (*dataTableRow)["linkageType"][0])];

        this->overdose = Utils::stobool((*dataTableRow)["isOverdosed"][0]);

        this->incompleteTreatment =
            Utils::stobool((*dataTableRow)["hasIncompleteTreatment"][0]);

        this->moudDetails.moudState =
            Person::moudMap[Utils::toLower((*dataTableRow)["MOUDState"][0])];
        this->moudDetails.timeStartedMoud =
            stoi((*dataTableRow)["timeStartedMOUD"][0]);

        this->pregnancyDetails.pregnancyState =
            Person::pregnancyStateMap[Utils::toLower(
                (*dataTableRow)["pregnancyState"][0])];
        this->pregnancyDetails.timeOfPregnancyChange =
            stoi((*dataTableRow)["timeOfPregnancyChange"][0]);
        this->pregnancyDetails.infantCount =
            stoi((*dataTableRow)["infantCount"][0]);
        this->pregnancyDetails.miscarriageCount =
            stoi((*dataTableRow)["miscarriageCount"][0]);
    }

    void Person::die() { this->isAlive = false; }

    void Person::grow() {
        if (this->isAlive) {
            this->age++;
        }
    }

    void Person::infect(int timestep) {
        // cannot be multiply infected
        if (this->infectionStatus.hepcState != HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        this->infectionStatus.timeHEPCStateChanged = timestep;
        this->infectionStatus.seropositivity = true;

        if (this->infectionStatus.liverState != LiverState::NONE) {
            return;
        }
        // once infected, immediately enter F0
        this->infectionStatus.liverState = LiverState::F0;
        this->infectionStatus.timeLiverStateChanged = timestep;
    }

    void Person::clearHCV(int timestep) {
        // cannot clear if the person is not infected
        if (this->infectionStatus.hepcState == HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::NONE;
        this->infectionStatus.timeHEPCStateChanged = timestep;
    }

    void Person::updateLiver(const LiverState &ls, int timestep) {
        // nothing to do -- can only advance liver state
        if (ls <= this->infectionStatus.liverState) {
            return;
        }
        this->infectionStatus.liverState = ls;
        this->infectionStatus.timeLiverStateChanged = timestep;
    }

    void Person::updateBehavior(const BehaviorClassification &bc,
                                int timestep) {
        // nothing to do -- cannot go back to NEVER
        if (bc == this->behaviorDetails.behaviorClassification ||
            bc == BehaviorClassification::NEVER) {
            return;
        }
        // count for timeSinceActive if switching from active to non-active use
        if ((bc == BehaviorClassification::FORMER_NONINJECTION) ||
            (bc == BehaviorClassification::FORMER_INJECTION)) {
            this->behaviorDetails.timeLastActive = timestep;
        }
        // update the behavior classification
        this->behaviorDetails.behaviorClassification = bc;
    }

    LiverState Person::diagnoseLiver(int timestep) {
        // need to add functionality here
        this->infectionStatus.liverState = LiverState::F0;
        return this->infectionStatus.liverState;
    }

    HEPCState Person::diagnoseHEPC(int timestep) {
        // need to add functionality here
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        return this->infectionStatus.hepcState;
    }
} // namespace Person
