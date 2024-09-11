#include "Person.hpp"
#include "Utils.hpp"
#include <DataManager.hpp>
#include <algorithm>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace person {
    class PersonBase::Person {
    private:
        size_t _currentTime = 0;
        Sex sex = Sex::MALE;
        bool isAlive = true;
        Health infectionStatus;
        BehaviorDetails behaviorDetails;
        LinkageDetails linkStatus;
        int numOverdoses = 0;
        bool currentlyOverdosing = false;
        MOUDDetails moudDetails;
        PregnancyDetails pregnancyDetails;
        StagingDetails stagingDetails;
        ScreeningDetails screeningDetails;
        TreatmentDetails treatmentDetails;
        UtilityTracker utilityTracker;
        cost::CostTracker costs;
        bool boomerClassification = false;
        /// @brief Person Age in months
        int age = 0;

    public:
        /// @brief Default constructor for Person
        Person() {}

        /// @brief Default destructor for Person
        virtual ~Person() {}

        /// @brief End a Person's life and set final Age
        void Die() { this->isAlive = false; }

        /// @brief increase a person's Age
        void Grow() {
            this->_currentTime++;
            if (this->isAlive) {
                this->age++;
            }
        }

        /// @brief Infect the person
        /// @param timestep Current simulation timestep
        void Infect() {
            // cannot be multiply infected
            if (this->infectionStatus.hcv != HCV::NONE) {
                return;
            }
            this->infectionStatus.hcv = HCV::ACUTE;
            this->infectionStatus.timeHCVChanged = this->_currentTime;
            this->infectionStatus.seropositive = true;
            this->infectionStatus.timesInfected++;

            if (this->infectionStatus.fibrosisState != FibrosisState::NONE) {
                return;
            }
            // once infected, immediately enter F0
            this->infectionStatus.fibrosisState = FibrosisState::F0;
            this->infectionStatus.timeFibrosisStateChanged = this->_currentTime;
        }

        /// @brief Clear of HCV
        /// @param timestep Current simulation timestep
        void ClearHCV() {
            // cannot clear if the person is not infected
            if (this->infectionStatus.hcv == HCV::NONE) {
                return;
            }
            this->infectionStatus.hcv = HCV::NONE;
            this->infectionStatus.timeHCVChanged = this->_currentTime;
            this->AddClearance();
        }

        /// @brief Update the Liver State
        /// @param ls Current Liver State
        /// @param timestep Current simulation timestep
        void UpdateFibrosis(const FibrosisState &ls) {
            // nothing to do -- can only advance fibrosis state
            if (ls <= this->infectionStatus.fibrosisState) {
                return;
            }
            this->infectionStatus.fibrosisState = ls;
            this->infectionStatus.timeFibrosisStateChanged = this->_currentTime;
        }

        /// @brief Update Opioid Use Behavior Classification
        /// @param bc The intended resultant Behavior
        /// @param timestep Current simulation timestep
        void UpdateBehavior(const Behavior &bc) {
            // nothing to do -- cannot go back to NEVER
            if (bc == this->behaviorDetails.behavior || bc == Behavior::NEVER) {
                return;
            }
            // count for timeSinceActive if switching from active to non-active
            // use
            if ((bc == Behavior::FORMER_NONINJECTION) ||
                (bc == Behavior::FORMER_INJECTION)) {
                this->behaviorDetails.timeLastActive = this->_currentTime;
            }
            // update the behavior classification
            this->behaviorDetails.behavior = bc;
        }

        /// @brief Diagnose somebody's fibrosis
        /// @return Fibrosis state that is diagnosed
        FibrosisState DiagnoseFibrosis() {
            // need to add functionality here
            this->infectionStatus.fibrosisState = FibrosisState::F0;
            return this->infectionStatus.fibrosisState;
        }

        /// @brief Dignose somebody with HEPC
        /// @return HEPC state that was diagnosed
        HCV DiagnoseHEPC() {
            // need to add functionality here
            this->infectionStatus.hcv = HCV::ACUTE;
            return this->infectionStatus.hcv;
        }

        /// @brief Add an acute clearance to the running count
        void AddClearance() { this->infectionStatus.timesCleared++; };

        /// @brief Mark somebody as having been screened this timestep
        void MarkScreened() { this->screeningDetails.timeOfLastScreening = 0; }

        /// @brief
        void AddAbScreen() { this->screeningDetails.numABTests++; }

        /// @brief
        void AddRnaScreen() { this->screeningDetails.numRNATests++; }

        /// @brief Reset a Person's Link State to Unlinked
        /// @param timestep Timestep during which the Person is Unlinked
        void Unlink() {
            if (this->linkStatus.linkState == LinkageState::LINKED) {
                this->linkStatus.linkState = LinkageState::UNLINKED;
                this->linkStatus.timeOfLinkChange = this->_currentTime;
            }
        }

        /// @brief Reset a Person's Link State to Linked
        /// @param timestep Timestep during which the Person is Linked
        /// @param linkType The linkage type the Person recieves
        void Link(LinkageType linkType) {
            this->linkStatus.linkState = LinkageState::LINKED;
            this->linkStatus.timeOfLinkChange = this->_currentTime;
            this->linkStatus.linkType = linkType;
            this->linkStatus.linkCount++;
        }

        /// @brief Mark a Person as Identified as Infected
        /// @param timestep Timestep during which Identification Occurs
        void IdentifyAsInfected() {
            this->infectionStatus.identifiedHCV = true;
            this->infectionStatus.timeIdentified = this->_currentTime;
        }

        /// @brief Add a cost to the person's CostTracker object
        /// @param cost The cost to be added
        /// @param timestep The timestep during which the cost was accrued
        void AddCost(cost::Cost cost) {
            this->costs.addCost(cost, this->_currentTime);
        }

        /// @brief Flips the person's overdose state
        void ToggleOverdose() {
            this->currentlyOverdosing = !this->currentlyOverdosing;
        }

        ////////////// CHECKS /////////////////

        /// @brief Getter for whether Person experienced fibtest two this cycle
        /// @return value of hadSecondTest
        bool HadSecondTest() { return this->stagingDetails.hadSecondTest; }

        /// @brief Getter for Identification Status
        /// @return Boolean Describing Indentified as Positive Status
        bool IsIdentifiedAsInfected() const {
            return this->infectionStatus.identifiedHCV;
        }

        /// @brief Getter for whether Person has initiated treatment
        /// @return Boolean true if Person has initiated treatment, false
        /// otherwise
        bool HasInitiatedTreatment() const {
            return this->treatmentDetails.initiatedTreatment;
        }
        /// @brief Getter for whether Person is genotype three
        /// @return True if genotype three, false otherwise
        bool IsGenotypeThree() const {
            return this->infectionStatus.isGenotypeThree;
        }
        bool IsBoomer() const { return this->boomerClassification; }

        bool IsCirrhotic() {
            if (this->GetFibrosisState() == FibrosisState::F4 ||
                this->GetFibrosisState() == FibrosisState::DECOMP) {
                return true;
            }
            return false;
        }
        ////////////// CHECKS /////////////////
        ////////////// GETTERS ////////////////

        int GetCurrentTimestep() const { return this->_currentTime; }

        int GetAge() const { return this->age; }

        /// @brief Getter for the number of HCV infections experienced by Person
        /// @return Number of HCV infections experienced by Person
        int GetTimesInfected() const {
            return this->infectionStatus.timesInfected;
        }

        /// @brief Get the running total of clearances for Person
        int GetClearances() const {
            return this->infectionStatus.timesCleared;
        };

        /// @brief
        int GetNumABTests() const { return this->screeningDetails.numRNATests; }

        /// @brief
        int GetNumRNATests() const {
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

        /// @brief Getter for the Fibrosis State
        /// @return The Current Fibrosis State
        FibrosisState GetFibrosisState() const {
            return this->infectionStatus.fibrosisState;
        }

        /// @brief Getter for the HCV State
        /// @return HCV State
        HCV GetHCV() const { return this->infectionStatus.hcv; }

        /// @brief Getter for Alive Status
        /// @return Alive Status
        bool GetIsAlive() const { return this->isAlive; }

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
        int GetTimeHCVChanged() const {
            return this->infectionStatus.timeHCVChanged;
        }

        /// @brief Getter for Time since Fibrosis State Change
        /// @return Time Since Fibrosis State Change
        int GetTimeFibrosisStateChanged() const {
            return this->infectionStatus.timeFibrosisStateChanged;
        }

        /// @brief Getter for Seropositive
        /// @return Seropositive
        bool GetSeropositive() const {
            return this->infectionStatus.seropositive;
        }

        int GetTimeIdentified() const {
            return this->infectionStatus.timeIdentified;
        }

        /// @brief Getter for Link State
        /// @return Link State
        LinkageState GetLinkState() const { return this->linkStatus.linkState; }

        /// @brief Getter for Timestep in which linkage changed
        /// @return Time Link Change
        int GetTimeOfLinkChange() const {
            return this->linkStatus.timeOfLinkChange;
        }

        /// @brief Getter for link count
        /// @return Number of times Person has linked to care
        int GetLinkCount() const { return this->linkStatus.linkCount; }

        /// @brief Getter for Linkage Type
        /// @return Linkage Type
        LinkageType GetLinkageType() const { return this->linkStatus.linkType; }

        /// @brief Getter for the number of timesteps Person has been in
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

        /// @brief Getter for number of infants
        /// @return Number of infants born to this person
        int GetNumInfants() const { return this->pregnancyDetails.numInfants; }

        /// @brief Getter for number of miscarriages
        /// @return Number of miscarriages this person has experienced
        int GetNumMiscarriages() const {
            return this->pregnancyDetails.numMiscarriages;
        }

        /// @brief Getter for timestep in which the last fibrosis
        /// staging test happened
        /// @return Timestep of person's last fibrosis staging
        int GetTimeOfLastStaging() const {
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
        /// @return Person's sex
        Sex GetSex() const { return this->sex; }
        /// @brief Getter for the person's stratified utilities
        /// @return Person's stratified utilities
        UtilityTracker GetUtility() const { return this->utilityTracker; }

        /// @brief Getter for the Person's costs
        /// @return cost::CostTracker containing this person's costs
        cost::CostTracker GetCosts() const { return this->costs; }

        Health GetHealth() const { return this->infectionStatus; }

        BehaviorDetails GetBehaviorDetails() const {
            return this->behaviorDetails;
        }

        LinkageDetails GetLinkStatus() const { return this->linkStatus; }

        MOUDDetails GetMOUDDetails() const { return this->moudDetails; }

        PregnancyDetails GetPregnancyDetails() const {
            return this->pregnancyDetails;
        }

        StagingDetails GetStagingDetails() const {
            return this->stagingDetails;
        }

        ScreeningDetails GetScreeningDetails() const {
            return this->screeningDetails;
        }

        TreatmentDetails GetTreatmentDetails() const {
            return this->treatmentDetails;
        }

        //////////////////// GETTERS ////////////////////
        //////////////////// SETTERS ////////////////////

        void SetAge(int age) { this->age = age; }

        /// @brief Set whether the person experienced fibtest two this cycle
        /// @param state New value of hadSecondTest
        void SetHadSecondTest(bool state) {
            this->stagingDetails.hadSecondTest = state;
        }

        /// @brief Set the Seropositive value
        /// @param seropositive Seropositive status to set
        void SetSeropositive(bool seropositive) {
            this->infectionStatus.seropositive = seropositive;
        }

        /// @brief Set HEPC State -- used to change to chronic infection
        /// @param New HEPC State
        void SetHCV(HCV hcvs) { this->infectionStatus.hcv = hcvs; }

        /// @brief Setter for Person's treatment initiation state
        /// @param incompleteTreatment Boolean value for initiated treatment
        /// state to be set
        void SetInitiatedTreatment(bool initiatedTreatment) {
            this->treatmentDetails.timeOfTreatmentInitiation =
                this->_currentTime;
            this->treatmentDetails.initiatedTreatment = initiatedTreatment;
        }

        /// @brief Setter for Pregnancy State
        /// @param state Person's pregnancy State
        void SetPregnancyState(PregnancyState state) {
            this->pregnancyDetails.timeOfPregnancyChange = this->_currentTime;
            this->pregnancyDetails.pregnancyState = state;
        }

        /// @brief Add infants to the count
        /// @param infants Number infants to add
        void SetNumInfants(int infants) {
            this->pregnancyDetails.numInfants += infants;
        }

        /// @brief Add miscarriages to the count
        /// @param miscarriages Number of miscarriages to add
        void SetNumMiscarriages(int miscarriages) {
            this->pregnancyDetails.numMiscarriages += miscarriages;
        }

        /// @brief Set Person's measured fibrosis state
        /// @param state
        void SetMeasuredFibrosisState(MeasuredFibrosisState state) {
            this->stagingDetails.measuredFibrosisState = state;
        }

        /// @brief Setter for whether Person is genotype three
        /// @param genotype True if infection is genotype three, false
        /// otherwise
        void SetGenotype(bool genotype) {
            this->infectionStatus.isGenotypeThree = genotype;
        }

        /// @brief Setter for MOUD State
        /// @param moud Person's new MOUD state
        void SetMoudState(MOUD moud) {
            this->moudDetails.timeStartedMoud = this->_currentTime;
            this->moudDetails.moudState = moud;
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

        void SetBoomerClassification(bool status) {
            this->boomerClassification = status;
        }

        //////////////////// SETTERS ////////////////////
    };

    int count = 0;

    std::string sqlQuery = "";
    std::ostream &operator<<(std::ostream &os, const PersonBase &person) {
        os << "sex: " << person.GetSex() << std::endl;
        os << "alive: " << std::boolalpha << person.GetIsAlive() << std::endl;
        os << person.GetHealth() << std::endl;
        os << person.GetBehaviorDetails() << std::endl;
        os << person.GetLinkStatus() << std::endl;
        os << "Overdoses: " << person.GetNumberOfOverdoses() << std::endl;
        os << person.GetMOUDDetails() << std::endl;
        os << person.GetPregnancyDetails() << std::endl;
        os << person.GetStagingDetails() << std::endl;
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

    PersonBase::PersonBase() {
        pImplPERSON = std::make_shared<person::PersonBase::Person>();
    }
    PersonBase::~PersonBase() = default;

    int PersonBase::Grow() {
        pImplPERSON->Grow();
        return 0;
    }
    int PersonBase::Die() {
        pImplPERSON->Die();
        return 0;
    }
    int PersonBase::Infect() {
        pImplPERSON->Infect();
        return 0;
    }
    int PersonBase::ClearHCV() {
        pImplPERSON->ClearHCV();
        return 0;
    }
    int PersonBase::UpdateFibrosis(const FibrosisState &ls) {
        pImplPERSON->UpdateFibrosis(ls);
        return 0;
    }
    int PersonBase::UpdateBehavior(const Behavior &bc) {
        pImplPERSON->UpdateBehavior(bc);
        return 0;
    }
    int PersonBase::DiagnoseFibrosis(FibrosisState &data) {
        data = pImplPERSON->DiagnoseFibrosis();
        return 0;
    }
    int PersonBase::DiagnoseHEPC(HCV &data) {
        data = pImplPERSON->DiagnoseHEPC();
        return 0;
    }
    int PersonBase::AddClearance() {
        pImplPERSON->AddClearance();
        return 0;
    }
    int PersonBase::MarkScreened() {
        pImplPERSON->MarkScreened();
        return 0;
    }
    int PersonBase::AddAbScreen() {
        pImplPERSON->AddAbScreen();
        return 0;
    }
    int PersonBase::AddRnaScreen() {
        pImplPERSON->AddRnaScreen();
        return 0;
    }
    int PersonBase::Unlink() {
        pImplPERSON->Unlink();
        return 0;
    }
    int PersonBase::Link(LinkageType linkType) {
        pImplPERSON->Link(linkType);
        return 0;
    }
    int PersonBase::IdentifyAsInfected() {
        pImplPERSON->IdentifyAsInfected();
        return 0;
    }
    int PersonBase::AddCost(cost::Cost cost) {
        pImplPERSON->AddCost(cost);
        return 0;
    }
    int PersonBase::ToggleOverdose() {
        pImplPERSON->ToggleOverdose();
        return 0;
    }
    // Checks
    bool PersonBase::IsAlive() { return pImplPERSON->GetIsAlive(); }
    bool PersonBase::HadSecondTest() { return pImplPERSON->HadSecondTest(); }
    bool PersonBase::IsIdentifiedAsInfected() const {
        return pImplPERSON->IsIdentifiedAsInfected();
    }
    bool PersonBase::HasInitiatedTreatment() const {
        return pImplPERSON->HasInitiatedTreatment();
    }
    bool PersonBase::IsGenotypeThree() const {
        return pImplPERSON->IsGenotypeThree();
    }
    bool PersonBase::IsBoomer() const { return pImplPERSON->IsBoomer(); }
    bool PersonBase::IsCirrhotic() { return pImplPERSON->IsCirrhotic(); }
    // Getters
    int PersonBase::GetAge() const { return pImplPERSON->GetAge(); }
    int PersonBase::GetTimesInfected() const {
        return pImplPERSON->GetTimesInfected();
    }
    int PersonBase::GetClearances() const {
        return pImplPERSON->GetClearances();
    }
    int PersonBase::GetNumABTests() const {
        return pImplPERSON->GetNumABTests();
    }
    int PersonBase::GetNumRNATests() const {
        return pImplPERSON->GetNumRNATests();
    }
    int PersonBase::GetTimeOfLastScreening() const {
        return pImplPERSON->GetTimeOfLastScreening();
    }
    bool PersonBase::GetCurrentlyOverdosing() const {
        return pImplPERSON->GetCurrentlyOverdosing();
    }
    int PersonBase::GetNumberOfOverdoses() const {
        return pImplPERSON->GetNumberOfOverdoses();
    }
    FibrosisState PersonBase::GetFibrosisState() const {
        return pImplPERSON->GetFibrosisState();
    }
    HCV PersonBase::GetHCV() const { return pImplPERSON->GetHCV(); }
    bool PersonBase::GetIsAlive() const { return pImplPERSON->GetIsAlive(); }
    Behavior PersonBase::GetBehavior() const {
        return pImplPERSON->GetBehavior();
    }
    int PersonBase::GetTimeBehaviorChange() const {
        return pImplPERSON->GetTimeBehaviorChange();
    }
    int PersonBase::GetTimeHCVChanged() const {
        return pImplPERSON->GetTimeHCVChanged();
    }

    int PersonBase::GetTimeSinceHCVChanged() const {
        return pImplPERSON->GetCurrentTimestep() - GetTimeHCVChanged();
    }

    int PersonBase::GetTimeFibrosisStateChanged() const {
        return pImplPERSON->GetTimeFibrosisStateChanged();
    }
    bool PersonBase::GetSeropositive() const {
        return pImplPERSON->GetSeropositive();
    }
    int PersonBase::GetTimeIdentified() const {
        return pImplPERSON->GetTimeIdentified();
    }
    LinkageState PersonBase::GetLinkState() const {
        return pImplPERSON->GetLinkState();
    }
    int PersonBase::GetTimeOfLinkChange() const {
        return pImplPERSON->GetTimeOfLinkChange();
    }
    int PersonBase::GetLinkCount() const { return pImplPERSON->GetLinkCount(); }
    LinkageType PersonBase::GetLinkageType() const {
        return pImplPERSON->GetLinkageType();
    }
    int PersonBase::GetTimeOfTreatmentInitiation() const {
        return pImplPERSON->GetTimeOfTreatmentInitiation();
    }
    PregnancyState PersonBase::GetPregnancyState() const {
        return pImplPERSON->GetPregnancyState();
    }
    int PersonBase::GetTimeOfPregnancyChange() const {
        return pImplPERSON->GetTimeOfPregnancyChange();
    }
    int PersonBase::GetNumInfants() const {
        return pImplPERSON->GetNumInfants();
    }
    int PersonBase::GetNumMiscarriages() const {
        return pImplPERSON->GetNumMiscarriages();
    }
    int PersonBase::GetTimeOfLastStaging() const {
        return pImplPERSON->GetTimeOfLastStaging();
    }
    int PersonBase::GetTimeSinceStaging() const {
        return pImplPERSON->GetCurrentTimestep() -
               pImplPERSON->GetTimeOfLastStaging();
    }
    MOUD PersonBase::GetMoudState() const {
        return pImplPERSON->GetMoudState();
    }
    MeasuredFibrosisState PersonBase::GetMeasuredFibrosisState() const {
        return pImplPERSON->GetMeasuredFibrosisState();
    }
    int PersonBase::GetTimeStartedMoud() const {
        return pImplPERSON->GetTimeStartedMoud();
    }
    Sex PersonBase::GetSex() const { return pImplPERSON->GetSex(); }
    UtilityTracker PersonBase::GetUtility() const {
        return pImplPERSON->GetUtility();
    }
    cost::CostTracker PersonBase::GetCosts() const {
        return pImplPERSON->GetCosts();
    }
    Health PersonBase::GetHealth() const { return pImplPERSON->GetHealth(); }
    BehaviorDetails PersonBase::GetBehaviorDetails() const {
        return pImplPERSON->GetBehaviorDetails();
    }
    LinkageDetails PersonBase::GetLinkStatus() const {
        return pImplPERSON->GetLinkStatus();
    }
    MOUDDetails PersonBase::GetMOUDDetails() const {
        return pImplPERSON->GetMOUDDetails();
    }
    PregnancyDetails PersonBase::GetPregnancyDetails() const {
        return pImplPERSON->GetPregnancyDetails();
    }
    StagingDetails PersonBase::GetStagingDetails() const {
        return pImplPERSON->GetStagingDetails();
    }
    ScreeningDetails PersonBase::GetScreeningDetails() const {
        return pImplPERSON->GetScreeningDetails();
    }
    TreatmentDetails PersonBase::GetTreatmentDetails() const {
        return pImplPERSON->GetTreatmentDetails();
    }
    // Setters
    void PersonBase::SetAge(int age) { pImplPERSON->SetAge(age); }
    void PersonBase::SetHadSecondTest(bool state) {
        pImplPERSON->SetHadSecondTest(state);
    }
    void PersonBase::SetSeropositive(bool seropositive) {
        pImplPERSON->SetSeropositive(seropositive);
    }
    void PersonBase::SetHCV(HCV hcvs) { pImplPERSON->SetHCV(hcvs); }
    void PersonBase::SetInitiatedTreatment(bool initiatedTreatment) {
        pImplPERSON->SetInitiatedTreatment(initiatedTreatment);
    }
    void PersonBase::SetPregnancyState(PregnancyState state) {
        pImplPERSON->SetPregnancyState(state);
    }
    void PersonBase::SetNumInfants(int infants) {
        pImplPERSON->SetNumInfants(infants);
    }
    void PersonBase::SetNumMiscarriages(int miscarriages) {
        pImplPERSON->SetNumMiscarriages(miscarriages);
    }
    void PersonBase::SetMeasuredFibrosisState(MeasuredFibrosisState state) {
        pImplPERSON->SetMeasuredFibrosisState(state);
    }
    void PersonBase::SetGenotype(bool genotype) {
        pImplPERSON->SetGenotype(genotype);
    }
    void PersonBase::SetMoudState(MOUD moud) {
        pImplPERSON->SetMoudState(moud);
    }
    void PersonBase::SetUtility(double util) { pImplPERSON->SetUtility(util); }
    void PersonBase::SetBoomerClassification(bool status) {
        pImplPERSON->SetBoomerClassification(status);
    }

} // namespace person
