////////////////////////////////////////////////////////////////////////////////
// File: PersonMock.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-21                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef PERSONMOCK_HPP_
#define PERSONMOCK_HPP_

#include "Person.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>

namespace person {
class MOCKPerson : public PersonBase {
public:
    // Functionality
    MOCK_METHOD(int, CreatePersonFromTable,
                (int id, std::shared_ptr<datamanagement::DataManagerBase> dm),
                (override));
    MOCK_METHOD(int, Grow, (), (override));
    MOCK_METHOD(int, Die, (DeathReason deathReason), (override));

    // HCV
    MOCK_METHOD(int, InfectHCV, (), (override));
    MOCK_METHOD(int, ClearHCV, (bool acute), (override));
    MOCK_METHOD(HCV, GetHCV, (), (const, override));
    MOCK_METHOD(void, SetHCV, (HCV hcv), (override));
    MOCK_METHOD(int, AddAcuteHCVClearance, (), (override));
    MOCK_METHOD(int, GetAcuteHCVClearances, (), (const, override));
    MOCK_METHOD(bool, GetSeropositivity, (), (const, override));
    MOCK_METHOD(void, SetSeropositivity, (bool seropositive), (override));
    MOCK_METHOD(void, Diagnose, (InfectionType it), (override));
    MOCK_METHOD(void, ClearDiagnosis, (InfectionType it), (override));
    MOCK_METHOD(bool, IsIdentifiedAsInfected, (InfectionType it),
                (const, override));

    MOCK_METHOD(int, GetTimeHCVIdentified, (), (const, override));
    MOCK_METHOD(int, GetTimesHCVInfected, (), (const, override));
    MOCK_METHOD(int, GetTimeHCVChanged, (), (const, override));
    MOCK_METHOD(int, GetTimeSinceHCVChanged, (), (const, override));

    // Screening
    MOCK_METHOD(int, MarkScreened, (InfectionType it), (override));
    MOCK_METHOD(int, AddAbScreen, (InfectionType it), (override));
    MOCK_METHOD(int, AddRnaScreen, (InfectionType it), (override));
    MOCK_METHOD(int, GetNumberOfABTests, (InfectionType it), (const, override));
    MOCK_METHOD(int, GetNumberOfRNATests, (InfectionType it),
                (const, override));
    MOCK_METHOD(int, GetTimeOfLastScreening, (InfectionType it),
                (const, override));
    MOCK_METHOD(int, GetTimeSinceLastScreening, (InfectionType it),
                (const, override));
    MOCK_METHOD(ScreeningDetails, GetScreeningDetails, (InfectionType it),
                (const, override));
    MOCK_METHOD(bool, HadSecondScreeningTest, (), (const, override));
    MOCK_METHOD(void, GiveSecondScreeningTest, (bool state), (override));
    MOCK_METHOD(bool, CheckAntibodyPositive, (InfectionType it),
                (const, override));
    MOCK_METHOD(void, SetAntibodyPositive, (bool result, InfectionType it),
                (override));

    // Linking
    MOCK_METHOD(int, Unlink, (InfectionType it), (override));
    MOCK_METHOD(int, Link, (LinkageType linkType, InfectionType it),
                (override));
    MOCK_METHOD(LinkageState, GetLinkState, (InfectionType it),
                (const, override));
    MOCK_METHOD(int, GetTimeOfLinkChange, (InfectionType it),
                (const, override));
    MOCK_METHOD(int, GetTimeSinceLinkChange, (InfectionType it),
                (const, override));
    MOCK_METHOD(int, GetLinkCount, (InfectionType it), (const, override));
    MOCK_METHOD(void, SetLinkageType, (LinkageType linkType, InfectionType it),
                (override));
    MOCK_METHOD(LinkageType, GetLinkageType, (InfectionType it),
                (const, override));
    MOCK_METHOD(LinkageDetails, GetLinkStatus, (InfectionType it),
                (const, override));

