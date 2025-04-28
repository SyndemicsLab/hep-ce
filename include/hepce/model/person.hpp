////////////////////////////////////////////////////////////////////////////////
// File: person.hpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_PERSON_HPP_
#define HEPCE_MODEL_PERSON_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include <datamanagement/datamanagement.hpp>

#include <hepce/data/types.hpp>
#include <hepce/model/cost.hpp>
#include <hepce/model/utility.hpp>

namespace hepce {
namespace model {
class Person {
public:
    // Functionality
    virtual int Grow() = 0;
    virtual int
    Die(data::DeathReason deathReason = data::DeathReason::kBackground) = 0;

    // HCV
    virtual int InfectHCV() = 0;
    virtual int ClearHCV(bool is_acute = false) = 0;
    virtual data::HCV GetHCV() const = 0;
    virtual void SetHCV(data::HCV hcv) = 0;
    virtual int AddAcuteHCVClearance() = 0;
    virtual int GetAcuteHCVClearances() const = 0;
    virtual bool GetSeropositivity() const = 0;
    virtual void SetSeropositivity(bool seropositive_state) = 0;

    virtual void Diagnose(data::InfectionType it) = 0;
    virtual void ClearDiagnosis(data::InfectionType it) = 0;
    virtual bool IsIdentifiedAsInfected(data::InfectionType it) const = 0;

    virtual int GetTimeHCVIdentified() const = 0;
    virtual int GetTimesHCVInfected() const = 0;
    virtual int GetTimeHCVChanged() const = 0;
    virtual int GetTimeSinceHCVChanged() const = 0;

    // Screening
    virtual int MarkScreened(data::InfectionType it) = 0;
    virtual int AddAbScreen(data::InfectionType it) = 0;
    virtual int AddRnaScreen(data::InfectionType it) = 0;
    virtual bool HadSecondScreeningTest() const = 0;
    virtual int GetNumberOfABTests(data::InfectionType it) const = 0;
    virtual int GetNumberOfRNATests(data::InfectionType it) const = 0;
    virtual int GetTimeOfLastScreening(data::InfectionType it) const = 0;
    virtual int GetTimeSinceLastScreening(data::InfectionType it) const = 0;
    virtual data::ScreeningDetails
    GetScreeningDetails(data::InfectionType it) const = 0;
    virtual bool CheckAntibodyPositive(data::InfectionType it) const = 0;
    virtual void SetAntibodyPositive(bool result = true,
                                     data::InfectionType it) = 0;
    virtual void GiveSecondScreeningTest(bool state) = 0;

    // Linking
    virtual int Unlink(data::InfectionType it) = 0;
    virtual int Link(data::LinkageType link_type, data::InfectionType it) = 0;
    virtual data::LinkageState GetLinkState(data::InfectionType it) const = 0;
    virtual int GetTimeOfLinkChange(data::InfectionType it) const = 0;
    virtual int GetTimeSinceLinkChange(data::InfectionType it) const = 0;
    virtual int GetLinkCount(data::InfectionType it) const = 0;
    virtual void SetLinkageType(data::LinkageType link_type,
                                data::InfectionType it) = 0;
    virtual data::LinkageType GetLinkageType(data::InfectionType it) const = 0;
    virtual data::LinkageDetails
    GetLinkStatus(data::InfectionType it) const = 0;

    // Treatment
    virtual int AddWithdrawal(data::InfectionType it) = 0;
    virtual int GetWithdrawals(data::InfectionType it) const = 0;
    virtual int AddToxicReaction(data::InfectionType it) = 0;
    virtual int GetToxicReactions(data::InfectionType it) const = 0;
    virtual int AddCompletedTreatment(data::InfectionType it) = 0;
    virtual int GetCompletedTreatments(data::InfectionType it) const = 0;
    virtual int GetRetreatments(data::InfectionType it) const = 0;
    virtual int AddSVR() = 0;
    virtual int GetSVRs() const = 0;
    virtual data::TreatmentDetails
    GetTreatmentDetails(data::InfectionType it) const = 0;
    virtual int GetNumberOfTreatmentStarts(data::InfectionType it) const = 0;
    virtual void InitiateTreatment(data::InfectionType it) = 0;
    virtual int EndTreatment(data::InfectionType it) = 0;
    virtual bool HasInitiatedTreatment(data::InfectionType it) const = 0;
    virtual bool IsInRetreatment(data::InfectionType it) const = 0;
    virtual int GetTimeOfTreatmentInitiation(data::InfectionType it) const = 0;
    virtual int
    GetTimeSinceTreatmentInitiation(data::InfectionType it) const = 0;

    // Drug Use Behavior
    virtual int SetBehavior(data::Behavior) = 0;
    virtual data::Behavior GetBehavior() const = 0;
    virtual data::BehaviorDetails GetBehaviorDetails() const = 0;

