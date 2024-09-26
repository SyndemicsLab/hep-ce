#include "Infections.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class InfectionsTest : public EventTest {};

TEST_F(InfectionsTest, Infections_NewInfection) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::NONE));

    // Data Setup
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0)); // Infect

    // Expectations
    EXPECT_CALL(*testPerson, InfectHCV()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Infections");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(InfectionsTest, Infections_DoNotInfect) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::NONE));

    // Data Setup
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1)); // Do Not Infect

    // Expectations
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Infections");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(InfectionsTest, Infections_AcuteProgression) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTimeSinceHCVChanged()).WillByDefault(Return(6));

    // Expectations
    EXPECT_CALL(*testPerson, SetHCV(person::HCV::CHRONIC)).Times(1);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Infections");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(InfectionsTest, Infections_NoAcuteProgression) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTimeSinceHCVChanged()).WillByDefault(Return(5));

    // Expectations
    EXPECT_CALL(*testPerson, SetHCV(_)).Times(0);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Infections");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(InfectionsTest, Infections_HandleChronicHCV) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::CHRONIC));
    ON_CALL(*testPerson, GetTimeSinceHCVChanged()).WillByDefault(Return(5));

    // Expectations
    EXPECT_CALL(*testPerson, SetHCV(_)).Times(0);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Infections");
    event->Execute(testPerson, event_dm, decider);
}