    // Treatment
    MOCK_METHOD(int, AddWithdrawal, (), (override));
    MOCK_METHOD(int, GetWithdrawals, (), (const, override));
    MOCK_METHOD(int, AddToxicReaction, (), (override));
    MOCK_METHOD(int, GetToxicReactions, (), (const, override));
    MOCK_METHOD(int, AddCompletedTreatment, (), (override));
    MOCK_METHOD(int, GetCompletedTreatments, (), (const, override));
    MOCK_METHOD(int, GetRetreatments, (), (const, override));
    MOCK_METHOD(int, AddSVR, (), (override));
    MOCK_METHOD(int, GetSVRs, (), (const, override));
    MOCK_METHOD(TreatmentDetails, GetTreatmentDetails, (InfectionType it),
                (const, override));
    MOCK_METHOD(int, GetNumberOfTreatmentStarts, (InfectionType it),
                (const, override));
    MOCK_METHOD(void, InitiateTreatment, (InfectionType it), (override));
    MOCK_METHOD(int, EndTreatment, (InfectionType it), (override));
    MOCK_METHOD(bool, HasInitiatedTreatment, (InfectionType it),
                (const, override));
    MOCK_METHOD(bool, IsInRetreatment, (InfectionType it), (const, override));
    MOCK_METHOD(int, GetTimeOfTreatmentInitiation, (InfectionType it),
                (const, override));
    MOCK_METHOD(int, GetTimeSinceTreatmentInitiation, (InfectionType it),
                (const, override));

    // Drug Use Behavior
    MOCK_METHOD(int, SetBehavior, (Behavior), (override));
    MOCK_METHOD(Behavior, GetBehavior, (), (const, override));
    MOCK_METHOD(BehaviorDetails, GetBehaviorDetails, (), (const, override));

    MOCK_METHOD(int, ToggleOverdose, (), (override));
    MOCK_METHOD(bool, GetCurrentlyOverdosing, (), (const, override));

    MOCK_METHOD(int, GetNumberOfOverdoses, (), (const, override));
    MOCK_METHOD(int, GetTimeBehaviorChange, (), (const, override));

    // Fibrosis
    MOCK_METHOD(MeasuredFibrosisState, GetMeasuredFibrosisState, (),
                (const, override));
    MOCK_METHOD(int, DiagnoseFibrosis, (MeasuredFibrosisState), (override));
    MOCK_METHOD(FibrosisState, GetTrueFibrosisState, (), (const, override));
    MOCK_METHOD(void, UpdateTrueFibrosis, (FibrosisState state), (override));

    MOCK_METHOD(StagingDetails, GetFibrosisStagingDetails, (),
                (const, override));

    MOCK_METHOD(int, GetTimeTrueFibrosisStateChanged, (), (const, override));
    MOCK_METHOD(int, GetTimeOfFibrosisStaging, (), (const, override));
    MOCK_METHOD(int, GetTimeSinceFibrosisStaging, (), (const, override));

    // Cost Effectiveness
    MOCK_METHOD(int, AddCost, (double, double, cost::CostCategory), (override));

    // General Data Handling
    MOCK_METHOD(int, LoadICValues, (int id, std::vector<std::string> icValues),
                (override));
    MOCK_METHOD(bool, IsAlive, (), (const, override));
    MOCK_METHOD(bool, IsGenotypeThree, (), (const, override));
    MOCK_METHOD(void, SetGenotypeThree, (bool genotype), (override));
    MOCK_METHOD(bool, IsBoomer, (), (const, override));
    MOCK_METHOD(void, SetBoomer, (bool status), (override));
    MOCK_METHOD(void, SetDeathReason, (DeathReason deathReason), (override));
    MOCK_METHOD(DeathReason, GetDeathReason, (), (const, override));
    MOCK_METHOD(int, GetAge, (), (const, override));
    MOCK_METHOD(void, SetAge, (int age), (override));

    MOCK_METHOD(bool, IsCirrhotic, (), (override));

