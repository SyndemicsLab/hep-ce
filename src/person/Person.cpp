////////////////////////////////////////////////////////////////////////////////
// File: Person.cpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-02                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-21                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Person.hpp"
#include "PersonTableOperations.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace person {
class Person::PersonIMPL {
private:
    size_t id = 0;
    size_t _current_time = 0;

    int age = 0;
    Sex sex = Sex::MALE;
    bool is_alive = true;
    DeathReason death_reason = DeathReason::NA;
    BehaviorDetails behavior_details;
    HCVDetails hcv_details;
    HIVDetails hivDetails;
    HCCDetails hccDetails;
    int numOverdoses = 0;
    int treatmentWithdrawals = 0;
    int treatmentToxicReactions = 0;
    int completedTreatments = 0;
    int retreatments = 0;
    int svrs = 0;
    bool currentlyOverdosing = false;
    bool hccDiagnosed = false;
    MOUDDetails moudDetails;
    PregnancyDetails pregnancyDetails;
    StagingDetails stagingDetails;
    std::unordered_map<InfectionType, LinkageDetails> linkStatus;
    std::unordered_map<InfectionType, ScreeningDetails> screeningDetails;
    std::unordered_map<InfectionType, TreatmentDetails> treatmentDetails;
    cost::CostTracker costs;
    bool boomerClassification = false;
    std::vector<Child> children = {};

    // utility
    utility::UtilityTracker utility_tracker;
    LifetimeUtility lifetime_utility;

    // life span tracking
    int life_span = 0;
    double discounted_life_span = 0;

    int UpdateTimers() {
        this->_current_time++;
        if (GetBehavior() == person::Behavior::NONINJECTION ||
            GetBehavior() == person::Behavior::INJECTION) {
            this->behavior_details.timeLastActive = this->_current_time;
        }
        if (GetMoudState() == person::MOUD::CURRENT) {
            this->moudDetails.totalMOUDMonths++;
        }
        if (GetHIV() == person::HIV::LOUN || GetHIV() == person::HIV::LOSU) {
            this->hivDetails.lowCD4MonthsCount++;
        }
        this->moudDetails.currentStateConcurrentMonths++;
        return 0;
    }

    /// @brief Setter for MOUD State
    /// @param moud PersonIMPL's new MOUD state
    void SetMoudState(MOUD moud) {
        if (moud == person::MOUD::CURRENT) {
            this->moudDetails.timeStartedMoud = this->_current_time;
        }
        this->moudDetails.currentStateConcurrentMonths = 0;
        this->moudDetails.moudState = moud;
    }

    int CalculateTimeSince(int lasttime) const {
        if (lasttime <= -1) {
            return GetCurrentTimestep();
        }
        return GetCurrentTimestep() - lasttime;
    }

public:
    PersonIMPL() {
        for (InfectionType i = InfectionType::HCV; i < InfectionType::COUNT;
             ++i) {
            this->linkStatus[i] = LinkageDetails();
            this->screeningDetails[i] = ScreeningDetails();
            this->treatmentDetails[i] = TreatmentDetails();
        }
    }
    int
    CreatePersonFromTable(int id,
                          std::shared_ptr<datamanagement::DataManagerBase> dm) {
        this->id = id;
        if (dm == nullptr) {
            // Let default values stay
            return 0;
        }
        std::stringstream query;
        query << "SELECT " << person::POPULATION_HEADERS;
        query << "FROM population ";
        query << "WHERE id = " << std::to_string(id);

        struct person_select storage;
        std::string error;
        int rc = dm->SelectCustomCallback(query.str(), person_callback,
                                          &storage, error);
        if (rc != 0) {
// only log on main because during debug we don't always expect tables
#ifdef NDEBUG
            spdlog::get("main")->error(
                "Issue Selecting person from population table! Error "
                "Message: {}",
                error);
#endif
            return rc;
        }

        sex = storage.sex;
        SetAge(storage.age);
        is_alive = storage.is_alive;
        screeningDetails[InfectionType::HCV].identified = storage.identifiedHCV;
        screeningDetails[InfectionType::HCV].timeIdentified =
            storage.timeInfectionIdentified;
        hcv_details.hcv = storage.hcv;
        hcv_details.fibrosisState = storage.fibrosisState;
        hcv_details.isGenotypeThree = storage.isGenotypeThree;
        hcv_details.seropositive = storage.seropositive;
        hcv_details.timeChanged = storage.timeHCVChanged;
        hcv_details.timeFibrosisStateChanged = storage.timeFibrosisStateChanged;
        behavior_details.behavior = storage.drugBehavior;
        behavior_details.timeLastActive = storage.timeLastActiveDrugUse;
        linkStatus[InfectionType::HCV].linkState = storage.linkageState;
        linkStatus[InfectionType::HCV].timeOfLinkChange =
            storage.timeOfLinkChange;
        linkStatus[InfectionType::HCV].linkType = storage.linkageType;
        linkStatus[InfectionType::HCV].linkCount = storage.linkCount;
        stagingDetails.measuredFibrosisState = storage.measuredFibrosisState;
        stagingDetails.timeOfLastStaging = storage.timeOfLastStaging;
        screeningDetails[InfectionType::HCV].timeOfLastScreening =
            storage.timeOfLastScreening;
        screeningDetails[InfectionType::HCV].numABTests = storage.numABTests;
        screeningDetails[InfectionType::HCV].numRNATests = storage.numRNATests;
        hcv_details.timesInfected = storage.timesInfected;
        hcv_details.timesAcuteCleared = storage.timesAcuteCleared;
        treatmentDetails[InfectionType::HCV].initiatedTreatment =
            storage.initiatedTreatment;
        treatmentDetails[InfectionType::HCV].timeOfTreatmentInitiation =
            storage.timeOfTreatmentInitiation;
        treatmentDetails[InfectionType::HCV].retreatment = storage.retreatment;
        treatmentDetails[InfectionType::HCV].numberOfTreatmentStarts =
            storage.numberOfTreatmentStarts;
        lifetime_utility.min_util = storage.min_utility;
        lifetime_utility.mult_util = storage.mult_utility;
        lifetime_utility.discount_min_util = storage.discount_min_utility;
        lifetime_utility.discount_mult_util = storage.discount_mult_utility;
        this->utility_tracker.SetUtility(storage.behaviorUtility,
                                         utility::UtilityCategory::BEHAVIOR);
        this->utility_tracker.SetUtility(storage.liverUtility,
                                         utility::UtilityCategory::LIVER);
        this->utility_tracker.SetUtility(storage.treatmentUtility,
                                         utility::UtilityCategory::TREATMENT);
        this->utility_tracker.SetUtility(storage.backgroundUtility,
                                         utility::UtilityCategory::BACKGROUND);
        this->utility_tracker.SetUtility(storage.hivUtility,
                                         utility::UtilityCategory::HIV);
        return 0;
    }

