//===-- Person.hpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the HEP-CE, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Wednesday, August 2nd 2023, 9:14:16 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef PERSON_PERSON_HPP_
#define PERSON_PERSON_HPP_

#include "Containers.hpp"
#include "Cost.hpp"
#include <memory>

namespace datamanagement {
    class DataManager;
}

/// @brief Namespace containing all code pertaining to an individual Person
namespace person {
    /// @brief Class describing a Person
    class PersonBase {
    private:
        class Person;
        std::shared_ptr<Person> pImplPERSON;

    public:
        inline static const std::string POPULATION_HEADERS =
            "sex,age,isAlive,deathReason,identifiedHCV,timeInfectionIdentified,"
            "HCV,"
            "fibrosisState,isGenotypeThree,seropositive,timeHCVChanged,"
            "timeFibrosisStateChanged,drugBehavior,timeLastActiveDrugUse,"
            "linkageState,timeOfLinkChange,linkageType,"
            "linkCount,measuredFibrosisState,"
            "timeOfLastStaging,timeOfLastScreening,numABTests,"
            "numRNATests,timesInfected,timesCleared,initiatedTreatment,"
            "timeOfTreatmentInitiation,minUtility,multUtility,"
            "treatmentWithdrawals,treatmentToxicReactions,completedTreatments,"
            "svrs";

        PersonBase(int id, std::shared_ptr<datamanagement::DataManager> dm);
        ~PersonBase();
        // Functionality
        int Grow();
        int Die(DeathReason deathReason = DeathReason::BACKGROUND);
        int Infect();
        int ClearHCV();
        int UpdateFibrosis(const FibrosisState &ls);
        int UpdateBehavior(const Behavior &bc);
        int DiagnoseFibrosis(FibrosisState &data);
        int DiagnoseHEPC(HCV &data);
        int AddClearance();
        int AddWithdrawal();
        int AddToxicReaction();
        int AddCompletedTreatment();
        int AddSVR();
        int MarkScreened();
        int AddAbScreen();
        int AddRnaScreen();
        int Unlink();
        int Link(LinkageType linkType);
        int IdentifyAsInfected();
        int AddCost(cost::Cost cost);
        int ToggleOverdose();
        int LoadICValues(std::string icValues);
        // Checks
        bool IsAlive();
        bool HadSecondTest();
        bool IsIdentifiedAsInfected() const;
        bool HasInitiatedTreatment() const;
        bool IsGenotypeThree() const;
        bool IsBoomer() const;
        bool IsCirrhotic();
        // Getters
        int GetID() const;
        int GetCurrentTimestep() const;
        int GetAge() const;
        int GetTimesInfected() const;
        int GetClearances() const;
        int GetWithdrawals() const;
        int GetToxicReactions() const;
        int GetCompletedTreatments() const;
        int GetSVRs() const;
        int GetNumABTests() const;
        int GetNumRNATests() const;
        int GetTimeOfLastScreening() const;
        int GetTimeSinceScreened() const;
        bool GetCurrentlyOverdosing() const;
        int GetNumberOfOverdoses() const;
        DeathReason GetDeathReason() const;
        FibrosisState GetFibrosisState() const;
        HCV GetHCV() const;
        bool GetIsAlive() const;
        Behavior GetBehavior() const;
        int GetTimeBehaviorChange() const;
        int GetTimeHCVChanged() const;
        int GetTimeSinceHCVChanged() const;
        int GetTimeFibrosisStateChanged() const;
        int GetTimeSinceStaging() const;
        bool GetSeropositive() const;
        int GetTimeIdentified() const;
        LinkageState GetLinkState() const;
        int GetTimeOfLinkChange() const;
        int GetTimeSinceLinkChange() const;
        int GetLinkCount() const;
        LinkageType GetLinkageType() const;
        int GetTimeOfTreatmentInitiation() const;
        int GetTimeSinceTreatmentInitiation() const;
        PregnancyState GetPregnancyState() const;
        int GetTimeOfPregnancyChange() const;
        int GetNumInfants() const;
        int GetNumMiscarriages() const;
        int GetTimeOfLastStaging() const;
        MOUD GetMoudState() const;
        MeasuredFibrosisState GetMeasuredFibrosisState() const;
        int GetTimeStartedMoud() const;
        Sex GetSex() const;
        UtilityTracker GetUtility() const;
        cost::CostTracker GetCosts() const;
        Health GetHealth() const;
        BehaviorDetails GetBehaviorDetails() const;
        LinkageDetails GetLinkStatus() const;
        MOUDDetails GetMOUDDetails() const;
        PregnancyDetails GetPregnancyDetails() const;
        StagingDetails GetStagingDetails() const;
        ScreeningDetails GetScreeningDetails() const;
        TreatmentDetails GetTreatmentDetails() const;
        std::string GetPersonDataString() const;
        // Setters
        void SetAge(int age);
        void SetHadSecondTest(bool state);
        void SetSeropositive(bool seropositive);
        void SetDeathReason(DeathReason deathReason);
        void SetHCV(HCV hcvs);
        void SetInitiatedTreatment(bool initiatedTreatment);
        void SetPregnancyState(PregnancyState state);
        void SetNumInfants(int infants);
        void SetNumMiscarriages(int miscarriages);
        void SetMeasuredFibrosisState(MeasuredFibrosisState state);
        void SetGenotype(bool genotype);
        void SetMoudState(MOUD moud);
        void SetUtility(double util);
        void SetBoomerClassification(bool status);
    };
} // namespace person
#endif
