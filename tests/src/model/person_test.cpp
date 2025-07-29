////////////////////////////////////////////////////////////////////////////////
// File: person_test.cpp                                                      //
// Project: hep-ce                                                            //
// Created Date: 2025-05-09                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-23                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/model/person.hpp>

// STL Libraries
#include <filesystem>

// 3rd Party Dependencies
#include <datamanagement/datamanagement.hpp>
#include <gtest/gtest.h>

// Library Includes
#include <hepce/data/types.hpp>
#include <hepce/utils/formatting.hpp>
#include <hepce/utils/logging.hpp>

using namespace hepce::model;
using namespace hepce::data;

namespace hepce {
namespace testing {

class PersonTest : public ::testing::Test {
protected:
    const std::string LOG_NAME = "PersonTest";
    const InfectionType TYPE = InfectionType::kHcv;
    std::unique_ptr<Person> person;
    static void SetUpTestSuite() {
        const std::string log_name = "PersonTest";
        const std::string log_file = log_name + ".log";
        hepce::utils::CreateFileLogger(log_name, log_file);
    }

    static void TearDownTestSuite() {
        std::filesystem::remove("PersonTest.log");
    }

    // Creating the Person object and growing to set the Current Time to 3
    void SetUp() override {
        person = Person::Create(LOG_NAME);
        person->Grow();
        person->Grow();
        person->Grow();
    }
    void TearDown() override {}
};

// Functionality Testing
TEST_F(PersonTest, Grow) {
    int b_time = person->GetCurrentTimestep();
    int b_age = person->GetAge();
    int b_ls = person->GetLifeSpan();

    person->Grow();

    EXPECT_EQ(person->GetCurrentTimestep(), b_time + 1);
    EXPECT_EQ(person->GetAge(), b_age + 1);
    EXPECT_EQ(person->GetLifeSpan(), b_ls + 1);
}

TEST_F(PersonTest, UpdateTimers) {
    person->SetBehavior(Behavior::kNoninjection);
    person->SetMoudState(MOUD::kCurrent);
    person->SetHIV(HIV::kLoUn);

    int b_behavior = person->GetBehaviorDetails().time_last_active;
    int b_moud = person->GetMoudDetails().total_moud_months;
    int b_hiv = person->GetHIVDetails().low_cd4_months_count;

    person->Grow();

    EXPECT_EQ(person->GetBehaviorDetails().time_last_active, b_behavior + 1);
    EXPECT_EQ(person->GetMoudDetails().total_moud_months, b_moud + 1);
    EXPECT_EQ(person->GetHIVDetails().low_cd4_months_count, b_hiv + 1);
}

TEST_F(PersonTest, Die) {
    person->Die(DeathReason::kAge);
    EXPECT_FALSE(person->IsAlive());
    EXPECT_EQ(person->GetDeathReason(), DeathReason::kAge);
}

TEST_F(PersonTest, SetPersonDetails) {
    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kNone);

    PersonSelect person_select;
    person_select.hcv = HCV::kChronic;
    person->SetPersonDetails(person_select);

    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kChronic);
}

// HCV Testing
TEST_F(PersonTest, InfectHCV_PriorInfection) {
    person->SetHCV(HCV::kChronic);
    person->InfectHCV();
    EXPECT_EQ(person->GetHCVDetails().times_infected, 0); // Default is 0
}

TEST_F(PersonTest, InfectHCV_NewInfection) {
    person->InfectHCV();
    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kAcute);
    EXPECT_EQ(person->GetHCVDetails().times_infected, 1);
    EXPECT_EQ(person->GetHCVDetails().fibrosis_state, FibrosisState::kF0);
}

TEST_F(PersonTest, ClearHCV_NotAcute) {
    person->SetHCV(HCV::kChronic);
    person->ClearHCV(false);

    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kNone);
    EXPECT_EQ(person->GetHCVDetails().times_acute_cleared, 0);
}

TEST_F(PersonTest, ClearHCV_Acute) {
    person->SetHCV(HCV::kAcute);
    person->ClearHCV(true);

    EXPECT_EQ(person->GetHCVDetails().hcv, HCV::kNone);
    EXPECT_EQ(person->GetHCVDetails().times_acute_cleared, 1);
}

