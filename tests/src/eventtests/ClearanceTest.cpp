#include "Clearance.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class ClearanceTest : public EventTest {};

TEST_F(ClearanceTest, Clearance) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("infection.clearance_prob", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, ClearHCV()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Clearance", event_dm);
    event->Execute(testPerson, event_dm, decider);
}