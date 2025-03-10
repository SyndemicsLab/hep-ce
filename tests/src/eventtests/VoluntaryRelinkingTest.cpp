////////////////////////////////////////////////////////////////////////////////
// File: VoluntaryRelinkingTest.cpp                                           //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "VoluntaryRelinking.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class VoluntaryRelinkingTest : public EventTest {};

TEST_F(VoluntaryRelinkingTest, Relink) {
    // Person Setup
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetTimeSinceLinkChange()).WillByDefault(Return(5));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::CHRONIC));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("linking.voluntary_relinkage_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.voluntary_relink_duration", _))
        .WillByDefault(DoAll(SetArgReferee<1>("100"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("100"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening_background_rna.cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("25.25"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("VoluntaryRelinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(VoluntaryRelinkingTest, NotUnlinked) {
    // Person Setup
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::LINKED));
    ON_CALL(*testPerson, GetTimeSinceLinkChange()).WillByDefault(Return(5));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("linking.voluntary_relinkage_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.voluntary_relink_duration", _))
        .WillByDefault(DoAll(SetArgReferee<1>("100"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("100"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening_background_rna.cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("25.25"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("VoluntaryRelinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(VoluntaryRelinkingTest, TooLongDuration) {
    // Person Setup
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetTimeSinceLinkChange()).WillByDefault(Return(15));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("linking.voluntary_relinkage_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.voluntary_relink_duration", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("100"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening_background_rna.cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("25.25"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("VoluntaryRelinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(VoluntaryRelinkingTest, DecideNotToRelink) {
    // Person Setup
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetTimeSinceLinkChange()).WillByDefault(Return(5));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("linking.voluntary_relinkage_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.voluntary_relink_duration", _))
        .WillByDefault(DoAll(SetArgReferee<1>("100"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("100"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening_background_rna.cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("25.25"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1));

    // Expectations
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("VoluntaryRelinking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