TEST_F(PersonTest, Diagnose) {
    person->Diagnose(TYPE);
    EXPECT_TRUE(person->GetScreeningDetails(TYPE).identified);
    EXPECT_TRUE(person->GetScreeningDetails(TYPE).ab_positive);
}

TEST_F(PersonTest, ClearDiagnosis) {
    person->Diagnose(TYPE);
    person->ClearDiagnosis(TYPE);
    EXPECT_FALSE(person->GetScreeningDetails(TYPE).identified);
    EXPECT_TRUE(person->GetScreeningDetails(TYPE).ab_positive);
    EXPECT_EQ(person->GetScreeningDetails(TYPE).times_identified, 1);
    EXPECT_EQ(person->GetScreeningDetails(TYPE).time_identified,
              person->GetCurrentTimestep());
}

TEST_F(PersonTest, FalsePositive) {
    person->Diagnose(TYPE);
    person->FalsePositive(TYPE);
    EXPECT_FALSE(person->GetScreeningDetails(TYPE).identified);
    EXPECT_FALSE(person->GetScreeningDetails(TYPE).ab_positive);
    EXPECT_EQ(person->GetScreeningDetails(TYPE).times_identified, 0);
    EXPECT_EQ(person->GetScreeningDetails(TYPE).time_identified, -1);
}

TEST_F(PersonTest, IsCirrhotic_False) {
    person->SetFibrosis(FibrosisState::kF1);
    EXPECT_FALSE(person->IsCirrhotic());
}

TEST_F(PersonTest, IsCirrhotic_F4) {
    person->SetFibrosis(FibrosisState::kF4);
    EXPECT_TRUE(person->IsCirrhotic());
}

TEST_F(PersonTest, IsCirrhotic_DECOMP) {
    person->SetFibrosis(FibrosisState::kDecomp);
    EXPECT_TRUE(person->IsCirrhotic());
}

TEST_F(PersonTest, SetFibrosis) {
    person->SetFibrosis(FibrosisState::kF3);
    auto hcv = person->GetHCVDetails();

    EXPECT_EQ(hcv.fibrosis_state, FibrosisState::kF3);
    EXPECT_EQ(hcv.time_fibrosis_state_changed, person->GetCurrentTimestep());
}

TEST_F(PersonTest, AddSVR) {
    person->AddSVR();
    EXPECT_EQ(person->GetHCVDetails().svrs, 1);
}

// Screening Testing
TEST_F(PersonTest, Screen_Ab) {
    person->Screen(TYPE, ScreeningTest::kAb, ScreeningType::kBackground);

    int time = person->GetCurrentTimestep();
    EXPECT_EQ(person->GetScreeningDetails(TYPE).time_of_last_screening, time);
    EXPECT_EQ(person->GetScreeningDetails(TYPE).num_ab_tests, 1);
}

TEST_F(PersonTest, Screen_Rna) {
    person->Screen(TYPE, ScreeningTest::kRna, ScreeningType::kBackground);

    int time = person->GetCurrentTimestep();
    EXPECT_EQ(person->GetScreeningDetails(TYPE).time_of_last_screening, time);
    EXPECT_EQ(person->GetScreeningDetails(TYPE).num_rna_tests, 1);
}

// Linking Testing
TEST_F(PersonTest, Link) {
    person->Link(TYPE);

    auto linkage = person->GetLinkageDetails(TYPE);
    EXPECT_EQ(linkage.link_state, LinkageState::kLinked);
    EXPECT_EQ(linkage.time_link_change, person->GetCurrentTimestep());
    EXPECT_EQ(linkage.link_count, 1);
}

TEST_F(PersonTest, Unlink) {
    person->Link(TYPE);
    person->Grow();
    person->Grow();
    person->Unlink(TYPE);

    auto linkage = person->GetLinkageDetails(TYPE);
    EXPECT_EQ(linkage.link_state, LinkageState::kUnlinked);
    EXPECT_EQ(linkage.time_link_change, person->GetCurrentTimestep());
}

// Treatment Testing
TEST_F(PersonTest, AddWithdrawal) {
    person->AddWithdrawal(TYPE);
    EXPECT_EQ(person->GetTreatmentDetails(TYPE).num_withdrawals, 1);
}