    /// @brief End a PersonIMPL's life and set final Age
    void Die(DeathReason death_reason) {
        this->is_alive = false;
        SetDeathReason(death_reason);
    }

    /// @brief increase a person's Age
    void Grow() {
        UpdateTimers();
        SetAge(GetAge() + 1);
        ++(this->life_span);
    }

    /// @brief Infect the person
    void InfectHCV() {
        // cannot be multiply infected
        if (this->hcv_details.hcv != HCV::NONE) {
            return;
        }
        this->hcv_details.hcv = HCV::ACUTE;
        this->hcv_details.timeChanged = this->_current_time;
        this->hcv_details.seropositive = true;
        this->hcv_details.timesInfected++;

        // once infected, immediately enter F0
        if (this->hcv_details.fibrosisState == FibrosisState::NONE) {
            UpdateTrueFibrosis(FibrosisState::F0);
        }
    }

    /// @brief Clear of HCV infection
    void ClearHCV(bool acute) {
        this->hcv_details.hcv = HCV::NONE;
        this->hcv_details.timeChanged = this->_current_time;
        if (acute) {
            this->AddAcuteHCVClearance();
        }
    }

    /// @brief infect the person with HIV
    void InfectHIV() {
        // cannot be multiply infected
        if (this->hivDetails.hiv != HIV::NONE) {
            return;
        }
        // infected start as high CD4, unsuppressed infection
        this->hivDetails.hiv = HIV::HIUN;
        this->hivDetails.timeChanged = this->_current_time;
    }

    int LoadICValues(int id, std::vector<std::string> icValues) {
        this->id = id;
        SetAge(std::stoi(icValues[1]));
        this->sex = static_cast<person::Sex>(std::stoi(icValues[2]));
        SetBehavior(static_cast<person::Behavior>(std::stoi(icValues[3])));
        behavior_details.timeLastActive = std::stoi(icValues[4]);
        bool sero = (std::stoi(icValues[5]) == 1) ? true : false;
        SetSeropositivity(sero);
        bool geno = (std::stoi(icValues[6]) == 1) ? true : false;
        SetGenotypeThree(geno);
        this->hcv_details.fibrosisState =
            static_cast<person::FibrosisState>(std::stoi(icValues[7]));
        if (std::stoi(icValues[8]) == 1) {
            this->screeningDetails[InfectionType::HCV].identified = true;
            this->screeningDetails[InfectionType::HCV].antibodyPositive = true;
        }
        linkStatus[InfectionType::HCV].linkState =
            static_cast<person::LinkageState>(std::stoi(icValues[9]));
        hcv_details.hcv = static_cast<person::HCV>(std::stoi(icValues[10]));

        if (icValues.size() == 11) {
            return 0;
        }

        pregnancyDetails.pregnancyState =
            static_cast<person::PregnancyState>(std::stoi(icValues[11]));
        if (pregnancyDetails.pregnancyState ==
            person::PregnancyState::PREGNANT) {
            pregnancyDetails.timeOfPregnancyChange = 0;
        }
        return 0;
    }

    /// @brief Update Opioid Use Behavior Classification
    /// @param bc The intended resultant Behavior
    /// @param timestep Current simulation timestep
    void SetBehavior(const Behavior &bc) {
        // nothing to do -- cannot go back to NEVER
        if (bc == this->behavior_details.behavior || bc == Behavior::NEVER) {
            return;
        }
        if ((bc == person::Behavior::NONINJECTION ||
             bc == person::Behavior::INJECTION) &&
            (this->behavior_details.behavior == Behavior::NEVER ||
             this->behavior_details.behavior == Behavior::FORMER_INJECTION ||
             this->behavior_details.behavior ==
                 Behavior::FORMER_NONINJECTION)) {
            this->behavior_details.timeLastActive = this->_current_time;
        }
        this->behavior_details.behavior = bc;
    }

    /// @brief Diagnose somebody's fibrosis
    /// @return Fibrosis state that is diagnosed
    MeasuredFibrosisState DiagnoseFibrosis(MeasuredFibrosisState &data) {
        // need to add functionality here
        this->stagingDetails.measuredFibrosisState = data;
        this->stagingDetails.timeOfLastStaging = this->_current_time;
        return this->stagingDetails.measuredFibrosisState;
    }

    /// @brief Add an acute clearance to the running count
    void AddAcuteHCVClearance() { this->hcv_details.timesAcuteCleared++; };

    void AddWithdrawal() { this->treatmentWithdrawals++; }

    void AddToxicReaction() { this->treatmentToxicReactions++; }

    void AddCompletedTreatment() { this->completedTreatments++; }

    void AddSVR() { this->svrs++; }

    /// @brief Mark somebody as having been screened this timestep
    void MarkScreened(InfectionType it) {
        this->screeningDetails[it].timeOfLastScreening = this->_current_time;
    }

    /// @brief
    void AddAbScreen(InfectionType it) {
        this->screeningDetails[it].timeOfLastScreening = this->_current_time;
        this->screeningDetails[it].numABTests++;
    }

    /// @brief
    void AddRnaScreen(InfectionType it) {
        this->screeningDetails[it].timeOfLastScreening = this->_current_time;
        this->screeningDetails[it].numRNATests++;
    }

    /// @brief Reset a PersonIMPL's Link State to Unlinked
    /// @param timestep Timestep during which the PersonIMPL is Unlinked
    void Unlink(InfectionType it) {
        this->linkStatus[it].linkState = LinkageState::UNLINKED;
        this->linkStatus[it].timeOfLinkChange = this->_current_time;
    }

