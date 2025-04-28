////////////////////////////////////////////////////////////////////////////////
// File: HCVInfectionsTest.cpp                                                   //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HCVInfections.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class HCVInfectionsTest : public EventTest {};

std::string const INCIDENCE_QUERY =
    "SELECT age_years, gender, drug_behavior, incidence FROM "
    "incidence;";

TEST_F(HCVInfectionsTest, HCVInfections_NewInfection) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kMale));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kNever));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kNone));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("infection.genotype_three_prob", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));

    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Incidence Setup
    double incidence = 1.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0)); // Infect

    // Expectations
    // infection probability
    std::vector<double> expected_infection_prob = {incidence};
    EXPECT_CALL(*decider, GetDecision(expected_infection_prob)).Times(1);
    // genotype three probability
    std::vector<double> genotype_three_prob = {0.5};
    EXPECT_CALL(*decider, GetDecision(genotype_three_prob)).Times(1);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVInfections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVInfectionsTest, HCVInfections_DoNotInfect) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kMale));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kNever));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kNone));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("infection.genotype_three_prob", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));

    // Incidence Setup
    double incidence = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1)); // Do Not Infect

    // Expectations
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVInfections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVInfectionsTest, HCVInfections_AcuteProgression) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kMale));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kNever));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));
    ON_CALL(*testPerson, GetTimeSinceHCVChanged()).WillByDefault(Return(6));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("infection.genotype_three_prob", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));

    // Incidence Setup
    double incidence = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, SetHCV(person::HCV::kChronic)).Times(1);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVInfections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVInfectionsTest, HCVInfections_NoAcuteProgression) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kMale));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kNever));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));
    ON_CALL(*testPerson, GetTimeSinceHCVChanged()).WillByDefault(Return(5));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("infection.genotype_three_prob", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));

    // Incidence Setup
    double incidence = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1)); // Do Not Infect

    // Expectations
    EXPECT_CALL(*testPerson, SetHCV(_)).Times(0);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVInfections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVInfectionsTest, HCVInfections_HandleChronicHCV) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kMale));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kNever));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kChronic));
    ON_CALL(*testPerson, GetTimeSinceHCVChanged()).WillByDefault(Return(5));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("infection.genotype_three_prob", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));

    // Incidence Setup
    double incidence = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, SetHCV(_)).Times(0);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVInfections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVInfectionsTest, HCVInfections_NewInfectionGenotype3) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kMale));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kNever));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kNone));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("infection.genotype_three_prob", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Incidence Setup
    double incidence = 1.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0)); // Infect

    // Expectations
    EXPECT_CALL(*testPerson, InfectHCV()).Times(1);
    EXPECT_CALL(*testPerson, SetGenotypeThree(true)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVInfections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVInfectionsTest, HCVInfections_NewInfectionNotGenotype3) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kMale));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kNever));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kNone));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("infection.genotype_three_prob", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Incidence Setup
    double incidence = 1.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0)) // Infect
        .WillRepeatedly(Return(1));

    // Expectations
    EXPECT_CALL(*testPerson, InfectHCV()).Times(1);
    EXPECT_CALL(*testPerson, SetGenotypeThree(_)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVInfections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
