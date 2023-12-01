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
        {{"never", PregnancyState::NEVER},
         {"pregnant", PregnancyState::PREGNANT},
         {"postpartum", PregnancyState::POSTPARTUM}};

    Person::Person(std::vector<std::string> dataTableRow, int simCycle) {
        count++;
        if (dataTableRow.empty() || dataTableRow.size() < 29) {
            return;
        }

        this->id = stoi(dataTableRow[0]);

        this->sex = Person::sexMap[Utils::toLower(dataTableRow[1])];
        this->age = stod(dataTableRow[2]);

        this->isAlive = Utils::stobool(dataTableRow[3]);

        this->screeningDetails.timeOfLastScreening = stoi(dataTableRow[4]);
        this->screeningDetails.screeningFrequency = stoi(dataTableRow[5]);
        this->screeningDetails.interventionScreening =
            Utils::stobool(dataTableRow[6]);

        this->idStatus.timeIdentified = stoi(dataTableRow[7]);
        this->idStatus.identifiedAsPositiveInfection =
            Utils::stobool(dataTableRow[8]);

        this->infectionStatus.hepcState =
            Person::hepcStateMap[Utils::toLower(dataTableRow[9])];
        this->infectionStatus.timeHEPCStateChanged = stoi(dataTableRow[10]);
        this->infectionStatus.seropositivity = Utils::stobool(dataTableRow[11]);
        this->infectionStatus.liverState =
            Person::liverStateMap[Utils::toLower(dataTableRow[12])];
        this->infectionStatus.timeLiverStateChanged = stoi(dataTableRow[13]);

        this->stagingDetails.measuredLiverState =
            Person::measuredLiverStateMap[Utils::toLower(dataTableRow[14])];
        this->stagingDetails.timeOfLastStaging = stoi(dataTableRow[15]);

        this->behaviorDetails.behaviorClassification =
            Person::behaviorClassificationMap[Utils::toLower(dataTableRow[16])];
        this->behaviorDetails.timeLastActive = stoi(dataTableRow[17]);

        this->linkStatus.linkState =
            Person::linkageStateMap[Utils::toLower(dataTableRow[18])];
        this->linkStatus.timeOfLinkChange = stoi(dataTableRow[19]);
        this->linkStatus.linkType =
            Person::linkageTypeMap[Utils::toLower(dataTableRow[20])];

        this->overdose = Utils::stobool(dataTableRow[21]);

        this->incompleteTreatment = Utils::stobool(dataTableRow[22]);

        this->moudDetails.moudState =
            Person::moudMap[Utils::toLower(dataTableRow[23])];
        this->moudDetails.timeStartedMoud = stoi(dataTableRow[24]);

        this->pregnancyDetails.pregnancyState =
            Person::pregnancyStateMap[Utils::toLower(dataTableRow[25])];
        this->pregnancyDetails.timeOfPregnancyChange = stoi(dataTableRow[26]);
        this->pregnancyDetails.infantCount = stoi(dataTableRow[27]);
        this->pregnancyDetails.miscarriageCount = stoi(dataTableRow[28]);
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
