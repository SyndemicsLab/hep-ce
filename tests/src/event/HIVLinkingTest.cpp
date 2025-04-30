////////////////////////////////////////////////////////////////////////////////
// File: HIVLinkingTest.cpp                                                   //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-09                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVLinking.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class HIVLinkingTest : public EventTest {
protected:
    person::InfectionType it = person:: ::kHiv;
    cost::CostCategory cc = cost::CostCategory::HIV;
    // person strata used throughout
    // 25 years / 300 months old, male, never behavior state, kNa pregnancy
    Utils::tuple_4i tup_4i = std::make_tuple(25, 0, 0, -1);
    // pair representing background, intervention
    std::pair<double, double> link_prob = {0.4, 0.8};
    double false_positive_test_cost = 500.00;
    double intervention_cost = 120.00;
    double discount_rate = 0.025;
    int current_timestep = 1;

    void SetUp() override {
        EventTest::SetUp();

        // Person setup
        ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
        ON_CALL(*testPerson, GetSex())
            .WillByDefault(Return(person::Sex::kMale));
        ON_CALL(*testPerson, GetBehavior())
            .WillByDefault(Return(person::Behavior::kNever));
        ON_CALL(*testPerson, GetPregnancyState())
            .WillByDefault(Return(person::PregnancyState::kNa));
        ON_CALL(*testPerson, GetHIV())
            .WillByDefault(Return(person::HIV::kHiUn));
        ON_CALL(*testPerson, GetLinkState(it))
            .WillByDefault(Return(person::LinkageState::kNever));
        ON_CALL(*testPerson, IsIdentifiedAsInfected(it))
            .WillByDefault(Return(true));
        ON_CALL(*testPerson, GetLinkageType(it))
            .WillByDefault(Return(person::LinkageType::kBackground));
        ON_CALL(*testPerson, GetCurrentTimestep())
            .WillByDefault(Return(current_timestep));
        ON_CALL(*testPerson, GetTimeSinceLastScreening(it))
            .WillByDefault(Return(3));

        // DataManager setup
        // catches hcv linking definition needs
        ON_CALL(*event_dm, GetFromConfig("simulation.events", _))
            .WillByDefault(DoAll(SetArgReferee<1>("HIVLinking"), Return(0)));
        ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
            .WillByDefault(DoAll(
                SetArgReferee<1>(std::to_string(discount_rate)), Return(0)));
        ON_CALL(*event_dm, GetFromConfig("hiv_linking.intervention_cost", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(intervention_cost)),
                      Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_linking.false_positive_test_cost", _))
            .WillByDefault(DoAll(
                SetArgReferee<1>(std::to_string(false_positive_test_cost)),
                Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_linking.recent_screen_multiplier", _))
            .WillByDefault(DoAll(SetArgReferee<1>("2.0"), Return(0)));
        ON_CALL(*event_dm, GetFromConfig("hiv_linking.recent_screen_cutoff", _))
            .WillByDefault(DoAll(SetArgReferee<1>("2"), Return(0)));
    }
};

const std::string BGLinkSQL = "SELECT age_years, gender, drug_behavior, -1, "
                              "hiv_background_link_probability"
                              " FROM screening_and_linkage;";
const std::string INTLinkSQL = "SELECT age_years, gender, drug_behavior, -1, "
                               "hiv_intervention_link_probability"
                               " FROM screening_and_linkage;";

TEST_F(HIVLinkingTest, FalsePositive) {
    // Person setup
    ON_CALL(*testPerson, GetHIV()).WillByDefault(Return(person::HIV::kNone));

    // DataManager setup
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        instorage, bgstorage;
    bgstorage[tup_4i] = link_prob.first;
    instorage[tup_4i] = link_prob.second;
    ON_CALL(*event_dm, SelectCustomCallback(BGLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bgstorage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(INTLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&instorage), Return(0)));

    // Expectations
    // because it's a false positive, no decision is ever made
    EXPECT_CALL(*decider, GetDecision(_)).Times(0);
    EXPECT_CALL(*testPerson, ClearDiagnosis(it)).Times(1);
    double discounted_cost = Utils::discount(false_positive_test_cost,
                                             discount_rate, current_timestep);
    EXPECT_CALL(*testPerson,
                AddCost(false_positive_test_cost, discounted_cost, cc))
        .Times(1);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVLinkingTest, DecideNotToLink) {
    // DataManager setup
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        instorage, bgstorage;
    bgstorage[tup_4i] = link_prob.first;
    instorage[tup_4i] = link_prob.second;
    ON_CALL(*event_dm, SelectCustomCallback(BGLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bgstorage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(INTLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&instorage), Return(0)));

    // Expectations
    std::vector<double> expected_link_prob = {link_prob.first};
    EXPECT_CALL(*decider, GetDecision(expected_link_prob))
        .Times(1)
        .WillOnce(Return(1));
    EXPECT_CALL(*testPerson, Link(_, it)).Times(0);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVLinkingTest, BackgroundLink) {
    // DataManager setup
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        instorage, bgstorage;
    bgstorage[tup_4i] = link_prob.first;
    instorage[tup_4i] = link_prob.second;
    ON_CALL(*event_dm, SelectCustomCallback(BGLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bgstorage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(INTLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&instorage), Return(0)));

    // Expectations
    std::vector<double> expected_link_prob = {link_prob.first};
    EXPECT_CALL(*decider, GetDecision(expected_link_prob))
        .Times(1)
        .WillOnce(Return(0));
    EXPECT_CALL(*testPerson, Link(person::LinkageType::kBackground, it))
        .Times(1);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVLinkingTest, InterventionLink) {
    // Person setup
    ON_CALL(*testPerson, GetLinkageType(it))
        .WillByDefault(Return(person::LinkageType::kIntervention));

    // DataManager setup
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        instorage, bgstorage;
    bgstorage[tup_4i] = link_prob.first;
    instorage[tup_4i] = link_prob.second;
    ON_CALL(*event_dm, SelectCustomCallback(BGLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bgstorage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(INTLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&instorage), Return(0)));

    // Expectations
    std::vector<double> expected_link_prob = {link_prob.second};
    EXPECT_CALL(*decider, GetDecision(expected_link_prob))
        .Times(1)
        .WillOnce(Return(0));
    EXPECT_CALL(*testPerson, Link(person::LinkageType::kIntervention, it))
        .Times(1);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVLinkingTest, AlreadyLinked) {
    // Person setup
    ON_CALL(*testPerson, GetLinkState(it))
        .WillByDefault(Return(person::LinkageState::kLinked));

    // DataManager setup
    std::unordered_map<Utils::tuple_4i, double, Utils::key_hash_4i,
                       Utils::key_equal_4i>
        instorage, bgstorage;
    bgstorage[tup_4i] = link_prob.first;
    instorage[tup_4i] = link_prob.second;
    ON_CALL(*event_dm, SelectCustomCallback(BGLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&bgstorage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(INTLinkSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_Double(&instorage), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_)).Times(0);
    EXPECT_CALL(*testPerson, Link(_, _)).Times(0);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVLinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
