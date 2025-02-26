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
#include "Utility.hpp"
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
        virtual int ClearHCVDiagnosis() = 0;
        virtual bool IsIdentifiedAsHCVInfected() const = 0;
        virtual bool HistoryOfHCVInfection() const = 0;

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
        virtual void SetLinkageType(LinkageType) = 0;
        virtual LinkageType GetLinkageType() const = 0;
        virtual LinkageDetails GetLinkStatus() const = 0;

        // Treatment
        virtual int AddWithdrawal() = 0;
        virtual int GetWithdrawals() const = 0;
        virtual int AddToxicReaction() = 0;
        virtual int GetToxicReactions() const = 0;
        virtual int AddCompletedTreatment() = 0;
        virtual int GetCompletedTreatments() const = 0;
        virtual int GetRetreatments() const = 0;
        virtual int AddSVR() = 0;
        virtual int GetSVRs() const = 0;
        virtual TreatmentDetails GetTreatmentDetails() const = 0;
        virtual int GetNumberOfTreatmentStarts() const = 0;
        virtual void InitiateTreatment() = 0;
        virtual int EndTreatment() = 0;

        virtual bool HasInitiatedTreatment() const = 0;
        virtual bool IsInRetreatment() const = 0;

        virtual int GetTimeOfTreatmentInitiation() const = 0;
        virtual int GetTimeSinceTreatmentInitiation() const = 0;

        // Drug Use Behavior
        virtual int SetBehavior(Behavior) = 0;
        virtual Behavior GetBehavior() const = 0;
        virtual BehaviorDetails GetBehaviorDetails() const = 0;

        virtual int ToggleOverdose() = 0;
        virtual bool GetCurrentlyOverdosing() const = 0;

        virtual int GetNumberOfOverdoses() const = 0;
        virtual int GetTimeBehaviorChange() const = 0;

        // Fibrosis
        virtual MeasuredFibrosisState GetMeasuredFibrosisState() const = 0;
        virtual int DiagnoseFibrosis(MeasuredFibrosisState) = 0;
        virtual FibrosisState GetTrueFibrosisState() const = 0;
        virtual void UpdateTrueFibrosis(FibrosisState) = 0;

        virtual StagingDetails GetFibrosisStagingDetails() const = 0;

        virtual int GetTimeTrueFibrosisStateChanged() const = 0;
        virtual int GetTimeOfFibrosisStaging() const = 0;
        virtual int GetTimeSinceFibrosisStaging() const = 0;

        // Cost Effectiveness
        virtual int AddCost(double base_cost, double discount_cost,
                            cost::CostCategory category) = 0;

        // Life, Quality of Life
        virtual LifetimeUtility GetTotalUtility() const = 0;
        virtual void
        AccumulateTotalUtility(std::pair<double, double> util,
                               std::pair<double, double> discount_util) = 0;
        virtual std::pair<double, double> GetUtility() const = 0;
        virtual void SetUtility(double util,
                                utility::UtilityCategory category) = 0;
        virtual int GetLifeSpan() const = 0;
        virtual double GetDiscountedLifeSpan() const = 0;
        virtual void AddDiscountedLifeSpan(double discounted_life) = 0;

        // General Data Handling
        virtual int LoadICValues(int id, std::vector<std::string> icValues) = 0;
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
        virtual std::unordered_map<cost::CostCategory,
                                   std::pair<double, double>>
        GetCosts() const = 0;
        virtual std::pair<double, double> GetCostTotals() const = 0;
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

        virtual void TransitionMOUD() = 0;
        virtual MOUD GetMoudState() const = 0;
        virtual int GetTimeStartedMoud() const = 0;
        virtual MOUDDetails GetMOUDDetails() const = 0;
        virtual std::vector<Child> GetChildren() const = 0;

        // HCC
        virtual void DevelopHCC(HCCState state) = 0;
        virtual HCCState GetHCCState() const = 0;
        virtual void DiagnoseHCC() = 0;
        virtual bool IsDiagnosedWithHCC() const = 0;
    };
    /// @brief Class describing a Person
    class Person : public PersonBase {
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
        int ClearHCVDiagnosis();
        bool IsIdentifiedAsHCVInfected() const;
        bool HistoryOfHCVInfection() const;

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
        void SetLinkageType(LinkageType linkType);
        LinkageType GetLinkageType() const;
        LinkageDetails GetLinkStatus() const;

        // Treatment
        int AddWithdrawal();
        int GetWithdrawals() const;
        int AddToxicReaction();
        int GetToxicReactions() const;
        int AddCompletedTreatment();
        int GetCompletedTreatments() const;
        int GetRetreatments() const;
        int AddSVR();
        int GetSVRs() const;
        TreatmentDetails GetTreatmentDetails() const;
        int GetNumberOfTreatmentStarts() const;
        void InitiateTreatment();
        int EndTreatment();

        bool HasInitiatedTreatment() const;
        bool IsInRetreatment() const;

        int GetTimeOfTreatmentInitiation() const;
        int GetTimeSinceTreatmentInitiation() const;

        // Drug Use Behavior
        int SetBehavior(Behavior);
        Behavior GetBehavior() const;
        BehaviorDetails GetBehaviorDetails() const;

        int ToggleOverdose();
        bool GetCurrentlyOverdosing() const;

        int GetNumberOfOverdoses() const;
        int GetTimeBehaviorChange() const;

        // Fibrosis
        MeasuredFibrosisState GetMeasuredFibrosisState() const;
        int DiagnoseFibrosis(MeasuredFibrosisState);
        FibrosisState GetTrueFibrosisState() const;
        void UpdateTrueFibrosis(FibrosisState);

        StagingDetails GetFibrosisStagingDetails() const;

        int GetTimeTrueFibrosisStateChanged() const;
        int GetTimeOfFibrosisStaging() const;
        int GetTimeSinceFibrosisStaging() const;

        // Cost Effectiveness
        int AddCost(double base_cost, double discount_cost,
                    cost::CostCategory category = cost::CostCategory::MISC);

        // General Data Handling
        int LoadICValues(int id, std::vector<std::string> icValues);
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
        std::unordered_map<cost::CostCategory, std::pair<double, double>>
        GetCosts() const;
        std::pair<double, double> GetCostTotals() const override;
        Health GetHealth() const;

        void AccumulateTotalUtility(std::pair<double, double> util,
                                    std::pair<double, double> discount_util);
        void SetUtility(double util, utility::UtilityCategory category);
        std::pair<double, double> GetUtility() const;
        LifetimeUtility GetTotalUtility() const;
        int GetLifeSpan() const;
        double GetDiscountedLifeSpan() const;
        void AddDiscountedLifeSpan(double discounted_life);

        // Pregnancy
        PregnancyState GetPregnancyState() const;
        int GetTimeOfPregnancyChange() const;
        int GetTimeSincePregnancyChange() const;
        int GetNumMiscarriages() const;
        int Miscarry();
        int Stillbirth();
        std::vector<Child> GetChildren() const;
        int AddChild(HCV hcv, bool test);
        int EndPostpartum();
        int Impregnate();
        PregnancyDetails GetPregnancyDetails() const;
        void SetPregnancyState(PregnancyState state);
        void SetNumMiscarriages(int miscarriages);

        // MOUD
        void TransitionMOUD();
        MOUD GetMoudState() const;
        int GetTimeStartedMoud() const;
        MOUDDetails GetMOUDDetails() const;

        // HCC
        void DevelopHCC(HCCState state);
        HCCState GetHCCState() const;
        void DiagnoseHCC();
        bool IsDiagnosedWithHCC() const;
    };
} // namespace person
#endif