    /// @brief Reset a PersonIMPL's Link State to Linked
    /// @param timestep Timestep during which the PersonIMPL is Linked
    /// @param linkType The linkage type the PersonIMPL recieves
    void Link(LinkageType linkType, InfectionType it) {
        this->linkStatus[it].linkState = LinkageState::LINKED;
        this->linkStatus[it].timeOfLinkChange = this->_current_time;
        this->linkStatus[it].linkType = linkType;
        this->linkStatus[it].linkCount++;
    }

    void EndTreatment(InfectionType it) {
        this->treatmentDetails[it].initiatedTreatment = false;
        this->treatmentDetails[it].retreatment = false;
    }

    void Diagnose(InfectionType it) {
        this->screeningDetails[it].identified = true;
        this->screeningDetails[it].timeIdentified = this->_current_time;
        this->screeningDetails[it].antibodyPositive = true;
    }

    void ClearDiagnosis(InfectionType it) {
        this->screeningDetails[it].identified = false;
    }

    /// @brief Add a cost to the person's CostTracker object
    /// @param cost The cost to be added
    /// @param timestep The timestep during which the cost was accrued
    void AddCost(double base_cost, double discount_cost,
                 cost::CostCategory category) {
        this->costs.AddCost(base_cost, discount_cost, category);
    }

    /// @brief Flips the person's overdose state
    void ToggleOverdose() {
        this->currentlyOverdosing = !this->currentlyOverdosing;
        if (this->currentlyOverdosing) {
            this->numOverdoses++;
        }
    }

    /// @brief Increment the tracker for number of infant HCV exposures
    void AddInfantExposure() { this->pregnancyDetails.numHCVExposures++; }

    /// @brief Increment the tracker for number of miscarriages and end person's
    /// pregnancy
    void Miscarry() {
        this->pregnancyDetails.numMiscarriages++;
        this->pregnancyDetails.timeOfPregnancyChange = this->_current_time;
        this->pregnancyDetails.pregnancyState = person::PregnancyState::NONE;
    }

    void EndPostpartum() {
        this->pregnancyDetails.timeOfPregnancyChange = this->_current_time;
        this->pregnancyDetails.pregnancyState = person::PregnancyState::NONE;
    }

    void Impregnate() {
        if (GetSex() == person::Sex::MALE) {
            return;
        }
        this->pregnancyDetails.count++;
        this->pregnancyDetails.timeOfPregnancyChange = this->_current_time;
        this->pregnancyDetails.pregnancyState =
            person::PregnancyState::PREGNANT;
    }

    void Stillbirth() {
        if (GetSex() == person::Sex::MALE) {
            return;
        }
        this->pregnancyDetails.timeOfPregnancyChange = this->_current_time;
        this->pregnancyDetails.pregnancyState =
            person::PregnancyState::POSTPARTUM;
        this->pregnancyDetails.numMiscarriages++;
    }

    int GetInfantHCVExposures() const {
        return this->pregnancyDetails.numHCVExposures;
    }

    int GetInfantHCVInfections() const {
        return this->pregnancyDetails.numHCVInfections;
    }

    int GetInfantHCVTests() const { return this->pregnancyDetails.numHCVTests; }

    int GetPregnancyCount() const { return this->pregnancyDetails.count; }

    void AddChild(HCV hcv, bool test) {
        person::Child child;
        child.hcv = hcv;
        if (hcv != HCV::NONE) {
            this->pregnancyDetails.numHCVInfections++;
        }
        child.tested = test;
        if (test) {
            this->pregnancyDetails.numHCVTests++;
        }
        this->children.push_back(child);
    }

    void TransitionMOUD() {
        if (GetBehavior() == person::Behavior::NEVER) {
            return;
        }
        person::MOUD current = GetMoudState();
        if (current == person::MOUD::CURRENT) {
            SetMoudState(person::MOUD::POST);
        } else if (current == person::MOUD::POST) {
            SetMoudState(person::MOUD::NONE);
        } else if (current == person::MOUD::NONE) {
            SetMoudState(person::MOUD::CURRENT);
        }
    }

    ////////////// CHECKS /////////////////

    bool IsAlive() const { return this->is_alive; }

    /// @brief Getter for whether PersonIMPL experienced fibtest two this
    /// cycle
    /// @return value of hadSecondTest
    bool HadSecondScreeningTest() const {
        return this->stagingDetails.hadSecondTest;
    }

    /// @brief Getter for Identification Status
    /// @return Boolean Describing Indentified as Positive Status
    bool IsIdentifiedAsInfected(InfectionType it) const {
        return this->screeningDetails.at(it).identified;
    }

    /// @brief Getter for whether PersonIMPL has initiated treatment
    /// @return Boolean true if PersonIMPL has initiated treatment, false
    /// otherwise
    bool HasInitiatedTreatment(InfectionType it) const {
        return this->treatmentDetails.at(it).initiatedTreatment;
    }

    bool IsInRetreatment(InfectionType it) const {
        return this->treatmentDetails.at(it).retreatment;
    }
    /// @brief Getter for whether PersonIMPL is genotype three
    /// @return True if genotype three, false otherwise
    bool IsGenotypeThree() const { return this->hcv_details.isGenotypeThree; }
    bool IsBoomer() const { return this->boomerClassification; }

    bool IsCirrhotic() {
        if (this->GetTrueFibrosisState() == FibrosisState::F4 ||
            this->GetTrueFibrosisState() == FibrosisState::DECOMP) {
            return true;
        }
        return false;
    }
    ////////////// CHECKS /////////////////
    ////////////// GETTERS ////////////////
    int GetID() const { return this->id; }

    int GetCurrentTimestep() const { return this->_current_time; }

    int GetAge() const { return this->age; }

    /// @brief Getter for the number of HCV infections experienced by
    /// PersonIMPL
    /// @return Number of HCV infections experienced by PersonIMPL
    int GetTimesHCVInfected() const { return this->hcv_details.timesInfected; }

    /// @brief Get the running total of clearances for PersonIMPL
    int GetAcuteHCVClearances() const {
        return this->hcv_details.timesAcuteCleared;
    };

    int GetWithdrawals() const { return this->treatmentWithdrawals; }

    int GetToxicReactions() const { return this->treatmentToxicReactions; }

    int GetCompletedTreatments() const { return this->completedTreatments; }

    int GetRetreatments() const { return this->retreatments; }

    int GetSVRs() const { return this->svrs; }

    /// @brief
    int GetNumberOfABTests(InfectionType it) const {
        return this->screeningDetails.at(it).numABTests;
    }

