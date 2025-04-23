////////////////////////////////////////////////////////////////////////////////
// File: person_internals.hpp                                                 //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_PERSONINTERNALS_HPP_
#define HEPCE_MODEL_PERSONINTERNALS_HPP_

#include <hepce/model/person.hpp>

namespace hepce {
namespace model {
class PersonImpl : public Person {
public:
    PersonImpl(const std::string &log_name = "console") {}
    ~PersonImpl() = default;
    // Functionality
    int CreatePersonFromTable(
        int id, std::shared_ptr<datamanagement::DataManagerBase> dm) override;
    int Grow() override;
    int
    Die(data::DeathReason deathReason = data::DeathReason::BACKGROUND) override;

    // HCV
    int InfectHCV() override;
    int ClearHCV(bool acute = false) override;
    data::HCV GetHCV() const override;
    void SetHCV(data::HCV hcv) override;
    int AddAcuteHCVClearance() override;
    int GetAcuteHCVClearances() const override;
    bool GetSeropositivity() const override;
    void SetSeropositivity(bool seropositive) override;

    void Diagnose(data::InfectionType it = data::InfectionType::HCV) override;
    void
    ClearDiagnosis(data::InfectionType it = data::InfectionType::HCV) override;
    bool IsIdentifiedAsInfected(
        data::InfectionType it = data::InfectionType::HCV) const override;

    int GetTimeHCVIdentified() const override;
    int GetTimesHCVInfected() const override;
    int GetTimeHCVChanged() const override;
    int GetTimeSinceHCVChanged() const override;

    // Screening
    int
    MarkScreened(data::InfectionType it = data::InfectionType::HCV) override;
    int AddAbScreen(data::InfectionType it = data::InfectionType::HCV) override;
    int
    AddRnaScreen(data::InfectionType it = data::InfectionType::HCV) override;
    bool HadSecondScreeningTest() const override;
    int GetNumberOfABTests(
        data::InfectionType it = data::InfectionType::HCV) const override;
    int GetNumberOfRNATests(
        data::InfectionType it = data::InfectionType::HCV) const override;
    int GetTimeOfLastScreening(
        data::InfectionType it = data::InfectionType::HCV) const override;
    int GetTimeSinceLastScreening(
        data::InfectionType it = data::InfectionType::HCV) const override;
    data::ScreeningDetails GetScreeningDetails(
        data::InfectionType it = data::InfectionType::HCV) const override;
    bool CheckAntibodyPositive(
        data::InfectionType it = data::InfectionType::HCV) const override;
    void SetAntibodyPositive(
        bool result = true,
        data::InfectionType it = data::InfectionType::HCV) override;
    void GiveSecondScreeningTest(bool state) override;

    // Linking
    int Unlink(data::InfectionType it = data::InfectionType::HCV) override;
    int Link(data::LinkageType linkType,
             data::InfectionType it = data::InfectionType::HCV) override;
    data::LinkageState GetLinkState(
        data::InfectionType it = data::InfectionType::HCV) const override;
    int GetTimeOfLinkChange(
        data::InfectionType it = data::InfectionType::HCV) const override;
    int GetTimeSinceLinkChange(
        data::InfectionType it = data::InfectionType::HCV) const override;
    int GetLinkCount(
        data::InfectionType it = data::InfectionType::HCV) const override;
    void
    SetLinkageType(data::LinkageType linkType,
                   data::InfectionType it = data::InfectionType::HCV) override;
    data::LinkageType GetLinkageType(
        data::InfectionType it = data::InfectionType::HCV) const override;
    data::LinkageDetails GetLinkStatus(
        data::InfectionType it = data::InfectionType::HCV) const override;

    // Treatment
    int AddWithdrawal() override;
    int GetWithdrawals() const override;
    int AddToxicReaction() override;
    int GetToxicReactions() const override;
    int AddCompletedTreatment() override;
    int GetCompletedTreatments() const override;
    int GetRetreatments() const override;
    int AddSVR() override;
    int GetSVRs() const override;
    data::TreatmentDetails GetTreatmentDetails() const override;
    int GetNumberOfTreatmentStarts() const override;
    void InitiateTreatment() override;
    int EndTreatment() override;

