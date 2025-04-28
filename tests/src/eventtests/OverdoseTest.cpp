////////////////////////////////////////////////////////////////////////////////
// File: OverdoseTest.cpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Overdose.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class OverdoseTest : public EventTest {};

TEST_F(OverdoseTest, FormerUsers) {
    // Person Setup
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kFormerInjection));

    // Expectations
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _)).Times(0);
    EXPECT_CALL(*testPerson, ToggleOverdose()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Overdose", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(OverdoseTest, NeverUsers) {
    // Person Setup
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kNever));

    // Expectations
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _)).Times(0);
    EXPECT_CALL(*testPerson, ToggleOverdose()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Overdose", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(OverdoseTest, Injection_No_Overdose) {
    // Person Setup
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kInjection));

    // Data Setup
    std::vector<double> prob = {0.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&prob), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1));

    // Expectations
    EXPECT_CALL(*testPerson, ToggleOverdose()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Overdose", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(OverdoseTest, Injection_Overdose) {
    // Person Setup
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::kInjection));

    // Data Setup
    std::vector<double> prob = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&prob), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, ToggleOverdose()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Overdose", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