    /// @brief
    int GetNumberOfRNATests(InfectionType it) const {
        return this->screeningDetails.at(it).numRNATests;
    }

    /// @brief Getter for the Time Since the Last Screening
    /// @return The Time Since the Last Screening
    int GetTimeOfLastScreening(InfectionType it) const {
        return this->screeningDetails.at(it).timeOfLastScreening;
    }

    /// @brief Getter for the person's overdose state
    /// @return Boolean representing overdose or not
    bool GetCurrentlyOverdosing() const { return this->currentlyOverdosing; }

    int GetNumberOfOverdoses() const { return this->numOverdoses; }

    DeathReason GetDeathReason() const { return this->death_reason; }

    /// @brief Getter for the Fibrosis State
    /// @return The Current Fibrosis State
    FibrosisState GetTrueFibrosisState() const {
        return this->hcv_details.fibrosisState;
    }

    /// @brief Getter for the HCV State
    /// @return HCV State
    HCV GetHCV() const { return this->hcv_details.hcv; }

    /// @brief Getter for Behavior Classification
    /// @return Behavior Classification
    Behavior GetBehavior() const { return this->behavior_details.behavior; }

    /// @brief Getter for time since active drug use
    /// @return Time since the person left an active drug use state
    int GetTimeBehaviorChange() const {
        return this->behavior_details.timeLastActive;
    }

    /// @brief Getter for timestep in which HCV last changed
    /// @return Timestep that HCV last changed
    int GetTimeHCVChanged() const { return this->hcv_details.timeChanged; }

    /// @brief Getter for timestep in which HIV last changed
    /// @return Timestep that HIV last changed
    int GetTimeHIVChanged() const { return this->hivDetails.timeChanged; }

    /// @brief Getter for Time since Fibrosis State Change
    /// @return Time Since Fibrosis State Change
    int GetTimeTrueFibrosisStateChanged() const {
        return this->hcv_details.timeFibrosisStateChanged;
    }

    /// @brief Getter for Seropositive
    /// @return Seropositive
    bool GetSeropositivity() const { return this->hcv_details.seropositive; }

    int GetTimeHCVIdentified() const {
        return this->screeningDetails.at(InfectionType::HCV).timeIdentified;
    }

    /// @brief Getter for Link State
    /// @return Link State
    LinkageState GetLinkState(InfectionType it) const {
        return this->linkStatus.at(it).linkState;
    }

    /// @brief Getter for Timestep in which linkage changed
    /// @return Time Link Change
    int GetTimeOfLinkChange(InfectionType it) const {
        return this->linkStatus.at(it).timeOfLinkChange;
    }

    /// @brief Getter for link count
    /// @return Number of times PersonIMPL has linked to care
    int GetLinkCount(InfectionType it) const {
        return this->linkStatus.at(it).linkCount;
    }

    void SetLinkageType(LinkageType linkType, InfectionType it) {
        this->linkStatus[it].linkType = linkType;
    }

    /// @brief Getter for Linkage Type
    /// @return Linkage Type
    LinkageType GetLinkageType(InfectionType it) const {
        return this->linkStatus.at(it).linkType;
    }

    /// @brief Getter for the number of timesteps PersonIMPL has been in
    /// treatment
    /// @return Integer number of timesteps spent in treatment
    int GetTimeOfTreatmentInitiation(InfectionType it) const {
        return this->treatmentDetails.at(it).timeOfTreatmentInitiation;
    }

    /// @brief Getter for pregnancy status
    /// @return Pregnancy State
    PregnancyState GetPregnancyState() const {
        return this->pregnancyDetails.pregnancyState;
    }

    /// @brief Getter for timestep in which last pregnancy change happened
    /// @return Time pregnancy state last changed
    int GetTimeOfPregnancyChange() const {
        return this->pregnancyDetails.timeOfPregnancyChange;
    }

    int GetTimeSincePregnancyChange() const {
        return CalculateTimeSince(GetTimeOfPregnancyChange());
    }

    int GetTimeSinceLastScreening(InfectionType it) const {
        return CalculateTimeSince(GetTimeOfLastScreening(it));
    }

    int GetTimeSinceHCVChanged() const {
        return CalculateTimeSince(GetTimeHCVChanged());
    }

    int GetTimeSinceHIVChanged() const {
        return CalculateTimeSince(GetTimeHIVChanged());
    }

    int GetTimeSinceLinkChange(InfectionType it) const {
        return CalculateTimeSince(GetTimeOfLinkChange(it));
    }

    int GetTimeSinceTreatmentInitiation(InfectionType it) const {
        return CalculateTimeSince(GetTimeOfTreatmentInitiation(it));
    }

    int GetTimeSinceFibrosisStaging() const {
        return CalculateTimeSince(GetTimeOfFibrosisStaging());
    }

    std::vector<Child> GetChildren() const { return this->children; }

    /// @brief Getter for number of miscarriages
    /// @return Number of miscarriages this person has experienced
    int GetNumMiscarriages() const {
        return this->pregnancyDetails.numMiscarriages;
    }

    /// @brief Getter for timestep in which the last fibrosis
    /// staging test happened
    /// @return Timestep of person's last fibrosis staging
    int GetTimeOfFibrosisStaging() const {
        return this->stagingDetails.timeOfLastStaging;
    }

    /// @brief Getter for MOUD State
    /// @return MOUD State
    MOUD GetMoudState() const { return this->moudDetails.moudState; }

    /// @brief Getter for measured fibrosis state
    /// @return Measured Fibrosis State
    MeasuredFibrosisState GetMeasuredFibrosisState() const {
        return this->stagingDetails.measuredFibrosisState;
    }
    /// @brief Getter for timestep in which MOUD was started
    /// @return Time spent on MOUD
    int GetTimeStartedMoud() const { return this->moudDetails.timeStartedMoud; }

    /// @brief Get Person's undiscounted life span
    /// @return Undiscounted life span, in months
    int GetLifeSpan() const { return this->life_span; }

    /// @brief Get Person's discounted life span
    /// @return Discounted life span, in months
    double GetDiscountedLifeSpan() const { return this->discounted_life_span; }

    /// @brief Add discounted life span to person
    /// @param
    void AddDiscountedLifeSpan(double discounted_life) {
        this->discounted_life_span += discounted_life;
    }

