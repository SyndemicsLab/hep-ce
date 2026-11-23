////////////////////////////////////////////////////////////////////////////////
// File: person.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_MODEL_PERSON_HPP_
#define HEPCE_MODEL_PERSON_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include <hepce/data/types.hpp>
#include <hepce/model/costing.hpp>
#include <hepce/model/utility.hpp>

namespace hepce {
namespace model {
class Person {
public:
    /// Destroys the person.
    virtual ~Person() = default;

    /// Creates an independent copy of the person.
    Person(const Person &) = delete;
    /// Persons cannot be copy-assigned.
    Person &operator=(const Person &) = delete;
    /// Creates an independent copy of the person.
    virtual std::unique_ptr<Person> clone() const = 0;

    /// Creates a person using the supplied logger name.
    /// @param log_name Logger name used by the person.
    static std::unique_ptr<Person>
    Create(const std::string &log_name = "console");

    /// Advances the person's age and time-dependent state by one timestep.
    virtual void Grow() = 0;
    /// Marks the person as dead for the supplied reason.
    /// @param deathReason Reason for death.
    virtual void
    Die(data::DeathReason deathReason = data::DeathReason::kBackground) = 0;

    /// Sets the person's state from a selected input record.
    /// @param select Input values used to initialize the person.
    virtual void SetPersonDetails(const data::PersonSelect &select) = 0;
    /// Sets the person's starting timestep.
    /// @param start_time Starting timestep.
    virtual void SetStartTime(const int start_time = 0) = 0;

    /// Returns the person's HCV state.
    virtual data::HCVDetails GetHCVDetails() const = 0;
    /// Infects the person with HCV.
    virtual void InfectHCV() = 0;
    /// Clears HCV infection.
    /// @param is_acute Whether the cleared infection was acute.
    virtual void ClearHCV(bool is_acute = false) = 0;
    /// Sets the person's HCV state.
    virtual void SetHCV(data::HCV hcv) = 0;
    /// Records a diagnosis for the specified infection type.
    virtual void Diagnose(data::InfectionType it) = 0;
    /// Clears a diagnosis for the specified infection type.
    virtual void ClearDiagnosis(data::InfectionType it) = 0;
    /// Records a false-positive result for the specified infection type.
    virtual void FalsePositive(data::InfectionType it) = 0;
    /// Returns whether the person's fibrosis state is cirrhotic.
    virtual bool IsCirrhotic() const = 0;
    /// Sets the person's fibrosis state.
    virtual void SetFibrosis(data::FibrosisState) = 0;
    /// Records a sustained virologic response.
    virtual void AddSVR() = 0;
    /// Records a false-negative result for the specified infection type.
    virtual void AddFalseNegative(data::InfectionType it) = 0;
    /// Records a cleared identification for the specified infection type.
    virtual void AddIdentificationsCleared(data::InfectionType it) = 0;

    /// Returns screening details for the specified infection type.
    virtual data::ScreeningDetails
    GetScreeningDetails(data::InfectionType it) const = 0;
    /// Records a screening test for the specified infection type.
    virtual void Screen(data::InfectionType it, data::ScreeningTest test,
                        data::ScreeningType type) = 0;

    /// Returns linkage details for the specified infection type.
    virtual data::LinkageDetails
    GetLinkageDetails(data::InfectionType it) const = 0;
    /// Links the person to care for the specified infection type.
    virtual void Link(data::InfectionType it) = 0;
    /// Removes the person's linkage to care for the specified infection type.
    virtual void Unlink(data::InfectionType it) = 0;

    /// Returns treatment details for the specified infection type.
    virtual data::TreatmentDetails
    GetTreatmentDetails(data::InfectionType it) const = 0;
    /// Records a treatment withdrawal.
    virtual void AddWithdrawal(data::InfectionType it) = 0;
    /// Records a toxic treatment reaction.
    virtual void AddToxicReaction(data::InfectionType it) = 0;
    /// Records a completed treatment course.
    virtual void AddCompletedTreatment(data::InfectionType it) = 0;
    /// Starts treatment for the specified infection type.
    virtual void InitiateTreatment(data::InfectionType it) = 0;
    /// Ends treatment for the specified infection type.
    virtual void EndTreatment(data::InfectionType it) = 0;

    /// Sets the person's drug-use behavior.
    virtual void SetBehavior(data::Behavior) = 0;
    /// Returns the person's drug-use behavior details.
    virtual data::BehaviorDetails GetBehaviorDetails() const = 0;

