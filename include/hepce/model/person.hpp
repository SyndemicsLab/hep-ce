////////////////////////////////////////////////////////////////////////////////
// File: person.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-09                                                  //
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
#include <hepce/model/costing.hpp>
#include <hepce/model/utility.hpp>

namespace hepce {
namespace model {
class Person {
public:
    // Functionality
    virtual void Grow() = 0;
    virtual void
    Die(data::DeathReason deathReason = data::DeathReason::kBackground) = 0;

    virtual void SetPersonDetails(const data::PersonSelect &select) = 0;

    // HCV
    virtual data::HCVDetails GetHCVDetails() const = 0;
    virtual void InfectHCV() = 0;
    virtual void ClearHCV(bool is_acute = false) = 0;
    virtual void SetHCV(data::HCV hcv) = 0;
    virtual void Diagnose(data::InfectionType it) = 0;
    virtual void ClearDiagnosis(data::InfectionType it) = 0;
    virtual bool IsCirrhotic() const = 0;
    virtual void SetFibrosis(data::FibrosisState) = 0;
    virtual void AddSVR() = 0;

    // Screening
    virtual data::ScreeningDetails
    GetScreeningDetails(data::InfectionType it) const = 0;
    virtual void Screen(data::InfectionType it, data::ScreeningTest test,
                        data::ScreeningType type) = 0;

    // Linking
    virtual data::LinkageDetails
    GetLinkageDetails(data::InfectionType it) const = 0;
    virtual void Link(data::InfectionType it) = 0;
    virtual void Unlink(data::InfectionType it) = 0;

    // Treatment
    virtual data::TreatmentDetails
    GetTreatmentDetails(data::InfectionType it) const = 0;
    virtual void AddWithdrawal(data::InfectionType it) = 0;
    virtual void AddToxicReaction(data::InfectionType it) = 0;
    virtual void AddCompletedTreatment(data::InfectionType it) = 0;
    virtual void InitiateTreatment(data::InfectionType it) = 0;
    virtual void EndTreatment(data::InfectionType it) = 0;

    // Drug Use Behavior
    virtual void SetBehavior(data::Behavior) = 0;
    virtual data::BehaviorDetails GetBehaviorDetails() const = 0;

    // Overdoses
    virtual void ToggleOverdose() = 0;
    virtual bool GetCurrentlyOverdosing() const = 0;
    virtual int GetNumberOfOverdoses() const = 0;

    // MOUD Details
    virtual data::MOUDDetails GetMoudDetails() const = 0;
    virtual void SetMoudState(data::MOUD moud) = 0;
    virtual void TransitionMOUD() = 0;

    // Fibrosis
    virtual void DiagnoseFibrosis(data::MeasuredFibrosisState) = 0;
    virtual data::StagingDetails GetFibrosisStagingDetails() const = 0;
    virtual void GiveSecondStagingTest() = 0;

    // Cost Effectiveness
    virtual void AddCost(double base_cost, double discount_cost,
                         model::CostCategory category) = 0;
    virtual std::unordered_map<model::CostCategory, std::pair<double, double>>
    GetCosts() const = 0;
    virtual std::pair<double, double> GetCostTotals() const = 0;

    // Life, Quality of Life
    virtual data::LifetimeUtility GetTotalUtility() const = 0;
    virtual void AccumulateTotalUtility(double discount) = 0;
    virtual std::unordered_map<model::UtilityCategory, double>
    GetUtilities() const = 0;
    virtual void SetUtility(double util, model::UtilityCategory category) = 0;
    virtual int GetLifeSpan() const = 0;
    virtual double GetDiscountedLifeSpan() const = 0;
    virtual void AddDiscountedLifeSpan(double discounted_life) = 0;

    // General Data Handling
    virtual bool IsAlive() const = 0;
    virtual void SetGenotypeThree(bool genotype) = 0;
    virtual bool IsBoomer() const = 0;
    virtual void SetDeathReason(data::DeathReason deathReason) = 0;
    virtual data::DeathReason GetDeathReason() const = 0;
    virtual int GetAge() const = 0;
    virtual int GetCurrentTimestep() const = 0;
    virtual data::Sex GetSex() const = 0;

    /// HIV
    virtual data::HIVDetails GetHIVDetails() const = 0;
    virtual void SetHIV(data::HIV hiv) = 0;
    virtual void InfectHIV() = 0;

    // Pregnancy
    virtual data::PregnancyDetails GetPregnancyDetails() const = 0;
    virtual void Miscarry() = 0;
    virtual void Stillbirth() = 0;
    virtual void Birth(const data::Child &child) = 0;
    virtual void EndPostpartum() = 0;
    virtual void Impregnate() = 0;
    virtual void AddInfantExposure() = 0;
    virtual void SetPregnancyState(data::PregnancyState state) = 0;

    // HCC
    virtual data::HCCDetails GetHCCDetails() const = 0;
    virtual void DevelopHCC(data::HCCState state) = 0;
    virtual void DiagnoseHCC() = 0;

    // Person Output
    virtual std::string MakePopulationRow() const = 0;

    static std::unique_ptr<Person>
    Create(const std::string &log_name = "console");
};
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_PERSON_HPP_