    /// @brief Getter for the person's sex
    /// @return PersonIMPL's sex
    Sex GetSex() const { return this->sex; }

    /// @brief Getter for the person's stratified utilities
    /// @return PersonIMPL's stratified utilities
    LifetimeUtility GetTotalUtility() const { return this->lifetime_utility; }

    /// @brief Getter for the person's utilities broken down by category
    /// @return PersonIMPL's category-specific utilities
    std::unordered_map<utility::UtilityCategory, double>
    GetCurrentUtilities() const {
        return this->utility_tracker.GetRawUtilities();
    }

    /// @brief Getter for the PersonIMPL's costs
    /// @return cost::CostTracker containing this person's costs
    std::unordered_map<cost::CostCategory, std::pair<double, double>>
    GetCosts() const {
        return this->costs.GetCosts();
    }

    std::pair<double, double> GetCostTotals() const {
        return this->costs.GetTotals();
    }

    HCVDetails GetHCVDetails() const { return this->hcv_details; }
    HIVDetails GetHIVDetails() const { return this->hivDetails; }
    HCCDetails GetHCCDetails() const { return this->hccDetails; }

    BehaviorDetails GetBehaviorDetails() const {
        return this->behavior_details;
    }

    LinkageDetails GetLinkStatus(InfectionType it) const {
        return this->linkStatus.at(it);
    }

    MOUDDetails GetMOUDDetails() const { return this->moudDetails; }

    HIV GetHIV() const { return this->hivDetails.hiv; }

    PregnancyDetails GetPregnancyDetails() const {
        return this->pregnancyDetails;
    }

    StagingDetails GetFibrosisStagingDetails() const {
        return this->stagingDetails;
    }

    ScreeningDetails GetScreeningDetails(InfectionType it) const {
        return this->screeningDetails.at(it);
    }

    bool CheckAntibodyPositive(InfectionType it) const {
        return this->screeningDetails.at(it).antibodyPositive;
    }

    void SetAntibodyPositive(bool result, InfectionType it) {
        this->screeningDetails[it].antibodyPositive = result;
    }

    TreatmentDetails GetTreatmentDetails(InfectionType it) const {
        return this->treatmentDetails.at(it);
    }
    int GetNumberOfTreatmentStarts(InfectionType it) const {
        return this->treatmentDetails.at(it).numberOfTreatmentStarts;
    }

    std::string GetPersonDataString() const {
        std::stringstream data;
        data << GetAge() << "," << GetSex() << "," << GetBehavior() << ","
             << GetTimeBehaviorChange() << "," << GetSeropositivity() << ","
             << IsGenotypeThree() << "," << GetTrueFibrosisState() << ","
             << IsIdentifiedAsInfected(InfectionType::HCV) << ","
             << GetLinkState(InfectionType::HCV) << "," << IsAlive() << ","
             << GetDeathReason() << "," << GetTimeHCVIdentified() << ","
             << GetHCV() << "," << GetTimeHCVChanged() << ","
             << GetTimeTrueFibrosisStateChanged() << ","
             << GetTimeBehaviorChange() << ","
             << GetTimeOfLinkChange(InfectionType::HCV) << ","
             << GetLinkageType(InfectionType::HCV) << ","
             << GetLinkCount(InfectionType::HCV) << ","
             << GetMeasuredFibrosisState() << "," << GetTimeOfFibrosisStaging()
             << "," << GetTimeOfLastScreening(InfectionType::HCV) << ","
             << GetNumberOfABTests(InfectionType::HCV) << ","
             << GetNumberOfRNATests(InfectionType::HCV) << ","
             << GetTimesHCVInfected() << "," << GetAcuteHCVClearances() << ","
             << std::boolalpha
             << GetTreatmentDetails(InfectionType::HCV).initiatedTreatment
             << "," << GetTimeOfTreatmentInitiation(InfectionType::HCV) << ","
             << std::to_string(GetTotalUtility().min_util) << ","
             << std::to_string(GetTotalUtility().mult_util);
        return data.str();
    }

    //////////////////// GETTERS ////////////////////
    //////////////////// SETTERS ////////////////////

    void SetAge(int age) { this->age = age; }

    /// @brief Set whether the person experienced fibtest two this cycle
    /// @param state New value of hadSecondTest
    void GiveSecondScreeningTest(bool state) {
        this->stagingDetails.hadSecondTest = state;
    }

    /// @brief Set the Seropositive value
    /// @param seropositive Seropositive status to set
    void SetSeropositivity(bool seropositive) {
        this->hcv_details.seropositive = seropositive;
    }

    void SetDeathReason(DeathReason death_reason) {
        this->death_reason = death_reason;
    }

    /// @brief Set HEPC State -- used to change to chronic infection
    /// @param New HEPC State
    void SetHCV(HCV hcv) {
        this->hcv_details.hcv = hcv;
        this->hcv_details.timeChanged = this->_current_time;
    }

    /// @brief Set HIV infection state -- used to change between states of
    /// suppression and CD4 count
    /// @param New HIV infection state
    void SetHIV(HIV hiv) {
        if (hiv == this->hivDetails.hiv) {
            return;
        }
        this->hivDetails.hiv = hiv;
        this->hivDetails.timeChanged = this->_current_time;
    }

    int GetLowCD4MonthCount() const {
        return this->hivDetails.lowCD4MonthsCount;
    }

    /// @brief Setter for PersonIMPL's treatment initiation state
    /// @param incompleteTreatment Boolean value for initiated treatment
    /// state to be set
    void InitiateTreatment(InfectionType it) {
        TreatmentDetails &td = this->treatmentDetails[it];
        // cannot continue being treated if already in retreatment
        if (td.initiatedTreatment && td.retreatment) {
            return;
        } else if (td.initiatedTreatment) {
            td.retreatment = true;
            retreatments++;
        } else {
            td.initiatedTreatment = true;
        }
        // treatment starts counts treatment regimens
        td.numberOfTreatmentStarts++;
        td.timeOfTreatmentInitiation = this->_current_time;
    }

    /// @brief Setter for Pregnancy State
    /// @param state PersonIMPL's pregnancy State
    void SetPregnancyState(PregnancyState state) {
        this->pregnancyDetails.timeOfPregnancyChange = this->_current_time;
        this->pregnancyDetails.pregnancyState = state;
    }