    virtual int ToggleOverdose() = 0;
    virtual bool GetCurrentlyOverdosing() const = 0;

    virtual int GetNumberOfOverdoses() const = 0;
    virtual int GetTimeBehaviorChange() const = 0;

    // Fibrosis
    virtual data::MeasuredFibrosisState GetMeasuredFibrosisState() const = 0;
    virtual int DiagnoseFibrosis(data::MeasuredFibrosisState) = 0;
    virtual data::FibrosisState GetTrueFibrosisState() const = 0;
    virtual void UpdateTrueFibrosis(data::FibrosisState) = 0;

    virtual data::StagingDetails GetFibrosisStagingDetails() const = 0;

    virtual int GetTimeTrueFibrosisStateChanged() const = 0;
    virtual int GetTimeOfFibrosisStaging() const = 0;
    virtual int GetTimeSinceFibrosisStaging() const = 0;

    // Cost Effectiveness
    virtual int AddCost(double base_cost, double discount_cost,
                        model::CostCategory category) = 0;

    // Life, Quality of Life
    virtual data::LifetimeUtility GetTotalUtility() const = 0;
    virtual void
    AccumulateTotalUtility(std::pair<double, double> util,
                           std::pair<double, double> discount_util) = 0;
    virtual std::pair<double, double> GetUtility() const = 0;
    virtual void SetUtility(double util, model::UtilityCategory category) = 0;
    virtual std::unordered_map<model::UtilityCategory, double>
    GetCurrentUtilities() const = 0;
    virtual int GetLifeSpan() const = 0;
    virtual double GetDiscountedLifeSpan() const = 0;
    virtual void AddDiscountedLifeSpan(double discounted_life) = 0;

    // General Data Handling
    virtual bool IsAlive() const = 0;
    virtual bool IsGenotypeThree() const = 0;
    virtual void SetGenotypeThree(bool genotype) = 0;
    virtual bool IsBoomer() const = 0;
    virtual void SetBoomer(bool status) = 0;
    virtual void SetDeathReason(data::DeathReason deathReason) = 0;
    virtual data::DeathReason GetDeathReason() const = 0;
    virtual int GetAge() const = 0;
    virtual void SetAge(int age) = 0;

    virtual bool IsCirrhotic() = 0;

    virtual std::string GetPersonDataString() const = 0;
    virtual int GetCurrentTimestep() const = 0;
    virtual data::Sex GetSex() const = 0;
    virtual std::unordered_map<model::CostCategory, std::pair<double, double>>
    GetCosts() const = 0;
    virtual std::pair<double, double> GetCostTotals() const = 0;
    virtual data::HCVDetails GetHCVDetails() const = 0;
    virtual data::HIVDetails GetHIVDetails() const = 0;
    virtual data::HCCDetails GetHCCDetails() const = 0;

    /// HIV
    // getter/setter
    virtual data::HIV GetHIV() const = 0;
    virtual void SetHIV(data::HIV hiv) = 0;
    // infection
    virtual void InfectHIV() = 0;
    virtual int GetTimeHIVChanged() const = 0;
    virtual int GetTimeSinceHIVChanged() const = 0;
    // outcomes
    virtual int GetLowCD4MonthCount() const = 0;

    // TODO
    virtual data::PregnancyState GetPregnancyState() const = 0;
    virtual int GetTimeOfPregnancyChange() const = 0;
    virtual int GetTimeSincePregnancyChange() const = 0;
    virtual int GetNumMiscarriages() const = 0;
    virtual int Miscarry() = 0;
    virtual int Stillbirth() = 0;
    virtual int AddChild(data::HCV hcv, bool test) = 0;
    virtual int EndPostpartum() = 0;
    virtual int Impregnate() = 0;
    virtual void AddInfantExposure() = 0;
    virtual data::PregnancyDetails GetPregnancyDetails() const = 0;
    virtual void SetPregnancyState(data::PregnancyState state) = 0;
    virtual void SetNumMiscarriages(int miscarriages) = 0;
    virtual int GetInfantHCVExposures() const = 0;
    virtual int GetInfantHCVInfections() const = 0;
    virtual int GetInfantHCVTests() const = 0;
    virtual int GetPregnancyCount() const = 0;

    virtual void TransitionMOUD() = 0;
    virtual data::MOUD GetMoudState() const = 0;
    virtual int GetTimeStartedMoud() const = 0;
    virtual data::MOUDDetails GetMOUDDetails() const = 0;
    virtual std::vector<data::Child> GetChildren() const = 0;

    // HCC
    virtual void DevelopHCC(data::HCCState state) = 0;
    virtual data::HCCState GetHCCState() const = 0;
    virtual void DiagnoseHCC() = 0;
    virtual bool IsDiagnosedWithHCC() const = 0;

    // Person Output
    virtual std::string MakePopulationRow() const = 0;

    static std::unique_ptr<Person>
    Create(const std::string &log_name = "console");
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_PERSON_HPP_