TEST_F(PersonTest, AddToxicReaction) {
    person->AddToxicReaction(TYPE);
    EXPECT_EQ(person->GetTreatmentDetails(TYPE).num_toxic_reactions, 1);
}

TEST_F(PersonTest, AddCompletedTreatment) {
    person->AddCompletedTreatment(TYPE);
    EXPECT_EQ(person->GetTreatmentDetails(TYPE).num_completed, 1);
}

TEST_F(PersonTest, InitiateTreatment_FirstTime) {
    person->InitiateTreatment(TYPE);
    auto details = person->GetTreatmentDetails(TYPE);

    EXPECT_TRUE(details.initiated_treatment);
    EXPECT_EQ(details.num_starts, 1);
    EXPECT_EQ(details.time_of_treatment_initiation,
              person->GetCurrentTimestep());
}

TEST_F(PersonTest, InitiateTreatment_GoToSalvage) {
    PersonSelect person_select;
    person_select.initiated_hcv_treatment = true;
    person->SetPersonDetails(person_select);

    person->InitiateTreatment(TYPE);
    auto details = person->GetTreatmentDetails(TYPE);

    EXPECT_TRUE(details.in_salvage_treatment);
}

TEST_F(PersonTest, InitiateTreatment_FailedSalvage) {
    PersonSelect person_select;
    person_select.in_hcv_salvage = true;
    person->SetPersonDetails(person_select);

    person->InitiateTreatment(TYPE);
    auto details = person->GetTreatmentDetails(TYPE);

    EXPECT_TRUE(details.in_salvage_treatment);
    EXPECT_EQ(details.num_starts, 0);
}

TEST_F(PersonTest, EndTreatment) {
    PersonSelect person_select;
    person_select.initiated_hcv_treatment = true;
    person_select.in_hcv_salvage = true;
    person->SetPersonDetails(person_select);

    person->EndTreatment(TYPE);
    auto details = person->GetTreatmentDetails(TYPE);

    EXPECT_FALSE(details.in_salvage_treatment);
    EXPECT_FALSE(details.initiated_treatment);
}

// Drug Use Behavior Testing
TEST_F(PersonTest, SetBehavior_Standard) {
    PersonSelect person_select;
    person_select.drug_behavior = Behavior::kNever;
    person->SetPersonDetails(person_select);

    person->SetBehavior(Behavior::kInjection);

    auto behavior = person->GetBehaviorDetails();
    EXPECT_EQ(behavior.behavior, Behavior::kInjection);
    EXPECT_EQ(behavior.time_last_active, person->GetCurrentTimestep());
}

TEST_F(PersonTest, SetBehavior_Nonactive) {
    person->SetBehavior(Behavior::kInjection);
    person->Grow();
    person->Grow();
    person->SetBehavior(Behavior::kFormerInjection);
    int time = person->GetCurrentTimestep();
    person->Grow();
    person->Grow();

    auto behavior = person->GetBehaviorDetails();
    EXPECT_EQ(behavior.behavior, Behavior::kFormerInjection);
    EXPECT_EQ(behavior.time_last_active, time);
}

TEST_F(PersonTest, SetBehavior_NoGoingBackToNever) {
    PersonSelect person_select;
    person_select.drug_behavior = Behavior::kInjection;
    person->SetPersonDetails(person_select);
    person->Grow();
    person->Grow();
    person->Grow();
    person->SetBehavior(Behavior::kNever);
    person->Grow();
    person->Grow();

    auto behavior = person->GetBehaviorDetails();
    EXPECT_EQ(behavior.behavior, Behavior::kInjection);
    EXPECT_EQ(behavior.time_last_active, person->GetCurrentTimestep());
}

// Fibrosis Testing
TEST_F(PersonTest, DiagnoseFibrosis) {
    person->DiagnoseFibrosis(MeasuredFibrosisState::kF23);

    auto staging = person->GetFibrosisStagingDetails();
    EXPECT_EQ(staging.measured_fibrosis_state, MeasuredFibrosisState::kF23);
    EXPECT_EQ(staging.time_of_last_staging, person->GetCurrentTimestep());
}

