#include "Person.hpp"
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
        struct person_select {
            Sex sex = Sex::MALE;
            int age = 0;
            bool isAlive = true;
            DeathReason deathReason = DeathReason::NA;
            int identifiedHCV = false;
            int timeInfectionIdentified = -1;
            HCV hcv = HCV::NONE;
            FibrosisState fibrosisState = FibrosisState::NONE;
            bool isGenotypeThree = false;
            bool seropositive = false;
            int timeHCVChanged = -1;
            int timeFibrosisStateChanged = -1;
            Behavior drugBehavior = Behavior::NEVER;
            int timeLastActiveDrugUse = -1;
            LinkageState linkageState = LinkageState::NEVER;
            int timeOfLinkChange = -1;
            LinkageType linkageType = LinkageType::INTERVENTION;
            int linkCount = 0;
            MeasuredFibrosisState measuredFibrosisState =
                MeasuredFibrosisState::NONE;
            int timeOfLastStaging = -1;
            int timeOfLastScreening = -1;
            int numABTests = 0;
            int numRNATests = 0;
            int timesInfected = 0;
            int timesCleared = 0;
            bool initiatedTreatment = false;
            int timeOfTreatmentInitiation = -1;
            double minUtility = 1.0;
            double multUtility = 1.0;
            int treatmentWithdrawals = 0;
            int treatmentToxicReactions = 0;
            int completedTreatments = 0;
            int svrs = 0;
        };
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            struct person_select *temp = (struct person_select *)storage;
            temp->sex << data[0];
            temp->age = std::stoi(data[1]);
            temp->isAlive = std::stoi(data[2]);
            temp->deathReason << data[3];
            temp->identifiedHCV = std::stoi(data[4]);
            temp->timeInfectionIdentified = std::stoi(data[5]);
            temp->hcv << data[6];
            temp->fibrosisState << data[7];
            temp->isGenotypeThree = std::stoi(data[8]);
            temp->seropositive = std::stoi(data[9]);
            temp->timeHCVChanged = std::stoi(data[10]);
            temp->timeFibrosisStateChanged = std::stoi(data[11]);
            temp->drugBehavior << data[12];
            temp->timeLastActiveDrugUse = std::stoi(data[13]);
            temp->linkageState << data[14];
            temp->timeOfLinkChange = std::stoi(data[15]);
            temp->linkageType << data[16];
            temp->linkCount = std::stoi(data[17]);
            temp->measuredFibrosisState << data[18];
            temp->timeOfLastStaging = std::stoi(data[19]);
            temp->timeOfLastScreening = std::stoi(data[20]);
            temp->numABTests = std::stoi(data[21]);
            temp->numRNATests = std::stoi(data[22]);
            temp->timesInfected = std::stoi(data[23]);
            temp->timesCleared = std::stoi(data[24]);
            temp->initiatedTreatment = std::stoi(data[25]);
            temp->timeOfTreatmentInitiation = std::stoi(data[26]);
            temp->minUtility = std::stod(data[27]);
            temp->multUtility = std::stod(data[28]);
            temp->treatmentWithdrawals = std::stoi(data[29]);
            temp->treatmentToxicReactions = std::stoi(data[30]);
            temp->completedTreatments = std::stoi(data[31]);
            temp->svrs = std::stoi(data[32]);
            return 0;
        }
        size_t id = 0;
        size_t _currentTime = 0;

        int age = 0;
        Sex sex = Sex::MALE;
        bool isAlive = true;
        DeathReason deathReason = DeathReason::NA;
        BehaviorDetails behaviorDetails;
        Health health;
        LinkageDetails linkStatus;
        int numOverdoses = 0;
        int treatmentWithdrawals = 0;
        int treatmentToxicReactions = 0;
        int completedTreatments = 0;
        int svrs = 0;
        bool currentlyOverdosing = false;
        bool hccDiagnosed = false;
        MOUDDetails moudDetails;
        PregnancyDetails pregnancyDetails;
        StagingDetails stagingDetails;
        ScreeningDetails screeningDetails;
        TreatmentDetails treatmentDetails;
        UtilityTracker utilityTracker;
        cost::CostTracker costs;
        bool boomerClassification = false;
        std::vector<Child> children = {};

        int UpdateTimers() {
            this->_currentTime++;
            if (GetBehavior() == person::Behavior::NONINJECTION ||
                GetBehavior() == person::Behavior::INJECTION) {
                this->behaviorDetails.timeLastActive = this->_currentTime;
            }
            if (GetMoudState() == person::MOUD::CURRENT) {
                this->moudDetails.totalMOUDMonths++;
            }
            this->moudDetails.currentStateConcurrentMonths++;
            return 0;
        }

        /// @brief Setter for MOUD State
        /// @param moud PersonIMPL's new MOUD state
        void SetMoudState(MOUD moud) {
            if (moud == person::MOUD::CURRENT) {
                this->moudDetails.timeStartedMoud = this->_currentTime;
            }
            this->moudDetails.currentStateConcurrentMonths = 0;
            this->moudDetails.moudState = moud;
        }

        int CalculateTimeSince(int lasttime) const {
            if (lasttime == -1) {
                return GetCurrentTimestep();
            }
            return GetCurrentTimestep() - lasttime;
        }

    public:
        PersonIMPL() {}
        int CreatePersonFromTable(
            int id, std::shared_ptr<datamanagement::DataManagerBase> dm) {
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
            int rc = dm->SelectCustomCallback(query.str(), this->callback,
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
            isAlive = storage.isAlive;
            health.identifiedHCV = storage.identifiedHCV;
            health.timeIdentified = storage.timeInfectionIdentified;
            health.hcv = storage.hcv;
            health.fibrosisState = storage.fibrosisState;
            health.isGenotypeThree = storage.isGenotypeThree;
            health.seropositive = storage.seropositive;
            health.timeHCVChanged = storage.timeHCVChanged;
            health.timeFibrosisStateChanged = storage.timeFibrosisStateChanged;
            behaviorDetails.behavior = storage.drugBehavior;
            behaviorDetails.timeLastActive = storage.timeLastActiveDrugUse;
            linkStatus.linkState = storage.linkageState;
            linkStatus.timeOfLinkChange = storage.timeOfLinkChange;
            linkStatus.linkType = storage.linkageType;
            linkStatus.linkCount = storage.linkCount;
            stagingDetails.measuredFibrosisState =
                storage.measuredFibrosisState;
            stagingDetails.timeOfLastStaging = storage.timeOfLastStaging;
            screeningDetails.timeOfLastScreening = storage.timeOfLastScreening;
            screeningDetails.numABTests = storage.numABTests;
            screeningDetails.numRNATests = storage.numRNATests;
            health.timesInfected = storage.timesInfected;
            health.timesCleared = storage.timesCleared;
            treatmentDetails.initiatedTreatment = storage.initiatedTreatment;
            treatmentDetails.timeOfTreatmentInitiation =
                storage.timeOfTreatmentInitiation;
            utilityTracker.minUtil = storage.minUtility;
            utilityTracker.multUtil = storage.multUtility;
            return 0;
        }

        /// @brief End a PersonIMPL's life and set final Age
        void Die(DeathReason deathReason) {
            this->isAlive = false;
            SetDeathReason(deathReason);
        }

        /// @brief increase a person's Age
        void Grow() {
            UpdateTimers();
            if (this->isAlive) {
                this->age++;
            }
        }

        /// @brief InfectHCV the person
        /// @param timestep Current simulation timestep
        void InfectHCV() {
            // cannot be multiply infected
            if (this->health.hcv != HCV::NONE) {
                return;
            }
            this->health.hcv = HCV::ACUTE;
            this->health.timeHCVChanged = this->_currentTime;
            this->health.seropositive = true;
            this->health.timesInfected++;

            // once infected, immediately enter F0
            if (this->health.fibrosisState == FibrosisState::NONE) {
                UpdateTrueFibrosis(FibrosisState::F0);
            }
        }

        /// @brief Clear of HCV
        /// @param timestep Current simulation timestep
        void ClearHCV() {
            // cannot clear if the person is not infected
            if (GetHCV() == HCV::NONE) {
                return;
            }
            this->health.hcv = HCV::NONE;
            this->health.identifiedHCV = false;
            this->health.timeHCVChanged = this->_currentTime;
            this->AddHCVClearance();
        }

        int LoadICValues(int id, std::vector<std::string> icValues) {
            if (icValues.size() < 10) {
                spdlog::get("main")->warn(
                    "Incorrect Number of Initial Cohort Values Retrieved!");
                return -1;
            }
            this->id = id;
            SetAge(std::stoi(icValues[1]));
            this->sex = static_cast<person::Sex>(std::stoi(icValues[2]));
            SetBehavior(static_cast<person::Behavior>(std::stoi(icValues[3])));
            behaviorDetails.timeLastActive = std::stoi(icValues[4]);
            bool sero = (std::stoi(icValues[5]) == 1) ? true : false;
            SetSeropositivity(sero);
            bool geno = (std::stoi(icValues[6]) == 1) ? true : false;
            SetGenotypeThree(geno);
            this->health.fibrosisState =
                static_cast<person::FibrosisState>(std::stoi(icValues[7]));
            if (std::stoi(icValues[8]) == 1) {
                this->health.identifiedHCV = true;
                this->health.historyOfHCV = true;
            }
            linkStatus.linkState =
                static_cast<person::LinkageState>(std::stoi(icValues[9]));
            health.hcv = static_cast<person::HCV>(std::stoi(icValues[10]));
            return 0;
        }

        /// @brief Update Opioid Use Behavior Classification
        /// @param bc The intended resultant Behavior
        /// @param timestep Current simulation timestep
        void SetBehavior(const Behavior &bc) {
            // nothing to do -- cannot go back to NEVER
            if (bc == this->behaviorDetails.behavior || bc == Behavior::NEVER) {
                return;
            }
            if ((bc == person::Behavior::NONINJECTION ||
                 bc == person::Behavior::INJECTION) &&
                (this->behaviorDetails.behavior == Behavior::NEVER ||
                 this->behaviorDetails.behavior == Behavior::FORMER_INJECTION ||
                 this->behaviorDetails.behavior ==
                     Behavior::FORMER_NONINJECTION)) {
                this->behaviorDetails.timeLastActive = this->_currentTime;
            }
            this->behaviorDetails.behavior = bc;
        }

        /// @brief Diagnose somebody's fibrosis
        /// @return Fibrosis state that is diagnosed
        MeasuredFibrosisState DiagnoseFibrosis(MeasuredFibrosisState &data) {
            // need to add functionality here
            this->stagingDetails.measuredFibrosisState = data;
            this->stagingDetails.timeOfLastStaging = this->_currentTime;
            return this->stagingDetails.measuredFibrosisState;
        }

        /// @brief Add an acute clearance to the running count
        void AddHCVClearance() { this->health.timesCleared++; };

        void AddWithdrawal() { this->treatmentWithdrawals++; }

        void AddToxicReaction() { this->treatmentToxicReactions++; }

        void AddCompletedTreatment() { this->completedTreatments++; }

        void AddSVR() { this->svrs++; }

        /// @brief Mark somebody as having been screened this timestep
        void MarkScreened() {
            this->screeningDetails.timeOfLastScreening = this->_currentTime;
        }

        /// @brief
        void AddAbScreen() {
            this->screeningDetails.timeOfLastScreening = this->_currentTime;
            this->screeningDetails.numABTests++;
        }

        /// @brief
        void AddRnaScreen() {
            this->screeningDetails.timeOfLastScreening = this->_currentTime;
            this->screeningDetails.numRNATests++;
        }

        /// @brief Reset a PersonIMPL's Link State to Unlinked
        /// @param timestep Timestep during which the PersonIMPL is Unlinked
        void Unlink() {
            if (this->linkStatus.linkState == LinkageState::LINKED) {
                this->linkStatus.linkState = LinkageState::UNLINKED;
                this->linkStatus.timeOfLinkChange = this->_currentTime;
            }
            this->treatmentDetails.initiatedTreatment = false;
            this->ClearHCVDiagnosis();
        }

        /// @brief Reset a PersonIMPL's Link State to Linked
        /// @param timestep Timestep during which the PersonIMPL is Linked
        /// @param linkType The linkage type the PersonIMPL recieves
        void Link(LinkageType linkType) {
            this->linkStatus.linkState = LinkageState::LINKED;
            this->linkStatus.timeOfLinkChange = this->_currentTime;
            this->linkStatus.linkType = linkType;
            this->linkStatus.linkCount += 1;
        }

        /// @brief Mark a PersonIMPL as Identified as Infected
        /// @param timestep Timestep during which Identification Occurs
        void DiagnoseHCV() {
            this->health.identifiedHCV = true;
            this->health.timeIdentified = this->_currentTime;
            this->health.historyOfHCV = true;
        }

        int ClearHCVDiagnosis() {
            this->health.identifiedHCV = false;
            return 0;
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

        void Miscarry() {
            this->pregnancyDetails.numMiscarriages++;
            this->pregnancyDetails.timeOfPregnancyChange = this->_currentTime;
            this->pregnancyDetails.pregnancyState =
                person::PregnancyState::NONE;
        }

        void EndPostpartum() {
            this->pregnancyDetails.timeOfPregnancyChange = this->_currentTime;
            this->pregnancyDetails.pregnancyState =
                person::PregnancyState::NONE;
        }

        void Impregnate() {
            if (GetSex() == person::Sex::MALE) {
                return;
            }
            this->pregnancyDetails.timeOfPregnancyChange = this->_currentTime;
            this->pregnancyDetails.pregnancyState =
                person::PregnancyState::PREGNANT;
        }

        void Stillbirth() {
            if (GetSex() == person::Sex::MALE) {
                return;
            }
            this->pregnancyDetails.timeOfPregnancyChange = this->_currentTime;
            this->pregnancyDetails.pregnancyState =
                person::PregnancyState::POSTPARTUM;
            this->pregnancyDetails.numMiscarriages++;
        }

        void AddChild(HCV hcv, bool test) {
            person::Child child;
            child.hcv = hcv;
            child.tested = test;
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

        bool IsAlive() const { return this->isAlive; }

        /// @brief Getter for whether PersonIMPL experienced fibtest two this
        /// cycle
        /// @return value of hadSecondTest
        bool HadSecondScreeningTest() const {
            return this->stagingDetails.hadSecondTest;
        }

        /// @brief Getter for Identification Status
        /// @return Boolean Describing Indentified as Positive Status
        bool IsIdentifiedAsHCVInfected() const {
            return this->health.identifiedHCV;
        }

        bool HistoryOfHCVInfection() const { return this->health.historyOfHCV; }

        /// @brief Getter for whether PersonIMPL has initiated treatment
        /// @return Boolean true if PersonIMPL has initiated treatment, false
        /// otherwise
        bool HasInitiatedTreatment() const {
            return this->treatmentDetails.initiatedTreatment;
        }
        /// @brief Getter for whether PersonIMPL is genotype three
        /// @return True if genotype three, false otherwise
        bool IsGenotypeThree() const { return this->health.isGenotypeThree; }
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

        int GetCurrentTimestep() const { return this->_currentTime; }

        int GetAge() const { return this->age; }

        /// @brief Getter for the number of HCV infections experienced by
        /// PersonIMPL
        /// @return Number of HCV infections experienced by PersonIMPL
        int GetTimesHCVInfected() const { return this->health.timesInfected; }

        /// @brief Get the running total of clearances for PersonIMPL
        int GetHCVClearances() const { return this->health.timesCleared; };

        int GetWithdrawals() const { return this->treatmentWithdrawals; }

        int GetToxicReactions() const { return this->treatmentToxicReactions; }

        int GetCompletedTreatments() const { return this->completedTreatments; }

        int GetSVRs() const { return this->svrs; }

        /// @brief
        int GetNumberOfABTests() const {
            return this->screeningDetails.numABTests;
        }

        /// @brief
        int GetNumberOfRNATests() const {
            return this->screeningDetails.numRNATests;
        }

        /// @brief Getter for the Time Since the Last Screening
        /// @return The Time Since the Last Screening
        int GetTimeOfLastScreening() const {
            return this->screeningDetails.timeOfLastScreening;
        }

        /// @brief Getter for the person's overdose state
        /// @return Boolean representing overdose or not
        bool GetCurrentlyOverdosing() const {
            return this->currentlyOverdosing;
        }

        int GetNumberOfOverdoses() const { return this->numOverdoses; }

        DeathReason GetDeathReason() const { return this->deathReason; }

        /// @brief Getter for the Fibrosis State
        /// @return The Current Fibrosis State
        FibrosisState GetTrueFibrosisState() const {
            return this->health.fibrosisState;
        }

        /// @brief Getter for the HCV State
        /// @return HCV State
        HCV GetHCV() const { return this->health.hcv; }

        /// @brief Getter for Behavior Classification
        /// @return Behavior Classification
        Behavior GetBehavior() const { return this->behaviorDetails.behavior; }

        /// @brief Getter for time since active drug use
        /// @return Time since the person left an active drug use state
        int GetTimeBehaviorChange() const {
            return this->behaviorDetails.timeLastActive;
        }

        /// @brief Getter for timestep in which HCV last changed
        /// @return Time Since HCV Change
        int GetTimeHCVChanged() const { return this->health.timeHCVChanged; }

        /// @brief Getter for Time since Fibrosis State Change
        /// @return Time Since Fibrosis State Change
        int GetTimeTrueFibrosisStateChanged() const {
            return this->health.timeFibrosisStateChanged;
        }

        /// @brief Getter for Seropositive
        /// @return Seropositive
        bool GetSeropositivity() const { return this->health.seropositive; }

        int GetTimeHCVIdentified() const { return this->health.timeIdentified; }

        /// @brief Getter for Link State
        /// @return Link State
        LinkageState GetLinkState() const { return this->linkStatus.linkState; }

        /// @brief Getter for Timestep in which linkage changed
        /// @return Time Link Change
        int GetTimeOfLinkChange() const {
            return this->linkStatus.timeOfLinkChange;
        }

        /// @brief Getter for link count
        /// @return Number of times PersonIMPL has linked to care
        int GetLinkCount() const { return this->linkStatus.linkCount; }

        void SetLinkageType(LinkageType linkType) {
            this->linkStatus.linkType = linkType;
        }

        /// @brief Getter for Linkage Type
        /// @return Linkage Type
        LinkageType GetLinkageType() const { return this->linkStatus.linkType; }

        /// @brief Getter for the number of timesteps PersonIMPL has been in
        /// treatment
        /// @return Integer number of timesteps spent in treatment
        int GetTimeOfTreatmentInitiation() const {
            return this->treatmentDetails.timeOfTreatmentInitiation;
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

        int GetTimeSinceLastScreening() const {
            return CalculateTimeSince(GetTimeOfLastScreening());
        }

        int GetTimeSinceHCVChanged() const {
            return CalculateTimeSince(GetTimeHCVChanged());
        }

        int GetTimeSinceLinkChange() const {
            return CalculateTimeSince(GetTimeOfLinkChange());
        }

        int GetTimeSinceTreatmentInitiation() const {
            return CalculateTimeSince(GetTimeOfTreatmentInitiation());
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
        int GetTimeStartedMoud() const {
            return this->moudDetails.timeStartedMoud;
        }

        /// @brief Getter for the person's sex
        /// @return PersonIMPL's sex
        Sex GetSex() const { return this->sex; }
        /// @brief Getter for the person's stratified utilities
        /// @return PersonIMPL's stratified utilities
        UtilityTracker GetUtility() const { return this->utilityTracker; }

        /// @brief Getter for the PersonIMPL's costs
        /// @return cost::CostTracker containing this person's costs
        std::unordered_map<cost::CostCategory, std::pair<double, double>>
        GetCosts() const {
            return this->costs.GetCosts();
        }

        std::pair<double, double> GetCostTotals() const {
            return this->costs.GetTotals();
        }

        Health GetHealth() const { return this->health; }

        BehaviorDetails GetBehaviorDetails() const {
            return this->behaviorDetails;
        }

        LinkageDetails GetLinkStatus() const { return this->linkStatus; }

        MOUDDetails GetMOUDDetails() const { return this->moudDetails; }

        PregnancyDetails GetPregnancyDetails() const {
            return this->pregnancyDetails;
        }

        StagingDetails GetFibrosisStagingDetails() const {
            return this->stagingDetails;
        }

        ScreeningDetails GetScreeningDetails() const {
            return this->screeningDetails;
        }

        TreatmentDetails GetTreatmentDetails() const {
            return this->treatmentDetails;
        }

        std::string GetPersonDataString() const {
            std::stringstream data;
            data << GetAge() << "," << GetSex() << "," << GetBehavior() << ","
                 << GetTimeBehaviorChange() << "," << GetSeropositivity() << ","
                 << IsGenotypeThree() << "," << GetTrueFibrosisState() << ","
                 << IsIdentifiedAsHCVInfected() << "," << GetLinkState() << ","
                 << IsAlive() << "," << GetDeathReason() << ","
                 << GetTimeHCVIdentified() << "," << GetHCV() << ","
                 << GetTimeHCVChanged() << ","
                 << GetTimeTrueFibrosisStateChanged() << ","
                 << GetTimeBehaviorChange() << "," << GetTimeOfLinkChange()
                 << "," << GetLinkageType() << "," << GetLinkCount() << ","
                 << GetMeasuredFibrosisState() << ","
                 << GetTimeOfFibrosisStaging() << ","
                 << GetTimeOfLastScreening() << "," << GetNumberOfABTests()
                 << "," << GetNumberOfRNATests() << "," << GetTimesHCVInfected()
                 << "," << GetHCVClearances() << "," << std::boolalpha
                 << GetTreatmentDetails().initiatedTreatment << ","
                 << GetTimeOfTreatmentInitiation() << ","
                 << std::to_string(GetUtility().minUtil) << ","
                 << std::to_string(GetUtility().multUtil);
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
            this->health.seropositive = seropositive;
        }

        void SetDeathReason(DeathReason deathReason) {
            this->deathReason = deathReason;
        }

        /// @brief Set HEPC State -- used to change to chronic infection
        /// @param New HEPC State
        void SetHCV(HCV hcv) {
            this->health.hcv = hcv;
            this->health.timeHCVChanged = this->_currentTime;
        }

        /// @brief Setter for PersonIMPL's treatment initiation state
        /// @param incompleteTreatment Boolean value for initiated treatment
        /// state to be set
        void InitiateTreatment() {
            this->treatmentDetails.timeOfTreatmentInitiation =
                this->_currentTime;
            this->treatmentDetails.initiatedTreatment = true;
        }

        /// @brief Setter for Pregnancy State
        /// @param state PersonIMPL's pregnancy State
        void SetPregnancyState(PregnancyState state) {
            this->pregnancyDetails.timeOfPregnancyChange = this->_currentTime;
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
            this->health.timeFibrosisStateChanged = this->_currentTime;
            this->health.fibrosisState = state;
        }

        /// @brief Setter for whether PersonIMPL is genotype three
        /// @param genotype True if infection is genotype three, false
        /// otherwise
        void SetGenotypeThree(bool genotype) {
            this->health.isGenotypeThree = genotype;
        }

        /// @brief Set a value for a person's utility
        /// @param category The category of the utility to be updated
        /// @param value The value of the utility to be updated, bounded by
        /// 0, 1
        void SetUtility(double util) {
            this->utilityTracker.minUtil =
                std::min(this->utilityTracker.minUtil, util);
            this->utilityTracker.multUtil *= util;
        }

        void SetBoomer(bool status) { this->boomerClassification = status; }

        void DevelopHCC(HCCState state) {
            HCCState current = this->health.hccState;
            switch (current) {
            case HCCState::NONE:
                this->health.hccState = HCCState::EARLY;
                break;
            case HCCState::EARLY:
                this->health.hccState = HCCState::LATE;
                break;
            default:
                break;
            }
        }
        HCCState GetHCCState() const { return this->health.hccState; }
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
        os << person.GetHealth() << std::endl;
        os << person.GetBehaviorDetails() << std::endl;
        os << person.GetLinkStatus() << std::endl;
        os << "Overdoses: " << person.GetNumberOfOverdoses() << std::endl;
        os << person.GetMOUDDetails() << std::endl;
        os << person.GetPregnancyDetails() << std::endl;
        os << person.GetFibrosisStagingDetails() << std::endl;
        os << person.GetScreeningDetails() << std::endl;
        os << person.GetTreatmentDetails() << std::endl;
        os << person.GetUtility() << std::endl;
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
    int Person::Die(DeathReason deathReason) {
        pImplPERSON->Die(deathReason);
        return 0;
    }
    int Person::InfectHCV() {
        pImplPERSON->InfectHCV();
        return 0;
    }
    int Person::ClearHCV() {
        pImplPERSON->ClearHCV();
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
    int Person::AddHCVClearance() {
        pImplPERSON->AddHCVClearance();
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
    int Person::MarkScreened() {
        pImplPERSON->MarkScreened();
        return 0;
    }
    int Person::AddAbScreen() {
        pImplPERSON->AddAbScreen();
        return 0;
    }
    int Person::AddRnaScreen() {
        pImplPERSON->AddRnaScreen();
        return 0;
    }
    int Person::Unlink() {
        pImplPERSON->Unlink();
        return 0;
    }
    int Person::Link(LinkageType linkType) {
        pImplPERSON->Link(linkType);
        return 0;
    }
    int Person::DiagnoseHCV() {
        pImplPERSON->DiagnoseHCV();
        return 0;
    }
    int Person::ClearHCVDiagnosis() { return pImplPERSON->ClearHCVDiagnosis(); }
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
    bool Person::IsIdentifiedAsHCVInfected() const {
        return pImplPERSON->IsIdentifiedAsHCVInfected();
    }
    bool Person::HistoryOfHCVInfection() const {
        return pImplPERSON->HistoryOfHCVInfection();
    }
    bool Person::HasInitiatedTreatment() const {
        return pImplPERSON->HasInitiatedTreatment();
    }
    bool Person::IsGenotypeThree() const {
        return pImplPERSON->IsGenotypeThree();
    }
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
    int Person::GetHCVClearances() const {
        return pImplPERSON->GetHCVClearances();
    }
    int Person::GetWithdrawals() const { return pImplPERSON->GetWithdrawals(); }
    int Person::GetToxicReactions() const {
        return pImplPERSON->GetToxicReactions();
    }
    int Person::GetCompletedTreatments() const {
        return pImplPERSON->GetCompletedTreatments();
    }
    int Person::GetSVRs() const { return pImplPERSON->GetSVRs(); }
    int Person::GetNumberOfABTests() const {
        return pImplPERSON->GetNumberOfABTests();
    }
    int Person::GetNumberOfRNATests() const {
        return pImplPERSON->GetNumberOfRNATests();
    }
    int Person::GetTimeOfLastScreening() const {
        return pImplPERSON->GetTimeOfLastScreening();
    }
    int Person::GetTimeSinceLastScreening() const {
        return pImplPERSON->GetTimeSinceLastScreening();
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
    LinkageState Person::GetLinkState() const {
        return pImplPERSON->GetLinkState();
    }
    int Person::GetTimeOfLinkChange() const {
        return pImplPERSON->GetTimeOfLinkChange();
    }
    int Person::GetTimeSinceLinkChange() const {
        return pImplPERSON->GetTimeSinceLinkChange();
    }
    int Person::GetLinkCount() const { return pImplPERSON->GetLinkCount(); }
    void Person::SetLinkageType(LinkageType linkType) {
        return pImplPERSON->SetLinkageType(linkType);
    }
    LinkageType Person::GetLinkageType() const {
        return pImplPERSON->GetLinkageType();
    }
    int Person::GetTimeOfTreatmentInitiation() const {
        return pImplPERSON->GetTimeOfTreatmentInitiation();
    }
    int Person::GetTimeSinceTreatmentInitiation() const {
        return pImplPERSON->GetTimeSinceTreatmentInitiation();
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
    UtilityTracker Person::GetUtility() const {
        return pImplPERSON->GetUtility();
    }
    std::unordered_map<cost::CostCategory, std::pair<double, double>>
    Person::GetCosts() const {
        return pImplPERSON->GetCosts();
    }
    std::pair<double, double> Person::GetCostTotals() const {
        return pImplPERSON->GetCostTotals();
    }
    Health Person::GetHealth() const { return pImplPERSON->GetHealth(); }
    BehaviorDetails Person::GetBehaviorDetails() const {
        return pImplPERSON->GetBehaviorDetails();
    }
    LinkageDetails Person::GetLinkStatus() const {
        return pImplPERSON->GetLinkStatus();
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
    ScreeningDetails Person::GetScreeningDetails() const {
        return pImplPERSON->GetScreeningDetails();
    }
    TreatmentDetails Person::GetTreatmentDetails() const {
        return pImplPERSON->GetTreatmentDetails();
    }
    std::string Person::GetPersonDataString() const {
        return pImplPERSON->GetPersonDataString();
    }
    // Setters
    void Person::SetDeathReason(DeathReason deathReason) {
        pImplPERSON->SetDeathReason(deathReason);
    }
    void Person::SetAge(int age) { pImplPERSON->SetAge(age); }
    void Person::GiveSecondScreeningTest(bool state) {
        pImplPERSON->GiveSecondScreeningTest(state);
    }
    void Person::SetSeropositivity(bool seropositive) {
        pImplPERSON->SetSeropositivity(seropositive);
    }
    void Person::SetHCV(HCV hcv) { pImplPERSON->SetHCV(hcv); }
    void Person::InitiateTreatment() { pImplPERSON->InitiateTreatment(); }
    void Person::SetPregnancyState(PregnancyState state) {
        pImplPERSON->SetPregnancyState(state);
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
    void Person::SetUtility(double util) { pImplPERSON->SetUtility(util); }
    void Person::SetBoomer(bool status) { pImplPERSON->SetBoomer(status); }

    void Person::DevelopHCC(HCCState state) { pImplPERSON->DevelopHCC(state); }
    HCCState Person::GetHCCState() const { return pImplPERSON->GetHCCState(); }
    void Person::DiagnoseHCC() { pImplPERSON->DiagnoseHCC(); }
    bool Person::IsDiagnosedWithHCC() const {
        return pImplPERSON->IsDiagnosedWithHCC();
    }

} // namespace person