    MOCK_METHOD(std::string, GetPersonDataString, (), (const, override));
    MOCK_METHOD(int, GetID, (), (const, override));
    MOCK_METHOD(int, GetCurrentTimestep, (), (const, override));
    MOCK_METHOD(Sex, GetSex, (), (const, override));
    MOCK_METHOD(
        (std::unordered_map<cost::CostCategory, std::pair<double, double>>),
        GetCosts, (), (const, override));
    MOCK_METHOD((std::pair<double, double>), GetCostTotals, (),
                (const, override));
    MOCK_METHOD(HCVDetails, GetHCVDetails, (), (const, override));
    MOCK_METHOD(HIVDetails, GetHIVDetails, (), (const, override));
    MOCK_METHOD(HCCDetails, GetHCCDetails, (), (const, override));

    MOCK_METHOD(LifetimeUtility, GetTotalUtility, (), (const, override));
    MOCK_METHOD(void, AccumulateTotalUtility,
                ((std::pair<double, double> util),
                 (std::pair<double, double> discount_util)),
                (override));
    MOCK_METHOD((std::pair<double, double>), GetUtility, (), (const, override));
    MOCK_METHOD(void, SetUtility, (double util, utility::UtilityCategory),
                (override));
    MOCK_METHOD((std::unordered_map<utility::UtilityCategory, double>),
                GetCurrentUtilities, (), (const, override));
    MOCK_METHOD(int, GetLifeSpan, (), (const, override));
    MOCK_METHOD(double, GetDiscountedLifeSpan, (), (const, override));
    MOCK_METHOD(void, AddDiscountedLifeSpan, (double discounted_life),
                (override));

    // HIV
    MOCK_METHOD(HIV, GetHIV, (), (const, override));
    MOCK_METHOD(void, SetHIV, (HIV hiv), (override));
    MOCK_METHOD(void, InfectHIV, (), (override));
    MOCK_METHOD(int, GetTimeHIVChanged, (), (const, override));
    MOCK_METHOD(int, GetTimeSinceHIVChanged, (), (const, override));
    MOCK_METHOD(int, GetLowCD4MonthCount, (), (const, override));

    // PREGNANCY
    MOCK_METHOD(PregnancyState, GetPregnancyState, (), (const, override));
    MOCK_METHOD(int, GetTimeOfPregnancyChange, (), (const, override));
    MOCK_METHOD(int, GetTimeSincePregnancyChange, (), (const, override));
    MOCK_METHOD(int, GetNumMiscarriages, (), (const, override));
    MOCK_METHOD(int, Miscarry, (), (override));
    MOCK_METHOD(int, Stillbirth, (), (override));
    MOCK_METHOD(int, AddChild, (HCV hcv, bool test), (override));
    MOCK_METHOD(int, EndPostpartum, (), (override));
    MOCK_METHOD(int, Impregnate, (), (override));
    MOCK_METHOD(PregnancyDetails, GetPregnancyDetails, (), (const, override));
    MOCK_METHOD(void, SetPregnancyState, (PregnancyState state), (override));
    MOCK_METHOD(void, SetNumMiscarriages, (int miscarriages), (override));
    MOCK_METHOD(void, AddInfantExposure, (), (override));
    MOCK_METHOD(int, GetInfantHCVExposures, (), (const, override));
    MOCK_METHOD(int, GetInfantHCVInfections, (), (const, override));
    MOCK_METHOD(int, GetInfantHCVTests, (), (const, override));
    MOCK_METHOD(int, GetPregnancyCount, (), (const, override));

    MOCK_METHOD(void, TransitionMOUD, (), (override));
    MOCK_METHOD(MOUD, GetMoudState, (), (const, override));
    MOCK_METHOD(int, GetTimeStartedMoud, (), (const, override));
    MOCK_METHOD(MOUDDetails, GetMOUDDetails, (), (const, override));
    MOCK_METHOD(std::vector<Child>, GetChildren, (), (const, override));

    MOCK_METHOD(void, DevelopHCC, (HCCState state), (override));
    MOCK_METHOD(HCCState, GetHCCState, (), (const, override));
    MOCK_METHOD(void, DiagnoseHCC, (), (override));
    MOCK_METHOD(bool, IsDiagnosedWithHCC, (), (const, override));
};
} // namespace person

#endif
