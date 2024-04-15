#include "Person.hpp"

namespace Person {
    int count = 0;

    std::map<std::string, HEPCState> Person::hepcStateMap = {
        {"none", HEPCState::NONE},
        {"acute", HEPCState::ACUTE},
        {"chronic", HEPCState::CHRONIC}};

    std::map<std::string, BehaviorClassification>
        Person::behaviorClassificationMap = {
            {"never", BehaviorClassification::NEVER},
            {"former_noninjection",
             BehaviorClassification::FORMER_NONINJECTION},
            {"former_injection", BehaviorClassification::FORMER_INJECTION},
            {"noninjection", BehaviorClassification::NONINJECTION},
            {"injection", BehaviorClassification::INJECTION}};

    std::map<std::string, LinkageType> Person::linkageTypeMap = {
        {"background", LinkageType::BACKGROUND},
        {"intervention", LinkageType::INTERVENTION}};

    std::map<std::string, LinkageState> Person::linkageStateMap = {
        {"never", LinkageState::NEVER},
        {"linked", LinkageState::LINKED},
        {"unlinked", LinkageState::UNLINKED}};

    std::map<std::string, FibrosisState> Person::fibrosisStateMap = {
        {"none", FibrosisState::NONE},    {"f0", FibrosisState::F0},
        {"f1", FibrosisState::F1},        {"f2", FibrosisState::F2},
        {"f3", FibrosisState::F3},        {"f4", FibrosisState::F4},
        {"decomp", FibrosisState::DECOMP}};

    std::map<std::string, MeasuredFibrosisState>
        Person::measuredFibrosisStateMap = {
            {"none", MeasuredFibrosisState::NONE},
            {"f01", MeasuredFibrosisState::F01},
            {"f23", MeasuredFibrosisState::F23},
            {"f4", MeasuredFibrosisState::F4},
            {"decomp", MeasuredFibrosisState::DECOMP}};

    std::map<std::string, HCCState> Person::hccStateMap = {
        {"none", HCCState::NONE},
        {"early", HCCState::EARLY},
        {"late", HCCState::LATE}};

    std::map<std::string, MOUD> Person::moudMap = {
        {"none", MOUD::NONE}, {"current", MOUD::CURRENT}, {"post", MOUD::POST}};

    std::map<std::string, Sex> Person::sexMap = {{"male", Sex::MALE},
                                                 {"female", Sex::FEMALE}};

    std::map<std::string, PregnancyState> Person::pregnancyStateMap = {
        {"none", PregnancyState::NONE},
        {"pregnant", PregnancyState::PREGNANT},
        {"postpartum", PregnancyState::POSTPARTUM}};

    std::map<HEPCState, std::string> Person::hepcStateEnumToStringMap = {
        {HEPCState::NONE, "none"},
        {HEPCState::ACUTE, "acute"},
        {HEPCState::CHRONIC, "chronic"}};

    std::map<BehaviorClassification, std::string>
        Person::behaviorClassificationEnumToStringMap = {
            {BehaviorClassification::NEVER, "never"},
            {BehaviorClassification::FORMER_NONINJECTION,
             "former_noninjection"},
            {BehaviorClassification::FORMER_INJECTION, "former_injection"},
            {BehaviorClassification::NONINJECTION, "noninjection"},
            {BehaviorClassification::INJECTION, "injection"}};

    std::map<LinkageType, std::string> Person::linkageTypeEnumToStringMap = {
        {LinkageType::BACKGROUND, "background"},
        {LinkageType::INTERVENTION, "intervention"}};

    std::map<LinkageState, std::string> Person::linkageStateEnumToStringMap = {
        {LinkageState::NEVER, "never"},
        {LinkageState::LINKED, "linked"},
        {LinkageState::UNLINKED, "unlinked"}};

    std::map<FibrosisState, std::string> Person::fibrosisStateEnumToStringMap =
        {{FibrosisState::NONE, "none"},    {FibrosisState::F0, "f0"},
         {FibrosisState::F1, "f1"},        {FibrosisState::F2, "f2"},
         {FibrosisState::F3, "f3"},        {FibrosisState::F4, "f4"},
         {FibrosisState::DECOMP, "decomp"}};

    std::map<MeasuredFibrosisState, std::string>
        Person::measuredFibrosisStateEnumToStringMap = {
            {MeasuredFibrosisState::NONE, "none"},
            {MeasuredFibrosisState::F01, "f01"},
            {MeasuredFibrosisState::F23, "f23"},
            {MeasuredFibrosisState::F4, "f4"},
            {MeasuredFibrosisState::DECOMP, "decomp"}};

    std::map<MOUD, std::string> Person::moudEnumToStringMap = {
        {MOUD::NONE, "none"}, {MOUD::CURRENT, "current"}, {MOUD::POST, "post"}};

    std::map<Sex, std::string> Person::sexEnumToStringMap = {
        {Sex::MALE, "male"}, {Sex::FEMALE, "female"}};

    std::map<PregnancyState, std::string>
        Person::pregnancyStateEnumToStringMap = {
            {PregnancyState::NONE, "none"},
            {PregnancyState::PREGNANT, "pregnant"},
            {PregnancyState::POSTPARTUM, "postpartum"}};

    Person::Person(Data::IDataTablePtr dataTableRow) {
        count++;

        if (dataTableRow->empty() || dataTableRow->ncols() < 29) {
            return;
        }

        this->id = stoi((*dataTableRow)["id"][0]);

        this->sex = Person::sexMap[Utils::toLower((*dataTableRow)["sex"][0])];
        this->age = stoul((*dataTableRow)["age"][0]);

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
        this->infectionStatus.fibrosisState =
            Person::fibrosisStateMap[Utils::toLower(
                (*dataTableRow)["fibrosisState"][0])];
        this->infectionStatus.timeFibrosisStateChanged =
            stoi((*dataTableRow)["timeFibrosisStateChanged"][0]);

        this->stagingDetails.measuredFibrosisState =
            Person::measuredFibrosisStateMap[Utils::toLower(
                (*dataTableRow)["measuredFibrosisState"][0])];
        this->stagingDetails.timeOfLastStaging =
            stoi((*dataTableRow)["timeOfLastStaging"][0]);

        this->behaviorDetails.behaviorClassification =
            Person::behaviorClassificationMap[Utils::toLower(
                (*dataTableRow)["drugBehavior"][0])];
        this->behaviorDetails.timeLastActive =
            stoul((*dataTableRow)["timeLastActiveDrugUse"][0]);

        this->linkStatus.linkState = Person::linkageStateMap[Utils::toLower(
            (*dataTableRow)["linkageState"][0])];
        this->linkStatus.timeOfLinkChange =
            stoi((*dataTableRow)["timeOfLinkChange"][0]);
        this->linkStatus.linkType = Person::linkageTypeMap[Utils::toLower(
            (*dataTableRow)["linkageType"][0])];

        this->overdose = Utils::stobool((*dataTableRow)["isOverdosed"][0]);

        this->treatmentDetails.incompleteTreatment =
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

    void Person::infect(int tstep) {
        // cannot be multiply infected
        if (this->infectionStatus.hepcState != HEPCState::NONE) {
            return;
        }
        this->infectionStatus.hepcState = HEPCState::ACUTE;
        this->infectionStatus.timeHEPCStateChanged = tstep;
        this->infectionStatus.seropositivity = true;

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