    /// @brief Add miscarriages to the count
    /// @param miscarriages Number of miscarriages to add
    void SetNumMiscarriages(int miscarriages) {
        this->pregnancyDetails.numMiscarriages += miscarriages;
    }

    /// @brief Set PersonIMPL's measured fibrosis state
    /// @param state
    void UpdateTrueFibrosis(FibrosisState state) {
        this->hcv_details.timeFibrosisStateChanged = this->_current_time;
        this->hcv_details.fibrosisState = state;
    }

    /// @brief Setter for whether PersonIMPL is genotype three
    /// @param genotype True if infection is genotype three, false
    /// otherwise
    void SetGenotypeThree(bool genotype) {
        this->hcv_details.isGenotypeThree = genotype;
    }

    void AccumulateTotalUtility(std::pair<double, double> util,
                                std::pair<double, double> discount_util) {
        this->lifetime_utility.min_util += util.first;
        this->lifetime_utility.mult_util += util.second;
        this->lifetime_utility.discount_min_util += discount_util.first;
        this->lifetime_utility.discount_mult_util += discount_util.second;
    }

    /// @brief Get Person's utility values
    /// @return min, mult utility
    std::pair<double, double> GetUtility() const {
        return this->utility_tracker.GetUtilities();
    }

    /// @brief Set a value for a person's utility
    /// @param util The value of the utility to be updated, bounded by
    /// 0, 1
    /// @param category The category of the utility to be updated
    void SetUtility(double util, utility::UtilityCategory category) {
        this->utility_tracker.SetUtility(util, category);
    }

    void SetBoomer(bool status) { this->boomerClassification = status; }

    void DevelopHCC(HCCState state) {
        HCCState current = this->hccDetails.hccState;
        switch (current) {
        case HCCState::NONE:
            this->hccDetails.hccState = HCCState::EARLY;
            break;
        case HCCState::EARLY:
            this->hccDetails.hccState = HCCState::LATE;
            break;
        default:
            break;
        }
    }
    HCCState GetHCCState() const { return this->hccDetails.hccState; }
    void DiagnoseHCC() { hccDiagnosed = true; }
    bool IsDiagnosedWithHCC() const { return hccDiagnosed; }

    //////////////////// SETTERS ////////////////////
};

int count = 0;

std::string sqlQuery = "";
std::ostream &operator<<(std::ostream &os, const Person &person) {
    os << "sex: " << person.GetSex() << std::endl;
    os << "alive: " << std::boolalpha << person.IsAlive()
       << "from: " << person.GetDeathReason() << std::endl;
    os << person.GetHCVDetails() << std::endl;
    os << person.GetBehaviorDetails() << std::endl;
    os << person.GetLinkStatus(InfectionType::HCV) << std::endl;
    os << "Overdoses: " << person.GetNumberOfOverdoses() << std::endl;
    os << person.GetMOUDDetails() << std::endl;
    os << person.GetPregnancyDetails() << std::endl;
    os << person.GetFibrosisStagingDetails() << std::endl;
    os << person.GetScreeningDetails(InfectionType::HCV) << std::endl;
    os << person.GetTreatmentDetails(InfectionType::HCV) << std::endl;
    os << person.GetTotalUtility() << std::endl;
    // os << person.getCosts() << std::endl;
    os << "Is a Baby Boomer: " << std::boolalpha << person.IsBoomer()
       << std::endl;
    os << "Age: " << person.GetAge() << std::endl;
    return os;
};

/// Base Defined Pass Through Wrappers
Person::Person() {
    pImplPERSON = std::make_unique<person::Person::PersonIMPL>();
}

Person::~Person() = default;

Person::Person(Person &&p) noexcept = default;
Person &Person::operator=(Person &&) noexcept = default;

int Person::CreatePersonFromTable(
    int id, std::shared_ptr<datamanagement::DataManagerBase> dm) {
    return pImplPERSON->CreatePersonFromTable(id, dm);
}

