////////////////////////////////////////////////////////////////////////////////
// File: types_test.cpp                                                       //
// Project: hep-ce                                                            //
// Created Date: 2026-04-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-04-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <hepce/data/types.hpp>

#include <sstream>

#include <gtest/gtest.h>

namespace hepce {
namespace testing {

using namespace hepce::data;

TEST(TypesTest, InfectionTypeConversionAndIncrement) {
    InfectionType type = InfectionType::kHcv;
    type << "hiv";
    EXPECT_EQ(type, InfectionType::kHiv);

    ++type;
    EXPECT_EQ(type, InfectionType::kCount);

    type = InfectionType::kHcv;
    ++type;
    EXPECT_EQ(type, InfectionType::kHiv);
}

TEST(TypesTest, LinkageStateInvalidInputFallsBackToNever) {
    LinkageState link = LinkageState::kLinked;

    link << "invalid-state";

    EXPECT_EQ(link, LinkageState::kNever);
}

TEST(TypesTest, FibrosisStateParsesAndIncrements) {
    FibrosisState fibrosis = FibrosisState::kNone;

    fibrosis << "f2";
    EXPECT_EQ(fibrosis, FibrosisState::kF2);

    FibrosisState prior = fibrosis++;
    EXPECT_EQ(prior, FibrosisState::kF2);
    EXPECT_EQ(fibrosis, FibrosisState::kF3);

    ++fibrosis;
    EXPECT_EQ(fibrosis, FibrosisState::kF4);

    ++fibrosis;
    EXPECT_EQ(fibrosis, FibrosisState::kDecomp);

    ++fibrosis;
    EXPECT_EQ(fibrosis, FibrosisState::kDecomp);
}

TEST(TypesTest, MeasuredFibrosisStateParsesAndIncrements) {
    MeasuredFibrosisState measured = MeasuredFibrosisState::kNone;

    measured << "f01";
    EXPECT_EQ(measured, MeasuredFibrosisState::kF01);

    ++measured;
    EXPECT_EQ(measured, MeasuredFibrosisState::kF23);

    MeasuredFibrosisState prior = measured++;
    EXPECT_EQ(prior, MeasuredFibrosisState::kF23);
    EXPECT_EQ(measured, MeasuredFibrosisState::kF4);

    ++measured;
    EXPECT_EQ(measured, MeasuredFibrosisState::kDecomp);
}

TEST(TypesTest, PregnancyStateParsesUnderscoreFormat) {
    PregnancyState state = PregnancyState::kNa;

    state << "restricted_postpartum";
    EXPECT_EQ(state, PregnancyState::kRestrictedPostpartum);

    state << "year_one_postpartum";
    EXPECT_EQ(state, PregnancyState::kYearOnePostpartum);

    state << "year_two_postpartum";
    EXPECT_EQ(state, PregnancyState::kYearTwoPostpartum);
}

TEST(TypesTest, PopulationHeadersReflectFeatureFlags) {
    const std::string baseline = POPULATION_HEADERS();
    const std::string all_features =
        POPULATION_HEADERS(true, true, true, true, true);

    EXPECT_NE(baseline.find("NULL,-1,0,"), std::string::npos);
    EXPECT_NE(baseline.find("false,0,"), std::string::npos);

    EXPECT_NE(all_features.find("hiv,time_hiv_changed,low_cd4_months_count,"),
              std::string::npos);
    EXPECT_NE(all_features.find("currently_overdosing,num_overdoses,"),
              std::string::npos);
    EXPECT_NE(all_features.find("moud_state,time_started_moud,"),
              std::string::npos);
}

TEST(TypesTest, StreamOutputUsesExpectedTokens) {
    std::stringstream ss;

    ss << HCV::kAcute << " " << HIV::kLoSu << " " << Behavior::kInjection << " "
       << ScreeningType::kBackground << " " << ScreeningTest::kRna << " "
       << Sex::kFemale;

    EXPECT_EQ(ss.str(), "acute lo-su injection background rna female");
}

TEST(TypesTest, InvalidStringInputsFallBackToDefaults) {
    HCV hcv = HCV::kAcute;
    HIV hiv = HIV::kHiUn;
    Behavior behavior = Behavior::kInjection;
    ScreeningType screen_type = ScreeningType::kBackground;
    ScreeningTest screen_test = ScreeningTest::kAb;
    Sex sex = Sex::kMale;

    hcv << "unknown";
    hiv << "unknown";
    behavior << "unknown";
    screen_type << "unknown";
    screen_test << "unknown";
    sex << "unknown";

    EXPECT_EQ(hcv, HCV::kNone);
    EXPECT_EQ(hiv, HIV::kNone);
    EXPECT_EQ(behavior, Behavior::kNever);
    EXPECT_EQ(screen_type, ScreeningType::kNa);
    EXPECT_EQ(screen_test, ScreeningTest::kNa);
    EXPECT_EQ(sex, Sex::kFemale);
}

TEST(TypesTest, AdditionalEnumParsingAndStreamingBranches) {
    DeathReason death = DeathReason::kNa;
    death << "background";
    EXPECT_EQ(death, DeathReason::kBackground);
    death << "liver";
    EXPECT_EQ(death, DeathReason::kLiver);
    death << "infection";
    EXPECT_EQ(death, DeathReason::kInfection);
    death << "age";
    EXPECT_EQ(death, DeathReason::kAge);
    death << "overdose";
    EXPECT_EQ(death, DeathReason::kOverdose);
    death << "invalid";
    EXPECT_EQ(death, DeathReason::kNa);

    HCCState hcc = HCCState::kNone;
    hcc << "early";
    EXPECT_EQ(hcc, HCCState::kEarly);
    hcc << "late";
    EXPECT_EQ(hcc, HCCState::kLate);
    hcc << "invalid";
    EXPECT_EQ(hcc, HCCState::kNone);

    MOUD moud = MOUD::kNone;
    moud << "current";
    EXPECT_EQ(moud, MOUD::kCurrent);
    moud << "post";
    EXPECT_EQ(moud, MOUD::kPost);
    moud << "invalid";
    EXPECT_EQ(moud, MOUD::kNone);

    PregnancyState preg = PregnancyState::kNa;
    preg << "pregnant";
    EXPECT_EQ(preg, PregnancyState::kPregnant);
    preg << "none";
    EXPECT_EQ(preg, PregnancyState::kNone);
    preg << "invalid";
    EXPECT_EQ(preg, PregnancyState::kNa);

    std::stringstream stream;
    stream << static_cast<InfectionType>(-1) << " " << DeathReason::kOverdose
           << " " << HCCState::kEarly << " " << MOUD::kPost << " "
           << PregnancyState::kPregnant << " " << static_cast<Sex>(-1);

    EXPECT_EQ(stream.str(), "INVALID overdose early post pregnant ");
}

TEST(TypesTest, IncrementDefaultBranchesDoNotAdvanceInvalidStates) {
    FibrosisState fibrosis = FibrosisState::kNone;
    ++fibrosis;
    EXPECT_EQ(fibrosis, FibrosisState::kNone);

    MeasuredFibrosisState measured = MeasuredFibrosisState::kNone;
    ++measured;
    EXPECT_EQ(measured, MeasuredFibrosisState::kNone);
}

TEST(TypesTest, DetailStructStreamsContainExpectedLabels) {
    HCVDetails hcv;
    hcv.hcv = HCV::kAcute;
    hcv.fibrosis_state = FibrosisState::kF2;
    hcv.is_genotype_three = true;
    hcv.seropositive = true;
    hcv.time_changed = 11;
    hcv.time_fibrosis_state_changed = 12;
    hcv.times_infected = 3;
    hcv.times_acute_cleared = 1;

    HIVDetails hiv;
    hiv.hiv = HIV::kHiUn;
    hiv.time_changed = 13;

    HCCDetails hcc;
    hcc.hcc_state = HCCState::kLate;
    hcc.hcc_diagnosed = true;

    BehaviorDetails behavior;
    behavior.behavior = Behavior::kNoninjection;
    behavior.time_last_active = 15;

    LinkageDetails linkage;
    linkage.link_state = LinkageState::kLinked;
    linkage.time_link_change = 16;
    linkage.link_count = 2;

    MOUDDetails moud;
    moud.moud_state = MOUD::kCurrent;
    moud.time_started_moud = 17;

    PregnancyDetails preg;
    preg.pregnancy_state = PregnancyState::kPregnant;
    preg.time_of_pregnancy_change = 18;
    preg.count = 1;
    preg.num_infants = 2;
    preg.num_stillbirths = 1;
    preg.num_hcv_exposures = 3;
    preg.num_hcv_infections = 1;
    preg.num_hcv_tests = 2;

    Child child;
    child.hcv = HCV::kChronic;
    child.tested = true;

    StagingDetails staging;
    staging.measured_fibrosis_state = MeasuredFibrosisState::kF23;
    staging.had_second_test = true;
    staging.time_of_last_staging = 21;

    ScreeningDetails screening;
    screening.time_of_last_screening = 22;
    screening.num_ab_tests = 3;
    screening.num_rna_tests = 4;
    screening.ab_positive = true;
    screening.identified = true;
    screening.time_identified = 23;
    screening.screen_type = ScreeningType::kIntervention;
    screening.identifications_cleared = 5;

    TreatmentDetails treatment;
    treatment.initiated_treatment = true;
    treatment.time_of_treatment_initiation = 24;
    treatment.num_starts = 1;
    treatment.num_withdrawals = 2;
    treatment.num_toxic_reactions = 3;
    treatment.num_completed = 4;
    treatment.num_salvages = 5;
    treatment.in_salvage_treatment = true;

    LifetimeUtility util;
    util.mult_util = 0.7;
    util.min_util = 0.6;
    util.discount_mult_util = 0.5;
    util.discount_min_util = 0.4;

    std::stringstream ss;
    ss << hcv << hiv << hcc << behavior << linkage << moud << preg << child
       << staging << screening << treatment << util;

    const std::string rendered = ss.str();
    EXPECT_NE(rendered.find("HCV State:"), std::string::npos);
    EXPECT_NE(rendered.find("HIV State:"), std::string::npos);
    EXPECT_NE(rendered.find("HCC State:"), std::string::npos);
    EXPECT_NE(rendered.find("Behavior Type:"), std::string::npos);
    EXPECT_NE(rendered.find("Linkage State:"), std::string::npos);
    EXPECT_NE(rendered.find("MOUD State:"), std::string::npos);
    EXPECT_NE(rendered.find("Pregnancy State:"), std::string::npos);
    EXPECT_NE(rendered.find("Fibrosis State:"), std::string::npos);
    EXPECT_NE(rendered.find("Time of Last Screening:"), std::string::npos);
    EXPECT_NE(rendered.find("Has the person Initiated Treatment:"),
              std::string::npos);
    EXPECT_NE(rendered.find("Multiplicative Utility:"), std::string::npos);
}

TEST(TypesTest, LifetimeUtilityEqualityComparesAllFields) {
    LifetimeUtility lhs;
    lhs.mult_util = 1.0;
    lhs.min_util = 0.5;
    lhs.discount_mult_util = 0.9;
    lhs.discount_min_util = 0.45;

    LifetimeUtility rhs = lhs;
    EXPECT_TRUE(lhs == rhs);

    rhs.discount_min_util = 0.4;
    EXPECT_FALSE(lhs == rhs);
}

} // namespace testing
} // namespace hepce
