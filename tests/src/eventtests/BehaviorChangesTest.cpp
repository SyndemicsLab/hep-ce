////////////////////////////////////////////////////////////////////////////////
// File: BehaviorChangesTest.cpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "BehaviorChanges.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class BehaviorChangesTest : public EventTest {};

std::string const COST_QUERY =
    "SELECT gender, drug_behavior, cost, utility FROM "
    "behavior_impacts;";

TEST_F(BehaviorChangesTest, BehaviorChanges) {
    // Person Setup
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::INJECTION));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetMoudState())
        .WillByDefault(Return(person::MOUD::NONE));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Transitions Setup
    struct behavior_transitions trans = {0.0, 0.0, 1.0, 0.0, 0.0};
    Utils::tuple_4i tup_4i = std::make_tuple(25, 0, 0, 4);
    std::unordered_map<Utils::tuple_4i, struct behavior_transitions,
                       Utils::key_hash_4i, Utils::key_equal_4i>
        tstorage;
    tstorage[tup_4i] = trans;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T4I_BT(&tstorage), Return(0)));

    // Cost Setup
    struct cost_util cost = {25.00, 0.5};
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, struct cost_util, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        cstorage;
    cstorage[tup_2i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_CU(&cstorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(2));

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);
    EXPECT_CALL(*testPerson, SetUtility(_, _)).Times(1);
    EXPECT_CALL(*testPerson, SetBehavior(person::Behavior::FORMER_INJECTION))
        .Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("BehaviorChanges", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
