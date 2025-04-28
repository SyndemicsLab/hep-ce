////////////////////////////////////////////////////////////////////////////////
// File: person_internals.hpp                                                 //
// Project: HEPCESimulationv2                                                 //
// Created Date: Fr Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_PERSONINTERNALS_HPP_
#define HEPCE_MODEL_PERSONINTERNALS_HPP_

#include <hepce/model/person.hpp>

#include <hepce/data/types.hpp>
#include <hepce/model/cost.hpp>

namespace hepce {
namespace model {
class PersonImpl : public Person {
public:
    PersonImpl(const std::string &log_name = "console");
    ~PersonImpl() = default;

    // Functionality
    inline int Grow() override {
        UpdateTimers();
        SetAge(GetAge() + 1);
        _life_span++;
    }
    int Die(data::DeathReason deathReason =
                data::DeathReason::kBackground) override;

    // HCV
    int InfectHCV() override;
    int ClearHCV(bool is_acute = false) override;
    data::HCV GetHCV() const override;
    void SetHCV(data::HCV hcv) override;
    int AddAcuteHCVClearance() override;
    int GetAcuteHCVClearances() const override;
    bool GetSeropositivity() const override;
    void SetSeropositivity(bool seropositive_state) override;

    void Diagnose(data::InfectionType it) override;
    void ClearDiagnosis(data::InfectionType it) override;
    bool IsIdentifiedAsInfected(data::InfectionType it) const override;

    int GetTimeHCVIdentified() const override;
    int GetTimesHCVInfected() const override;
    int GetTimeHCVChanged() const override;
    int GetTimeSinceHCVChanged() const override;

    // Screening
    int MarkScreened(data::InfectionType it) override;
    int AddAbScreen(data::InfectionType it) override;
    int AddRnaScreen(data::InfectionType it) override;
    bool HadSecondScreeningTest() const override;
    int GetNumberOfABTests(data::InfectionType it) const override;
    int GetNumberOfRNATests(data::InfectionType it) const override;
    int GetTimeOfLastScreening(data::InfectionType it) const override;
    int GetTimeSinceLastScreening(data::InfectionType it) const override;
    data::ScreeningDetails
    GetScreeningDetails(data::InfectionType it) const override;
    bool CheckAntibodyPositive(data::InfectionType it) const override;
    void SetAntibodyPositive(bool result = true,
                             data::InfectionType it) override;
    void GiveSecondScreeningTest(bool state) override;

    // Linking
    int Unlink(data::InfectionType it) override;
    int Link(data::LinkageType link_type, data::InfectionType it) override;
    data::LinkageState GetLinkState(data::InfectionType it) const override;
    int GetTimeOfLinkChange(data::InfectionType it) const override;
    int GetTimeSinceLinkChange(data::InfectionType it) const override;
    int GetLinkCount(data::InfectionType it) const override;
    void SetLinkageType(data::LinkageType link_type,
                        data::InfectionType it) override;
    data::LinkageType GetLinkageType(data::InfectionType it) const override;
    data::LinkageDetails GetLinkStatus(data::InfectionType it) const override;

    // Treatment
    int AddWithdrawal(data::InfectionType it) override;
    int GetWithdrawals(data::InfectionType it) const override;
    int AddToxicReaction(data::InfectionType it) override;
    int GetToxicReactions(data::InfectionType it) const override;
    int AddCompletedTreatment(data::InfectionType it) override;
    int GetCompletedTreatments(data::InfectionType it) const override;
    int GetRetreatments(data::InfectionType it) const override;
    int AddSVR() override;
    int GetSVRs() const override;
    data::TreatmentDetails
    GetTreatmentDetails(data::InfectionType it) const override;
    int GetNumberOfTreatmentStarts(data::InfectionType it) const override;
    void InitiateTreatment(data::InfectionType it) override;
    int EndTreatment(data::InfectionType it) override;
    bool HasInitiatedTreatment(data::InfectionType it) const override;
    bool IsInRetreatment(data::InfectionType it) const override;
    int GetTimeOfTreatmentInitiation(data::InfectionType it) const override;
    int GetTimeSinceTreatmentInitiation(data::InfectionType it) const override;

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
    // outcomes
    int GetLowCD4MonthCount() const override;

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
    void AddInfantExposure() override;
    data::PregnancyDetails GetPregnancyDetails() const override;
    void SetPregnancyState(data::PregnancyState state) override;
    void SetNumMiscarriages(int miscarriages) override;
    int GetInfantHCVExposures() const override;
    int GetInfantHCVInfections() const override;
    int GetInfantHCVTests() const override;
    int GetPregnancyCount() const override;

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

    // Person Output
    std::string MakePopulationRow() const override;

private:
    size_t _id;
    size_t _current_time = 0;

    data::Sex _sex = data::Sex::kMale;
    int _age = 0;
    bool _is_alive = true;
    bool _boomer_classification = false;
    data::DeathReason _death_reason = data::DeathReason::kNa;
    data::BehaviorDetails _behavior_details;
    data::HCVDetails _hcv_details;
    data::HIVDetails _hiv_details;
    data::HCCDetails _hcc_details;
    bool _currently_overdosing = false;
    int _num_overdoses = 0;
    data::MOUDDetails _moud_details;
    data::PregnancyDetails _pregnancy_details;
    data::StagingDetails _staging_details;
    std::unordered_map<data::InfectionType, data::LinkageDetails>
        _linkage_details;
    std::unordered_map<data::InfectionType, data::ScreeningDetails>
        _screening_details;
    std::unordered_map<data::InfectionType, data::TreatmentDetails>
        _treatment_details;
    // utility
    std::unordered_map<model::UtilityCategory, double> _utilities;
    // life span tracking
    int _life_span = 0;
    double _discounted_life_span = 0;
    // cost
    std::unique_ptr<model::Costs> _costs;

    int UpdateTimers() {
        _current_time++;
        if (GetBehavior() == data::Behavior::kNoninjection ||
            GetBehavior() == data::Behavior::kInjection) {
            _behavior_details.time_last_active = _current_time;
        }
        if (GetMoudState() == data::MOUD::kCurrent) {
            _moud_details.total_moud_months++;
        }
        if (GetHIV() == data::HIV::LoUn || GetHIV() == data::HIV::LoSu) {
            _hiv_details.low_cd4_months_count++;
        }
        _moud_details.current_state_concurrent_months++;
        return 0;
    }
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_PERSONINTERNALS_HPP_