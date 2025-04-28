////////////////////////////////////////////////////////////////////////////////
// File: FibrosisProgressionTest.cpp                                          //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "FibrosisProgression.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class FibrosisProgressionTest : public EventTest {};

std::string const SQL_QUERY =
    "SELECT hcv_status, fibrosis_state, cost, utility FROM hcv_impacts;";

TEST_F(FibrosisProgressionTest, FibrosisProgression_NoHCV) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("false"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost & Utility Setup
    cost_util custorage = {25.25, 0.8};
    Utils::tuple_2i tup_2i = std::make_tuple(0, 6);
    std::unordered_map<Utils::tuple_2i, struct cost_util, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        storage;
    storage[tup_2i] = custorage;
    ON_CALL(*event_dm, SelectCustomCallback(SQL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_CU(&storage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, GetHCV()).WillOnce(Return(person::HCV::kNone));
    EXPECT_CALL(*testPerson, GetTrueFibrosisState()).Times(0);
    EXPECT_CALL(*testPerson, SetUtility(_, _)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F01) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost & Utility Setup
    cost_util custorage = {25.25, 0.8};
    Utils::tuple_2i tup_2i = std::make_tuple(1, 1);
    std::unordered_map<Utils::tuple_2i, struct cost_util, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        storage;
    storage[tup_2i] = custorage;
    ON_CALL(*event_dm, SelectCustomCallback(SQL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_CU(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::kF1));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person:: ::kHcv))
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, GetTrueFibrosisState())
        .WillOnce(Return(person::FibrosisState::kF0))
        .WillOnce(Return(person::FibrosisState::kF0))
        .WillRepeatedly(Return(person::FibrosisState::kF1));
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::kF1))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);
    EXPECT_CALL(*testPerson, SetUtility(custorage.util, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F12) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost & Utility Setup
    cost_util custorage = {25.25, 0.8};
    Utils::tuple_2i tup_2i = std::make_tuple(1, 2);
    std::unordered_map<Utils::tuple_2i, struct cost_util, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        storage;
    storage[tup_2i] = custorage;
    ON_CALL(*event_dm, SelectCustomCallback(SQL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_CU(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::kF2));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person:: ::kHcv))
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, GetTrueFibrosisState())
        .WillOnce(Return(person::FibrosisState::kF1))
        .WillOnce(Return(person::FibrosisState::kF1))
        .WillRepeatedly(Return(person::FibrosisState::kF2));
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::kF2))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);
    EXPECT_CALL(*testPerson, SetUtility(custorage.util, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F23) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost & Utility Setup
    cost_util custorage = {25.25, 0.8};
    Utils::tuple_2i tup_2i = std::make_tuple(1, 3);
    std::unordered_map<Utils::tuple_2i, struct cost_util, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        storage;
    storage[tup_2i] = custorage;
    ON_CALL(*event_dm, SelectCustomCallback(SQL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_CU(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person:: ::kHcv))
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, GetTrueFibrosisState())
        .WillOnce(Return(person::FibrosisState::kF2))
        .WillOnce(Return(person::FibrosisState::kF2))
        .WillRepeatedly(Return(person::FibrosisState::kF3));
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::kF3))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);
    EXPECT_CALL(*testPerson, SetUtility(custorage.util, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F34) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost & Utility Setup
    cost_util custorage = {25.25, 0.8};
    Utils::tuple_2i tup_2i = std::make_tuple(1, 4);
    std::unordered_map<Utils::tuple_2i, struct cost_util, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        storage;
    storage[tup_2i] = custorage;
    ON_CALL(*event_dm, SelectCustomCallback(SQL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_CU(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person:: ::kHcv))
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, GetTrueFibrosisState())
        .WillOnce(Return(person::FibrosisState::kF3))
        .WillOnce(Return(person::FibrosisState::kF3))
        .WillRepeatedly(Return(person::FibrosisState::kF4));
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::kF4))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);
    EXPECT_CALL(*testPerson, SetUtility(custorage.util, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F4D) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost & Utility Setup
    cost_util custorage = {25.25, 0.8};
    Utils::tuple_2i tup_2i = std::make_tuple(1, 5);
    std::unordered_map<Utils::tuple_2i, struct cost_util, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        storage;
    storage[tup_2i] = custorage;
    ON_CALL(*event_dm, SelectCustomCallback(SQL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_CU(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person:: ::kHcv))
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, GetTrueFibrosisState())
        .WillOnce(Return(person::FibrosisState::kF4))
        .WillOnce(Return(person::FibrosisState::kF4))
        .WillRepeatedly(Return(person::FibrosisState::kDecomp));
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::kDecomp))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);
    EXPECT_CALL(*testPerson, SetUtility(custorage.util, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_DECOMP) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost & Utility Setup
    cost_util custorage = {25.25, 0.8};
    Utils::tuple_2i tup_2i = std::make_tuple(1, 5);
    std::unordered_map<Utils::tuple_2i, struct cost_util, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        storage;
    storage[tup_2i] = custorage;
    ON_CALL(*event_dm, SelectCustomCallback(SQL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_CU(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::kDecomp));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person:: ::kHcv))
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(_)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);
    EXPECT_CALL(*testPerson, SetUtility(custorage.util, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_PersonNotIdentified) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost & Utility Setup
    cost_util custorage = {25.25, 0.8};
    Utils::tuple_2i tup_2i = std::make_tuple(1, 1);
    std::unordered_map<Utils::tuple_2i, struct cost_util, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        storage;
    storage[tup_2i] = custorage;
    ON_CALL(*event_dm, SelectCustomCallback(SQL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_CU(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::kF1));
    ON_CALL(*testPerson, IsIdentifiedAsInfected(person:: ::kHcv))
        .WillByDefault(Return(false));

    // Expectations
    EXPECT_CALL(*testPerson, GetTrueFibrosisState())
        .WillOnce(Return(person::FibrosisState::kF0))
        .WillOnce(Return(person::FibrosisState::kF0))
        .WillRepeatedly(Return(person::FibrosisState::kF1));
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::kF1))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(*testPerson, SetUtility(custorage.util, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
