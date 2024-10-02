#include "MOUD.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class MOUDTest : public EventTest {};

TEST_F(MOUDTest, NeverUsed) {
    // Person Setup
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Expectations
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _)).Times(0);
    EXPECT_CALL(*testPerson, TransitionMOUD()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("MOUD");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(MOUDTest, No_Transition) {
    // Person Setup
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::INJECTION));

    // Data Setup
    std::vector<double> prob = {0.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&prob), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1));

    // Expectations
    EXPECT_CALL(*testPerson, TransitionMOUD()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("MOUD");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(MOUDTest, Transition) {
    // Person Setup
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::INJECTION));

    // Data Setup
    std::vector<double> prob = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&prob), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, TransitionMOUD()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("MOUD");
    event->Execute(testPerson, event_dm, decider);
}