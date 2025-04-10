////////////////////////////////////////////////////////////////////////////////
// File: HCVLinkingTest.cpp                                                      //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-08                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HCVLinking.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class HCVLinkingTest : public EventTest {};

std::string const BACKGROUND_LINK_QUERY =
    "SELECT age_years, gender, drug_behavior, -1, "
    "background_link_probability FROM "
    "screening_and_linkage;";

std::string const INTERVENTION_LINK_QUERY =
    "SELECT age_years, gender, drug_behavior, -1, "
    "intervention_link_probability FROM "
    "screening_and_linkage;";

std::string const P_BACKGROUND_LINK_QUERY =
    "SELECT age_years, gender, drug_behavior, pregnancy, "
    "background_link_probability FROM "
    "screening_and_linkage;";

std::string const P_INTERVENTION_LINK_QUERY =
    "SELECT age_years, gender, drug_behavior, pregnancy, "
    "intervention_link_probability FROM "
    "screening_and_linkage;";

TEST_F(HCVLinkingTest, FalsePositive) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::NONE));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person::InfectionType::HCV))
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::NA));

    // Data Setup
    double false_positive_test_cost = 12.00;
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(false_positive_test_cost)),
                  Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_cutoff", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 0.5;
    Utils::tuple_4i tup_4i = std::make_tuple(25, 0, 0, -1);
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        bstorage;
    bstorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.5;
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        istorage;
    istorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&istorage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, ClearDiagnosis(person::InfectionType::HCV))
        .Times(1);
    EXPECT_CALL(*testPerson,
                AddCost(false_positive_test_cost, false_positive_test_cost,
                        cost::CostCategory::LINKING))
        .Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVLinkingTest, BackgroundLink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person::InfectionType::HCV))
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetLinkageType(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageType::BACKGROUND));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::NA));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_cutoff", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 1.0;
    Utils::tuple_4i tup_4i = std::make_tuple(25, 0, 0, -1);
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        bstorage;
    bstorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.0;
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        istorage;
    istorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    std::vector<double> expected_prob = {1.0};
    EXPECT_CALL(*decider, GetDecision(expected_prob)).Times(1);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND,
                                  person::InfectionType::HCV))
        .Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVLinkingTest, InterventionLink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person::InfectionType::HCV))
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageState::NEVER));
    ON_CALL(*testPerson, GetLinkageType(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageType::INTERVENTION));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::NA));

    // Data Setup
    double intervention_cost = 100.00;
    ON_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(
            SetArgReferee<1>(std::to_string(intervention_cost)), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_cutoff", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 0.0;
    Utils::tuple_4i tup_4i = std::make_tuple(25, 0, 0, -1);
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        bstorage;
    bstorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 1.0;
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        istorage;
    istorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    std::vector<double> expected_link_prob = {link_prob};
    EXPECT_CALL(*decider, GetDecision(expected_link_prob)).Times(1);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::INTERVENTION,
                                  person::InfectionType::HCV))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(intervention_cost, intervention_cost,
                                     cost::CostCategory::LINKING))
        .Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVLinkingTest, DecideToNotLink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person::InfectionType::HCV))
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetLinkageType(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageType::INTERVENTION));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::NA));
    ON_CALL(*testPerson, GetTimeSinceLastScreening(person::InfectionType::HCV))
        .WillByDefault(Return(1));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_cutoff", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0"), Return(0)));

    // Background Link Setup
    double link_prob = 0.0;
    Utils::tuple_4i tup_4i = std::make_tuple(25, 0, 0, -1);
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        bstorage;
    bstorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.0;
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        istorage;
    istorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1));

    // Expectations
    std::vector<double> expected_link_prob = {0.0};
    EXPECT_CALL(*decider, GetDecision(expected_link_prob)).Times(1);
    EXPECT_CALL(*testPerson, GetLinkageType(person::InfectionType::HCV))
        .Times(1);
    EXPECT_CALL(*testPerson, Link(_, _)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVLinkingTest, AlreadyLinked) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person::InfectionType::HCV))
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageState::LINKED));
    ON_CALL(*testPerson, GetLinkageType(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageType::INTERVENTION));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::NA));
    ON_CALL(*testPerson, GetTimeSinceLastScreening(person::InfectionType::HCV))
        .WillByDefault(Return(1));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 0.0;
    Utils::tuple_4i tup_4i = std::make_tuple(25, 0, 0, -1);
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        bstorage;
    bstorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.0;
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        istorage;
    istorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_)).Times(0);
    EXPECT_CALL(*testPerson, Link(_, _)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVLinkingTest, RecentScreen) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person::InfectionType::HCV))
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetLinkageType(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageType::BACKGROUND));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::NA));
    ON_CALL(*testPerson, GetTimeSinceLastScreening(person::InfectionType::HCV))
        .WillByDefault(Return(0));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("2.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_cutoff", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 0.8;
    Utils::tuple_4i tup_4i = std::make_tuple(25, 0, 0, -1);
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        bstorage;
    bstorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.0;
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        istorage;
    istorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    std::vector<double> expected_probs = {0.96};
    EXPECT_CALL(*decider, GetDecision(expected_probs)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HCVLinkingTest, RecentScreenCutoff) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person::InfectionType::HCV))
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetLinkageType(person::InfectionType::HCV))
        .WillByDefault(Return(person::LinkageType::BACKGROUND));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::NA));
    ON_CALL(*testPerson, GetTimeSinceLastScreening(person::InfectionType::HCV))
        .WillByDefault(Return(1));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("2.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.recent_screen_cutoff", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 0.8;
    Utils::tuple_4i tup_4i = std::make_tuple(25, 0, 0, -1);
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        bstorage;
    bstorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.0;
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        istorage;
    istorage[tup_4i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    std::vector<double> expected_probs = {0.8};
    EXPECT_CALL(*decider, GetDecision(expected_probs)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("HCVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
