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
    class DataManagerBase;
}

/// @brief Namespace containing all code pertaining to an individual Person
namespace person {
    /// @brief Class describing a Person
    class PersonBase {
    private:
        class Person;
        std::unique_ptr<Person> pImplPERSON;

    public:
        PersonBase();
        ~PersonBase();

        // Copy Operations
        PersonBase(PersonBase const &) = delete;
        PersonBase &operator=(PersonBase const &) = delete;
        PersonBase(PersonBase &&) noexcept;
        PersonBase &operator=(PersonBase &&) noexcept;

        // Functionality
        int CreatePersonFromTable(
            int id, std::shared_ptr<datamanagement::DataManagerBase> dm);
        int Grow();
        int Die(DeathReason deathReason = DeathReason::BACKGROUND);

        // HCV
        int InfectHCV();
        int ClearHCV();
        HCV GetHCV() const;
        void SetHCV(HCV hcv);
        int AddHCVClearance();
        int GetHCVClearances() const;
        bool GetSeropositivity() const;
        void SetSeropositivity(bool seropositive);

        int DiagnoseHCV();
        bool IsIdentifiedAsHCVInfected() const;

        int GetTimeHCVIdentified() const;
        int GetTimesHCVInfected() const;
        int GetTimeHCVChanged() const;
        int GetTimeSinceHCVChanged() const;

        // Screening
        int MarkScreened();
        int AddAbScreen();
        int AddRnaScreen();
        bool HadSecondScreeningTest() const;
        int GetNumberOfABTests() const;
        int GetNumberOfRNATests() const;
        int GetTimeOfLastScreening() const;
        int GetTimeSinceLastScreening() const;
        ScreeningDetails GetScreeningDetails() const;
        void GiveSecondScreeningTest(bool state);

        // Linking
        int Unlink();
        int Link(LinkageType linkType);
        LinkageState GetLinkState() const;
        int GetTimeOfLinkChange() const;
        int GetTimeSinceLinkChange() const;
        int GetLinkCount() const;
        LinkageType GetLinkageType() const;
        LinkageDetails GetLinkStatus() const;

        // Treatment
        int AddWithdrawal();
        int GetWithdrawals() const;
        int AddToxicReaction();
        int GetToxicReactions() const;
        int AddCompletedTreatment();
        int GetCompletedTreatments() const;
        int AddSVR();
        int GetSVRs() const;
        TreatmentDetails GetTreatmentDetails() const;
        void InitiateTreatment();

        bool HasInitiatedTreatment() const;

        int GetTimeOfTreatmentInitiation() const;
        int GetTimeSinceTreatmentInitiation() const;

        // Drug Use Behavior
        int SetBehavior(const Behavior &bc);
        Behavior GetBehavior() const;
        BehaviorDetails GetBehaviorDetails() const;

        int ToggleOverdose();
        bool GetCurrentlyOverdosing() const;

        int GetNumberOfOverdoses() const;
        int GetTimeBehaviorChange() const;

        // Fibrosis
        MeasuredFibrosisState GetMeasuredFibrosisState() const;
        int DiagnoseFibrosis(MeasuredFibrosisState &data);
        FibrosisState GetTrueFibrosisState() const;
        void SetTrueFibrosisState(FibrosisState state);

        StagingDetails GetFibrosisStagingDetails() const;

        int GetTimeTrueFibrosisStateChanged() const;
        int GetTimeOfFibrosisStaging() const;
        int GetTimeSinceFibrosisStaging() const;

        // Cost Effectiveness
        int AddCost(cost::Cost cost);
        void SetUtility(double util);

        // General Data Handling
        int LoadICValues(std::string icValues);
        bool IsAlive() const;
        bool IsGenotypeThree() const;
        void SetGenotypeThree(bool genotype);
        bool IsBoomer() const;
        void SetBoomer(bool status);
        void SetDeathReason(DeathReason deathReason);
        DeathReason GetDeathReason() const;
        int GetAge() const;
        void SetAge(int age);

        bool IsCirrhotic();

        std::string GetPersonDataString() const;
        int GetID() const;
        int GetCurrentTimestep() const;
        Sex GetSex() const;
        UtilityTracker GetUtility() const;
        cost::CostTracker GetCosts() const;
        Health GetHealth() const;

        // TODO
        PregnancyState GetPregnancyState() const;
        int GetTimeOfPregnancyChange() const;
        int GetNumInfants() const;
        int GetNumMiscarriages() const;
        MOUD GetMoudState() const;
        int GetTimeStartedMoud() const;
        MOUDDetails GetMOUDDetails() const;
        PregnancyDetails GetPregnancyDetails() const;
        void SetPregnancyState(PregnancyState state);
        void SetNumInfants(int infants);
        void SetNumMiscarriages(int miscarriages);
        void SetMoudState(MOUD moud);
    };
} // namespace person
#endif