    /// Toggles the person's current overdose state.
    virtual void ToggleOverdose() = 0;
    /// Returns whether the person is currently overdosing.
    virtual bool GetCurrentlyOverdosing() const = 0;
    /// Returns the number of overdoses recorded for the person.
    virtual int GetNumberOfOverdoses() const = 0;

    /// Returns the person's MOUD details.
    virtual data::MOUDDetails GetMoudDetails() const = 0;
    /// Sets the person's MOUD state.
    virtual void SetMoudState(data::MOUD moud) = 0;
    /// Advances the person's MOUD state.
    virtual void TransitionMOUD() = 0;

    /// Records a fibrosis staging diagnosis.
    virtual void DiagnoseFibrosis(data::MeasuredFibrosisState) = 0;
    /// Returns the person's fibrosis staging details.
    virtual data::StagingDetails GetFibrosisStagingDetails() const = 0;
    /// Records administration of a second staging test.
    virtual void GiveSecondStagingTest() = 0;

    /// Adds undiscounted and discounted cost for a category.
    /// @param base_cost Undiscounted cost.
    /// @param discount_cost Discounted cost.
    /// @param category Cost category.
    virtual void AddCost(double base_cost, double discount_cost,
                         model::CostCategory category) = 0;
    /// Returns costs grouped by category as base and discounted values.
    virtual std::unordered_map<model::CostCategory, std::pair<double, double>>
    GetCosts() const = 0;
    /// Returns total base and discounted costs.
    virtual std::pair<double, double> GetCostTotals() const = 0;

    /// Returns the person's accumulated utility.
    virtual data::LifetimeUtility GetTotalUtility() const = 0;
    /// Accumulates utility using the supplied discount factor.
    virtual void AccumulateTotalUtility(double discount) = 0;
    /// Returns utility values grouped by category.
    virtual std::unordered_map<model::UtilityCategory, double>
    GetUtilities() const = 0;
    /// Sets utility for a category.
    virtual void SetUtility(double util, model::UtilityCategory category) = 0;
    /// Returns the person's undiscounted lifespan.
    virtual int GetLifeSpan() const = 0;
    /// Returns the person's discounted lifespan.
    virtual double GetDiscountedLifeSpan() const = 0;
    /// Adds a discounted lifespan value.
    virtual void AddDiscountedLifeSpan(double discounted_life) = 0;

    /// Returns whether the person is alive.
    virtual bool IsAlive() const = 0;
    /// Sets whether the person has genotype-three HCV.
    virtual void SetGenotypeThree(bool genotype) = 0;
    /// Returns whether the person is in the boomer classification.
    virtual bool IsBoomer() const = 0;
    /// Sets the person's death reason.
    virtual void SetDeathReason(data::DeathReason deathReason) = 0;
    /// Returns the person's death reason.
    virtual data::DeathReason GetDeathReason() const = 0;
    /// Returns the person's age.
    virtual int GetAge() const = 0;
    /// Returns the person's current timestep.
    virtual int GetCurrentTimestep() const = 0;
    /// Returns the person's sex.
    virtual data::Sex GetSex() const = 0;

    /// Returns the person's HIV details.
    virtual data::HIVDetails GetHIVDetails() const = 0;
    /// Sets the person's HIV state.
    virtual void SetHIV(data::HIV hiv) = 0;
    /// Infects the person with HIV.
    virtual void InfectHIV() = 0;

    /// Returns the person's pregnancy details.
    virtual data::PregnancyDetails GetPregnancyDetails() const = 0;
    /// Records a stillbirth.
    virtual void Stillbirth() = 0;
    /// Records a birth and its child details.
    virtual void Birth(const data::Child &child) = 0;
    /// Ends the person's postpartum state.
    virtual void EndPostpartum() = 0;
    /// Makes the person pregnant.
    virtual void Impregnate() = 0;
    /// Records an infant HCV exposure.
    virtual void AddInfantExposure() = 0;
    /// Sets the person's pregnancy state.
    virtual void SetPregnancyState(data::PregnancyState state) = 0;

    /// Returns the person's HCC details.
    virtual data::HCCDetails GetHCCDetails() const = 0;
    /// Sets the person's HCC state.
    virtual void DevelopHCC(data::HCCState state) = 0;
    /// Records an HCC diagnosis.
    virtual void DiagnoseHCC() = 0;

    /// Serializes the person's current state as a population row.
    virtual std::string MakePopulationRow() const = 0;

protected:
    // default constructor. Do not want public access, but need for subclasses.
    Person() = default;
};

using People = std::vector<std::unique_ptr<Person>>;
} // namespace model
} // namespace hepce

#endif // HEPCE_MODEL_PERSON_HPP_