int Person::Grow() {
    pImplPERSON->Grow();
    return 0;
}
int Person::Die(DeathReason death_reason) {
    pImplPERSON->Die(death_reason);
    return 0;
}
int Person::InfectHCV() {
    pImplPERSON->InfectHCV();
    return 0;
}
int Person::ClearHCV(bool acute) {
    pImplPERSON->ClearHCV(acute);
    return 0;
}
int Person::SetBehavior(Behavior bc) {
    pImplPERSON->SetBehavior(bc);
    return 0;
}
int Person::DiagnoseFibrosis(MeasuredFibrosisState data) {
    data = pImplPERSON->DiagnoseFibrosis(data);
    return 0;
}
int Person::LoadICValues(int id, std::vector<std::string> icValues) {
    pImplPERSON->LoadICValues(id, icValues);
    return 0;
}
int Person::AddAcuteHCVClearance() {
    pImplPERSON->AddAcuteHCVClearance();
    return 0;
}
int Person::AddWithdrawal() {
    pImplPERSON->AddWithdrawal();
    return 0;
}
int Person::AddToxicReaction() {
    pImplPERSON->AddToxicReaction();
    return 0;
}
int Person::AddCompletedTreatment() {
    pImplPERSON->AddCompletedTreatment();
    return 0;
}
int Person::AddSVR() {
    pImplPERSON->AddSVR();
    return 0;
}
int Person::MarkScreened(InfectionType it) {
    pImplPERSON->MarkScreened(it);
    return 0;
}
int Person::AddAbScreen(InfectionType it) {
    pImplPERSON->AddAbScreen(it);
    return 0;
}
int Person::AddRnaScreen(InfectionType it) {
    pImplPERSON->AddRnaScreen(it);
    return 0;
}
int Person::Unlink(InfectionType it) {
    pImplPERSON->Unlink(it);
    return 0;
}
int Person::Link(LinkageType linkType, InfectionType it) {
    pImplPERSON->Link(linkType, it);
    return 0;
}
int Person::EndTreatment(InfectionType it) {
    pImplPERSON->EndTreatment(it);
    return 0;
}
void Person::ClearDiagnosis(InfectionType it) {
    return pImplPERSON->ClearDiagnosis(it);
}
int Person::AddCost(double base_cost, double discount_cost,
                    cost::CostCategory category) {
    pImplPERSON->AddCost(base_cost, discount_cost, category);
    return 0;
}
int Person::ToggleOverdose() {
    pImplPERSON->ToggleOverdose();
    return 0;
}
int Person::Miscarry() {
    pImplPERSON->Miscarry();
    return 0;
}
int Person::EndPostpartum() {
    pImplPERSON->EndPostpartum();
    return 0;
}
int Person::Impregnate() {
    pImplPERSON->Impregnate();
    return 0;
}
int Person::Stillbirth() {
    pImplPERSON->Stillbirth();
    return 0;
}
int Person::AddChild(HCV hcv, bool test) {
    pImplPERSON->AddChild(hcv, test);
    return 0;
}
// Checks
bool Person::IsAlive() const { return pImplPERSON->IsAlive(); }
bool Person::HadSecondScreeningTest() const {
    return pImplPERSON->HadSecondScreeningTest();
}
bool Person::IsIdentifiedAsInfected(InfectionType it) const {
    return pImplPERSON->IsIdentifiedAsInfected(it);
}
bool Person::HasInitiatedTreatment(InfectionType it) const {
    return pImplPERSON->HasInitiatedTreatment(it);
}
bool Person::IsInRetreatment(InfectionType it) const {
    return pImplPERSON->IsInRetreatment(it);
}
bool Person::IsGenotypeThree() const { return pImplPERSON->IsGenotypeThree(); }
bool Person::IsBoomer() const { return pImplPERSON->IsBoomer(); }
bool Person::IsCirrhotic() { return pImplPERSON->IsCirrhotic(); }
// Getters
int Person::GetID() const { return pImplPERSON->GetID(); }
int Person::GetCurrentTimestep() const {
    return pImplPERSON->GetCurrentTimestep();
}
int Person::GetAge() const { return pImplPERSON->GetAge(); }
int Person::GetTimesHCVInfected() const {
    return pImplPERSON->GetTimesHCVInfected();
}
int Person::GetAcuteHCVClearances() const {
    return pImplPERSON->GetAcuteHCVClearances();
}
int Person::GetWithdrawals() const { return pImplPERSON->GetWithdrawals(); }
int Person::GetToxicReactions() const {
    return pImplPERSON->GetToxicReactions();
}
int Person::GetCompletedTreatments() const {
    return pImplPERSON->GetCompletedTreatments();
}
int Person::GetRetreatments() const { return pImplPERSON->GetRetreatments(); }
int Person::GetSVRs() const { return pImplPERSON->GetSVRs(); }
int Person::GetNumberOfABTests(InfectionType it) const {
    return pImplPERSON->GetNumberOfABTests(it);
}
int Person::GetNumberOfRNATests(InfectionType it) const {
    return pImplPERSON->GetNumberOfRNATests(it);
}
int Person::GetTimeOfLastScreening(InfectionType it) const {
    return pImplPERSON->GetTimeOfLastScreening(it);
}
int Person::GetTimeSinceLastScreening(InfectionType it) const {
    return pImplPERSON->GetTimeSinceLastScreening(it);
}
bool Person::GetCurrentlyOverdosing() const {
    return pImplPERSON->GetCurrentlyOverdosing();
}
int Person::GetNumberOfOverdoses() const {
    return pImplPERSON->GetNumberOfOverdoses();
}
FibrosisState Person::GetTrueFibrosisState() const {
    return pImplPERSON->GetTrueFibrosisState();
}
DeathReason Person::GetDeathReason() const {
    return pImplPERSON->GetDeathReason();
}
HCV Person::GetHCV() const { return pImplPERSON->GetHCV(); }
Behavior Person::GetBehavior() const { return pImplPERSON->GetBehavior(); }
int Person::GetTimeBehaviorChange() const {
    return pImplPERSON->GetTimeBehaviorChange();
}
int Person::GetTimeHCVChanged() const {
    return pImplPERSON->GetTimeHCVChanged();
}
int Person::GetTimeSinceHCVChanged() const {
    return pImplPERSON->GetTimeSinceHCVChanged();
}
int Person::GetTimeTrueFibrosisStateChanged() const {
    return pImplPERSON->GetTimeTrueFibrosisStateChanged();
}
bool Person::GetSeropositivity() const {
    return pImplPERSON->GetSeropositivity();
}
int Person::GetTimeHCVIdentified() const {
    return pImplPERSON->GetTimeHCVIdentified();
}
LinkageState Person::GetLinkState(InfectionType it) const {
    return pImplPERSON->GetLinkState(it);
}
int Person::GetTimeOfLinkChange(InfectionType it) const {
    return pImplPERSON->GetTimeOfLinkChange(it);
}
int Person::GetTimeSinceLinkChange(InfectionType it) const {
    return pImplPERSON->GetTimeSinceLinkChange(it);
}
int Person::GetLinkCount(InfectionType it) const {
    return pImplPERSON->GetLinkCount(it);
}
void Person::SetLinkageType(LinkageType linkType, InfectionType it) {
    return pImplPERSON->SetLinkageType(linkType, it);
}
LinkageType Person::GetLinkageType(InfectionType it) const {
    return pImplPERSON->GetLinkageType(it);
}
int Person::GetTimeOfTreatmentInitiation(InfectionType it) const {
    return pImplPERSON->GetTimeOfTreatmentInitiation(it);
}
int Person::GetTimeSinceTreatmentInitiation(InfectionType it) const {
    return pImplPERSON->GetTimeSinceTreatmentInitiation(it);
}
PregnancyState Person::GetPregnancyState() const {
    return pImplPERSON->GetPregnancyState();
}
int Person::GetTimeOfPregnancyChange() const {
    return pImplPERSON->GetTimeOfPregnancyChange();
}
int Person::GetTimeSincePregnancyChange() const {
    return pImplPERSON->GetTimeSincePregnancyChange();
}
std::vector<Child> Person::GetChildren() const {
    return pImplPERSON->GetChildren();
}
int Person::GetNumMiscarriages() const {
    return pImplPERSON->GetNumMiscarriages();
}
int Person::GetTimeOfFibrosisStaging() const {
    return pImplPERSON->GetTimeOfFibrosisStaging();
}
int Person::GetTimeSinceFibrosisStaging() const {
    return pImplPERSON->GetTimeSinceFibrosisStaging();
}
MOUD Person::GetMoudState() const { return pImplPERSON->GetMoudState(); }
MeasuredFibrosisState Person::GetMeasuredFibrosisState() const {
    return pImplPERSON->GetMeasuredFibrosisState();
}
int Person::GetTimeStartedMoud() const {
    return pImplPERSON->GetTimeStartedMoud();
}
Sex Person::GetSex() const { return pImplPERSON->GetSex(); }
LifetimeUtility Person::GetTotalUtility() const {
    return pImplPERSON->GetTotalUtility();
}
std::unordered_map<cost::CostCategory, std::pair<double, double>>
Person::GetCosts() const {
    return pImplPERSON->GetCosts();
}
std::pair<double, double> Person::GetCostTotals() const {
    return pImplPERSON->GetCostTotals();
}
HCVDetails Person::GetHCVDetails() const {
    return pImplPERSON->GetHCVDetails();
}
HIVDetails Person::GetHIVDetails() const {
    return pImplPERSON->GetHIVDetails();
}
HCCDetails Person::GetHCCDetails() const {
    return pImplPERSON->GetHCCDetails();
}
BehaviorDetails Person::GetBehaviorDetails() const {
    return pImplPERSON->GetBehaviorDetails();
}
int Person::GetLifeSpan() const { return pImplPERSON->GetLifeSpan(); }
double Person::GetDiscountedLifeSpan() const {
    return pImplPERSON->GetDiscountedLifeSpan();
}
void Person::AddDiscountedLifeSpan(double discounted_life) {
    pImplPERSON->AddDiscountedLifeSpan(discounted_life);
}
LinkageDetails Person::GetLinkStatus(InfectionType it) const {
    return pImplPERSON->GetLinkStatus(it);
}
MOUDDetails Person::GetMOUDDetails() const {
    return pImplPERSON->GetMOUDDetails();
}
PregnancyDetails Person::GetPregnancyDetails() const {
    return pImplPERSON->GetPregnancyDetails();
}
StagingDetails Person::GetFibrosisStagingDetails() const {
    return pImplPERSON->GetFibrosisStagingDetails();
}
ScreeningDetails Person::GetScreeningDetails(InfectionType it) const {
    return pImplPERSON->GetScreeningDetails(it);
}
TreatmentDetails Person::GetTreatmentDetails(InfectionType it) const {
    return pImplPERSON->GetTreatmentDetails(it);
}
int Person::GetNumberOfTreatmentStarts(InfectionType it) const {
    return pImplPERSON->GetNumberOfTreatmentStarts(it);
}
std::string Person::GetPersonDataString() const {
    return pImplPERSON->GetPersonDataString();
}
// Setters
void Person::SetDeathReason(DeathReason death_reason) {
    pImplPERSON->SetDeathReason(death_reason);
}
void Person::SetAge(int age) { pImplPERSON->SetAge(age); }
void Person::GiveSecondScreeningTest(bool state) {
    pImplPERSON->GiveSecondScreeningTest(state);
}
void Person::Diagnose(InfectionType it) { pImplPERSON->Diagnose(it); }
bool Person::CheckAntibodyPositive(InfectionType it) const {
    return pImplPERSON->CheckAntibodyPositive(it);
}
void Person::SetAntibodyPositive(bool result, InfectionType it) {
    pImplPERSON->SetAntibodyPositive(result, it);
}
void Person::SetSeropositivity(bool seropositive) {
    pImplPERSON->SetSeropositivity(seropositive);
}
void Person::SetHCV(HCV hcv) { pImplPERSON->SetHCV(hcv); }
void Person::InitiateTreatment(InfectionType it) {
    pImplPERSON->InitiateTreatment(it);
}
void Person::SetPregnancyState(PregnancyState state) {
    pImplPERSON->SetPregnancyState(state);
}
void Person::AddInfantExposure() { pImplPERSON->AddInfantExposure(); }
int Person::GetInfantHCVExposures() const {
    return pImplPERSON->GetInfantHCVExposures();
}
int Person::GetInfantHCVInfections() const {
    return pImplPERSON->GetInfantHCVInfections();
}
int Person::GetInfantHCVTests() const {
    return pImplPERSON->GetInfantHCVTests();
}
int Person::GetPregnancyCount() const {
    return pImplPERSON->GetPregnancyCount();
}
void Person::SetNumMiscarriages(int miscarriages) {
    pImplPERSON->SetNumMiscarriages(miscarriages);
}
void Person::UpdateTrueFibrosis(FibrosisState state) {
    pImplPERSON->UpdateTrueFibrosis(state);
}
void Person::SetGenotypeThree(bool genotype) {
    pImplPERSON->SetGenotypeThree(genotype);
}
void Person::TransitionMOUD() { pImplPERSON->TransitionMOUD(); }
void Person::AccumulateTotalUtility(std::pair<double, double> util,
                                    std::pair<double, double> discount_util) {
    pImplPERSON->AccumulateTotalUtility(util, discount_util);
}
std::pair<double, double> Person::GetUtility() const {
    return pImplPERSON->GetUtility();
}
void Person::SetUtility(double util, utility::UtilityCategory category) {
    pImplPERSON->SetUtility(util, category);
}
std::unordered_map<utility::UtilityCategory, double>
Person::GetCurrentUtilities() const {
    return pImplPERSON->GetCurrentUtilities();
}
void Person::SetBoomer(bool status) { pImplPERSON->SetBoomer(status); }
void Person::DevelopHCC(HCCState state) { pImplPERSON->DevelopHCC(state); }
HCCState Person::GetHCCState() const { return pImplPERSON->GetHCCState(); }
void Person::DiagnoseHCC() { pImplPERSON->DiagnoseHCC(); }
bool Person::IsDiagnosedWithHCC() const {
    return pImplPERSON->IsDiagnosedWithHCC();
}

// HIV
HIV Person::GetHIV() const { return pImplPERSON->GetHIV(); }
void Person::SetHIV(HIV hiv) { return pImplPERSON->SetHIV(hiv); }
int Person::GetLowCD4MonthCount() const {
    return pImplPERSON->GetLowCD4MonthCount();
}
void Person::InfectHIV() { return pImplPERSON->InfectHIV(); }
int Person::GetTimeHIVChanged() const {
    return pImplPERSON->GetTimeHIVChanged();
}
int Person::GetTimeSinceHIVChanged() const {
    return pImplPERSON->GetTimeSinceHIVChanged();
}
} // namespace person
