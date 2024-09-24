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
    class PersonBase {
    public:
        // Functionality
        virtual int CreatePersonFromTable(
            int id, std::shared_ptr<datamanagement::DataManagerBase> dm) = 0;
        virtual int Grow() = 0;
        virtual int Die(DeathReason deathReason = DeathReason::BACKGROUND) = 0;

        // HCV
        virtual int InfectHCV() = 0;
        virtual int ClearHCV() = 0;
        virtual HCV GetHCV() const = 0;
        virtual void SetHCV(HCV hcv) = 0;
        virtual int AddHCVClearance() = 0;
        virtual int GetHCVClearances() const = 0;
        virtual bool GetSeropositivity() const = 0;
        virtual void SetSeropositivity(bool seropositive) = 0;

        virtual int DiagnoseHCV() = 0;
        virtual bool IsIdentifiedAsHCVInfected() const = 0;

        virtual int GetTimeHCVIdentified() const = 0;
        virtual int GetTimesHCVInfected() const = 0;
        virtual int GetTimeHCVChanged() const = 0;
        virtual int GetTimeSinceHCVChanged() const = 0;

        // Screening
        virtual int MarkScreened() = 0;
        virtual int AddAbScreen() = 0;
        virtual int AddRnaScreen() = 0;
        virtual bool HadSecondScreeningTest() const = 0;
        virtual int GetNumberOfABTests() const = 0;
        virtual int GetNumberOfRNATests() const = 0;
        virtual int GetTimeOfLastScreening() const = 0;
        virtual int GetTimeSinceLastScreening() const = 0;
        virtual ScreeningDetails GetScreeningDetails() const = 0;
        virtual void GiveSecondScreeningTest(bool state) = 0;

        // Linking
        virtual int Unlink() = 0;
        virtual int Link(LinkageType linkType) = 0;
        virtual LinkageState GetLinkState() const = 0;
        virtual int GetTimeOfLinkChange() const = 0;
        virtual int GetTimeSinceLinkChange() const = 0;
        virtual int GetLinkCount() const = 0;
        virtual LinkageType GetLinkageType() const = 0;
        virtual LinkageDetails GetLinkStatus() const = 0;

        // Treatment
        virtual int AddWithdrawal() = 0;
        virtual int GetWithdrawals() const = 0;
        virtual int AddToxicReaction() = 0;
        virtual int GetToxicReactions() const = 0;
        virtual int AddCompletedTreatment() = 0;
        virtual int GetCompletedTreatments() const = 0;
        virtual int AddSVR() = 0;
        virtual int GetSVRs() const = 0;
        virtual TreatmentDetails GetTreatmentDetails() const = 0;
        virtual void InitiateTreatment() = 0;

        virtual bool HasInitiatedTreatment() const = 0;

        virtual int GetTimeOfTreatmentInitiation() const = 0;
        virtual int GetTimeSinceTreatmentInitiation() const = 0;

        // Drug Use Behavior
        virtual int SetBehavior(const Behavior &bc) = 0;
        virtual Behavior GetBehavior() const = 0;
        virtual BehaviorDetails GetBehaviorDetails() const = 0;

        virtual int ToggleOverdose() = 0;
        virtual bool GetCurrentlyOverdosing() const = 0;

        virtual int GetNumberOfOverdoses() const = 0;
        virtual int GetTimeBehaviorChange() const = 0;

        // Fibrosis
        virtual MeasuredFibrosisState GetMeasuredFibrosisState() const = 0;
        virtual int DiagnoseFibrosis(MeasuredFibrosisState &data) = 0;
        virtual FibrosisState GetTrueFibrosisState() const = 0;
        virtual void UpdateTrueFibrosis(FibrosisState state) = 0;

        virtual StagingDetails GetFibrosisStagingDetails() const = 0;

        virtual int GetTimeTrueFibrosisStateChanged() const = 0;
        virtual int GetTimeOfFibrosisStaging() const = 0;
        virtual int GetTimeSinceFibrosisStaging() const = 0;

        // Cost Effectiveness
        virtual int AddCost(cost::Cost cost) = 0;
        virtual void SetUtility(double util) = 0;

        // General Data Handling
        virtual int LoadICValues(std::string icValues) = 0;
        virtual bool IsAlive() const = 0;
        virtual bool IsGenotypeThree() const = 0;
        virtual void SetGenotypeThree(bool genotype) = 0;
        virtual bool IsBoomer() const = 0;
        virtual void SetBoomer(bool status) = 0;
        virtual void SetDeathReason(DeathReason deathReason) = 0;
        virtual DeathReason GetDeathReason() const = 0;
        virtual int GetAge() const = 0;
        virtual void SetAge(int age) = 0;

        virtual bool IsCirrhotic() = 0;

        virtual std::string GetPersonDataString() const = 0;
        virtual int GetID() const = 0;
        virtual int GetCurrentTimestep() const = 0;
        virtual Sex GetSex() const = 0;
        virtual UtilityTracker GetUtility() const = 0;
        virtual cost::CostTracker GetCosts() const = 0;
        virtual Health GetHealth() const = 0;

        // TODO
        virtual PregnancyState GetPregnancyState() const = 0;
        virtual int GetTimeOfPregnancyChange() const = 0;
        virtual int GetTimeSincePregnancyChange() const = 0;
        virtual int GetNumMiscarriages() const = 0;
        virtual int Miscarry() = 0;
        virtual int Stillbirth() = 0;
        virtual int AddChild(HCV hcv, bool test) = 0;
        virtual int EndPostpartum() = 0;
        virtual int Impregnate() = 0;
        virtual PregnancyDetails GetPregnancyDetails() const = 0;
        virtual void SetPregnancyState(PregnancyState state) = 0;
        virtual void SetNumMiscarriages(int miscarriages) = 0;

        virtual void SetMoudState(MOUD moud) = 0;
        virtual MOUD GetMoudState() const = 0;
        virtual int GetTimeStartedMoud() const = 0;
        virtual MOUDDetails GetMOUDDetails() const = 0;
        virtual std::vector<Child> GetChildren() const = 0;
    };
    /// @brief Class describing a Person
    class Person : PersonBase {
    private:
        class PersonIMPL;
        std::unique_ptr<PersonIMPL> pImplPERSON;

    public:
        Person();
        ~Person();

        // Copy Operations
        Person(Person const &) = delete;
        Person &operator=(Person const &) = delete;
        Person(Person &&) noexcept;
        Person &operator=(Person &&) noexcept;

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
        void UpdateTrueFibrosis(FibrosisState state);

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
        int GetTimeSincePregnancyChange() const;
        int GetNumMiscarriages() const;
        int Miscarry();
        int Stillbirth();
        int AddChild(HCV hcv, bool test);
        int EndPostpartum();
        int Impregnate();
        PregnancyDetails GetPregnancyDetails() const;
        void SetPregnancyState(PregnancyState state);
        void SetNumMiscarriages(int miscarriages);

        void SetMoudState(MOUD moud);
        MOUD GetMoudState() const;
        int GetTimeStartedMoud() const;
        MOUDDetails GetMOUDDetails() const;
        std::vector<Child> GetChildren() const;
    };
} // namespace person
#endif
