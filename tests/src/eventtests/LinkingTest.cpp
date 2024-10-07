#include "Linking.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class LinkingTest : public EventTest {};

TEST_F(LinkingTest, FalsePositive) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::NONE));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, Unlink()).Times(1);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(LinkingTest, BackgroundRelink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetLinkageType())
        .WillByDefault(Return(person::LinkageType::BACKGROUND));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(LinkingTest, BackgroundFirstLink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::NEVER));
    ON_CALL(*testPerson, GetLinkageType())
        .WillByDefault(Return(person::LinkageType::BACKGROUND));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(LinkingTest, InterventionLink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::NEVER));
    ON_CALL(*testPerson, GetLinkageType())
        .WillByDefault(Return(person::LinkageType::INTERVENTION));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_)).Times(1);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::INTERVENTION)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(LinkingTest, DecideToUnlink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::LINKED));
    ON_CALL(*testPerson, GetLinkageType())
        .WillByDefault(Return(person::LinkageType::INTERVENTION));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    std::vector<double> storage = {0.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1));

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_)).Times(0);
    EXPECT_CALL(*testPerson, Link(_)).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}