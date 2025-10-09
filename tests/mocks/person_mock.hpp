////////////////////////////////////////////////////////////////////////////////
// File: person_mock.hpp                                                      //
// Project: hep-ce                                                            //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-10-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Your Company                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef HEPCE_TESTS_PERSONMOCK_HPP_
#define HEPCE_TESTS_PERSONMOCK_HPP_

#include <hepce/model/person.hpp>

#include <string>
#include <vector>

#include <gmock/gmock.h>

using namespace hepce::model;

namespace hepce {
namespace testing {
class MockPerson : public virtual Person {
public:
    // Functionality
    MOCK_METHOD(void, Grow, (), (override));
    MOCK_METHOD(void, Die, (data::DeathReason deathReason), (override));
    MOCK_METHOD(void, SetPersonDetails, (const data::PersonSelect &select),
                (override));

    // HCV
    MOCK_METHOD(data::HCVDetails, GetHCVDetails, (), (const, override));
    MOCK_METHOD(void, InfectHCV, (), (override));
    MOCK_METHOD(void, ClearHCV, (bool is_acute), (override));
    MOCK_METHOD(void, SetHCV, (data::HCV hcv), (override));
    MOCK_METHOD(void, Diagnose, (data::InfectionType it), (override));
    MOCK_METHOD(void, ClearDiagnosis, (data::InfectionType it), (override));
    MOCK_METHOD(void, FalsePositive, (data::InfectionType it), (override));
    MOCK_METHOD(bool, IsCirrhotic, (), (const, override));

    // Screening
    MOCK_METHOD(data::ScreeningDetails, GetScreeningDetails,
                (data::InfectionType it), (const, override));
    MOCK_METHOD(void, Screen,
                (data::InfectionType it, data::ScreeningTest test,
                 data::ScreeningType type),
                (override));
    MOCK_METHOD(void, GiveSecondStagingTest, (), (override));
    MOCK_METHOD(void, AddFalseNegative, (data::InfectionType it), (override));
    MOCK_METHOD(void, AddIdentificationsCleared, (data::InfectionType it),
                (override));

    // Linking
    MOCK_METHOD(data::LinkageDetails, GetLinkageDetails,
                (data::InfectionType it), (const, override));
    MOCK_METHOD(void, Unlink, (data::InfectionType it), (override));
    MOCK_METHOD(void, Link, (data::InfectionType it), (override));

    // Treatment
    MOCK_METHOD(data::TreatmentDetails, GetTreatmentDetails,
                (data::InfectionType it), (const, override));
    MOCK_METHOD(void, AddWithdrawal, (data::InfectionType it), (override));
    MOCK_METHOD(void, AddToxicReaction, (data::InfectionType it), (override));
    MOCK_METHOD(void, AddCompletedTreatment, (data::InfectionType it),
                (override));
    MOCK_METHOD(void, AddSVR, (), (override));
    MOCK_METHOD(void, InitiateTreatment, (data::InfectionType it), (override));
    MOCK_METHOD(void, EndTreatment, (data::InfectionType it), (override));

    // Drug Use Behavior
    MOCK_METHOD(void, SetBehavior, (data::Behavior), (override));
    MOCK_METHOD(data::BehaviorDetails, GetBehaviorDetails, (),
                (const, override));

    // Overdoses
    MOCK_METHOD(void, ToggleOverdose, (), (override));
    MOCK_METHOD(bool, GetCurrentlyOverdosing, (), (const, override));
    MOCK_METHOD(int, GetNumberOfOverdoses, (), (const, override));

    // MOUD Details
    MOCK_METHOD(data::MOUDDetails, GetMoudDetails, (), (const, override));
    MOCK_METHOD(void, SetMoudState, (data::MOUD moud), (override));
    MOCK_METHOD(void, TransitionMOUD, (), (override));

    // Fibrosis
    MOCK_METHOD(void, DiagnoseFibrosis, (data::MeasuredFibrosisState),
                (override));
    MOCK_METHOD(void, SetFibrosis, (data::FibrosisState), (override));
    MOCK_METHOD(data::StagingDetails, GetFibrosisStagingDetails, (),
                (const, override));

    // Cost Effectiveness
    MOCK_METHOD(void, AddCost,
                (double base_cost, double discount_cost,
                 model::CostCategory category),
                (override));

    // Life, Quality of Life
    MOCK_METHOD(data::LifetimeUtility, GetTotalUtility, (), (const, override));
    MOCK_METHOD(void, AccumulateTotalUtility, (double discount), (override));
    MOCK_METHOD((std::unordered_map<model::UtilityCategory, double>),
                GetUtilities, (), (const, override));
    MOCK_METHOD(void, SetUtility,
                (double util, model::UtilityCategory category), (override));
    MOCK_METHOD(int, GetLifeSpan, (), (const, override));
    MOCK_METHOD(double, GetDiscountedLifeSpan, (), (const, override));
    MOCK_METHOD(void, AddDiscountedLifeSpan, (double discounted_life),
                (override));

    // General Data Handling
    MOCK_METHOD(bool, IsAlive, (), (const, override));
    MOCK_METHOD(void, SetGenotypeThree, (bool genotype), (override));
    MOCK_METHOD(bool, IsBoomer, (), (const, override));
    MOCK_METHOD(void, SetDeathReason, (data::DeathReason deathReason),
                (override));
    MOCK_METHOD(data::DeathReason, GetDeathReason, (), (const, override));
    MOCK_METHOD(int, GetAge, (), (const, override));
    MOCK_METHOD(int, GetCurrentTimestep, (), (const, override));
    MOCK_METHOD(data::Sex, GetSex, (), (const, override));
    MOCK_METHOD(
        (std::unordered_map<model::CostCategory, std::pair<double, double>>),
        GetCosts, (), (const, override));
    MOCK_METHOD((std::pair<double, double>), GetCostTotals, (),
                (const, override));
    MOCK_METHOD(data::HIVDetails, GetHIVDetails, (), (const, override));
    MOCK_METHOD(data::HCCDetails, GetHCCDetails, (), (const, override));

    /// HIV
    MOCK_METHOD(void, SetHIV, (data::HIV hiv), (override));
    MOCK_METHOD(void, InfectHIV, (), (override));

    // Pregnancy
    MOCK_METHOD(data::PregnancyDetails, GetPregnancyDetails, (),
                (const, override));
    MOCK_METHOD(void, Stillbirth, (), (override));
    MOCK_METHOD(void, Birth, (const data::Child &child), (override));
    MOCK_METHOD(void, EndPostpartum, (), (override));
    MOCK_METHOD(void, Impregnate, (), (override));
    MOCK_METHOD(void, AddInfantExposure, (), (override));
    MOCK_METHOD(void, SetPregnancyState, (data::PregnancyState state),
                (override));

    // HCC
    MOCK_METHOD(void, DevelopHCC, (data::HCCState state), (override));
    MOCK_METHOD(void, DiagnoseHCC, (), (override));

    // Person Output
    MOCK_METHOD(std::string, MakePopulationRow, (), (const, override));
};
} // namespace testing
} // namespace hepce

#endif