    bool HasInitiatedTreatment() const override;
    bool IsInRetreatment() const override;

    int GetTimeOfTreatmentInitiation() const override;
    int GetTimeSinceTreatmentInitiation() const override;

    // Drug Use Behavior
    int SetBehavior(data::Behavior) override;
    data::Behavior GetBehavior() const override;
    data::BehaviorDetails GetBehaviorDetails() const override;

    int ToggleOverdose() override;
    bool GetCurrentlyOverdosing() const override;

    int GetNumberOfOverdoses() const override;
    int GetTimeBehaviorChange() const override;

    // Fibrosis
    data::MeasuredFibrosisState GetMeasuredFibrosisState() const override;
    int DiagnoseFibrosis(data::MeasuredFibrosisState) override;
    data::FibrosisState GetTrueFibrosisState() const override;
    void UpdateTrueFibrosis(data::FibrosisState) override;

    data::StagingDetails GetFibrosisStagingDetails() const override;

    int GetTimeTrueFibrosisStateChanged() const override;
    int GetTimeOfFibrosisStaging() const override;
    int GetTimeSinceFibrosisStaging() const override;

    // Cost Effectiveness
    int AddCost(double base_cost, double discount_cost,
                model::CostCategory category) override;

    // Life, Quality of Life
    data::LifetimeUtility GetTotalUtility() const override;
    void
    AccumulateTotalUtility(std::pair<double, double> util,
                           std::pair<double, double> discount_util) override;
    std::pair<double, double> GetUtility() const override;
    void SetUtility(double util, model::UtilityCategory category) override;
    std::unordered_map<model::UtilityCategory, double>
    GetCurrentUtilities() const override;
    int GetLifeSpan() const override;
    double GetDiscountedLifeSpan() const override;
    void AddDiscountedLifeSpan(double discounted_life) override;

    // General Data Handling
    int LoadICValues(int id, std::vector<std::string> icValues) override;
    bool IsAlive() const override;
    bool IsGenotypeThree() const override;
    void SetGenotypeThree(bool genotype) override;
    bool IsBoomer() const override;
    void SetBoomer(bool status) override;
    void SetDeathReason(data::DeathReason deathReason) override;
    data::DeathReason GetDeathReason() const override;
    int GetAge() const override;
    void SetAge(int age) override;

    bool IsCirrhotic() override;

    std::string GetPersonDataString() const override;
    int GetID() const override;
    int GetCurrentTimestep() const override;
    data::Sex GetSex() const override;
    std::unordered_map<model::CostCategory, std::pair<double, double>>
    GetCosts() const override;
    std::pair<double, double> GetCostTotals() const override;
    data::HCVDetails GetHCVDetails() const override;
    data::HIVDetails GetHIVDetails() const override;
    data::HCCDetails GetHCCDetails() const override;

    /// HIV
    // getter/setter
    data::HIV GetHIV() const override;
    void SetHIV(data::HIV hiv) override;
    // infection
    void InfectHIV() override;
    int GetTimeHIVChanged() const override;
    int GetTimeSinceHIVChanged() const override;

    // TODO
    data::PregnancyState GetPregnancyState() const override;
    int GetTimeOfPregnancyChange() const override;
    int GetTimeSincePregnancyChange() const override;
    int GetNumMiscarriages() const override;
    int Miscarry() override;
    int Stillbirth() override;
    int AddChild(data::HCV hcv, bool test) override;
    int EndPostpartum() override;
    int Impregnate() override;
    void ExposeInfant() override;
    data::PregnancyDetails GetPregnancyDetails() const override;
    void SetPregnancyState(data::PregnancyState state) override;
    void SetNumMiscarriages(int miscarriages) override;

    void TransitionMOUD() override;
    data::MOUD GetMoudState() const override;
    int GetTimeStartedMoud() const override;
    data::MOUDDetails GetMOUDDetails() const override;
    std::vector<data::Child> GetChildren() const override;

    // HCC
    void DevelopHCC(data::HCCState state) override;
    data::HCCState GetHCCState() const override;
    void DiagnoseHCC() override;
    bool IsDiagnosedWithHCC() const override;
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_PERSONINTERNALS_HPP_