TEST_F(PersonTest, GiveSecondStagingTest) {
    EXPECT_FALSE(person->GetFibrosisStagingDetails().had_second_test);
    person->GiveSecondStagingTest();
    EXPECT_TRUE(person->GetFibrosisStagingDetails().had_second_test);
}

// Cost Effectiveness
TEST_F(PersonTest, AddCost) {
    double base = 123.45;
    double discount = 91.23;
    CostCategory cc = CostCategory::kLinking;
    person->AddCost(base, discount, cc);
    auto costs = person->GetCosts();
    EXPECT_EQ(costs[cc].first, base);
    EXPECT_EQ(costs[cc].second, discount);
}

TEST_F(PersonTest, Stillbirth) {
    person->Stillbirth();
    auto details = person->GetPregnancyDetails();

    EXPECT_EQ(details.num_stillbirths, 1);
    EXPECT_EQ(details.time_of_pregnancy_change, person->GetCurrentTimestep());
    EXPECT_EQ(details.pregnancy_state, PregnancyState::kRestrictedPostpartum);
}

TEST_F(PersonTest, Birth_NoHCV_NoTest) {
    Child child = {HCV::kNone, false};
    person->Birth(child);
    auto details = person->GetPregnancyDetails();

    EXPECT_EQ(details.num_infants, 1);
    EXPECT_FALSE(details.children.empty());
    EXPECT_EQ(details.pregnancy_state, PregnancyState::kRestrictedPostpartum);
}

TEST_F(PersonTest, Birth_HCV_Test) {
    Child child = {HCV::kAcute, true};
    person->Birth(child);
    auto details = person->GetPregnancyDetails();

    EXPECT_EQ(details.num_hcv_infections, 1);
    EXPECT_EQ(details.num_hcv_tests, 1);
    EXPECT_EQ(details.num_infants, 1);
    EXPECT_FALSE(details.children.empty());
    EXPECT_EQ(details.pregnancy_state, PregnancyState::kRestrictedPostpartum);
}

TEST_F(PersonTest, EndPostpartum) {
    PersonSelect person_select;
    person_select.pregnancy_state = PregnancyState::kYearTwoPostpartum;
    person->SetPersonDetails(person_select);

    person->EndPostpartum();
    auto details = person->GetPregnancyDetails();

    EXPECT_EQ(details.time_of_pregnancy_change, person->GetCurrentTimestep());
    EXPECT_EQ(details.pregnancy_state, PregnancyState::kNone);
}

TEST_F(PersonTest, Impregnate) {
    person->Impregnate();
    auto details = person->GetPregnancyDetails();

    EXPECT_EQ(details.count, 1);
    EXPECT_EQ(details.time_of_pregnancy_change, person->GetCurrentTimestep());
    EXPECT_EQ(details.pregnancy_state, PregnancyState::kPregnant);
}

TEST_F(PersonTest, InfantExposure) {
    person->AddInfantExposure();
    EXPECT_EQ(person->GetPregnancyDetails().num_hcv_exposures, 1);
}

// Utility Testing
TEST_F(PersonTest, Utility) {
    person->SetUtility(.5, UtilityCategory::kBehavior);
    auto utils = person->GetUtilities();
    EXPECT_EQ(utils[UtilityCategory::kBehavior], .5);
}

TEST_F(PersonTest, Accumulate) {
    person->SetUtility(.5, UtilityCategory::kBehavior);
    person->AccumulateTotalUtility(.25);
    auto total_utils = person->GetTotalUtility();
    EXPECT_EQ(total_utils.mult_util, .5);
    EXPECT_EQ(total_utils.min_util, .5);
}

// Not sure how to test this, its a dump of the data into a CSV string
TEST_F(PersonTest, MakePopulationRow) {
    std::string str = person->MakePopulationRow();
    EXPECT_EQ(utils::SplitToVecT<std::string>(str, ',').size(), 87);
}

TEST_F(PersonTest, LastTimeActiveLessThanNegOne) {
    PersonSelect person_select;
    person_select.time_last_active_drug_use = -6;
    person->SetPersonDetails(person_select);
    EXPECT_EQ(person->GetBehaviorDetails().time_last_active, -6);
}
} // namespace testing
} // namespace hepce
