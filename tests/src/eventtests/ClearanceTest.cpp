#include "Clearance.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class ClearanceTest : public EventTest {};

TEST_F(ClearanceTest, Clearance) {
    std::string clearance_prob = std::string("1.0");
    ON_CALL(*event_dm, GetFromConfig("infection.clearance_prob", _))
        .WillByDefault(DoAll(SetArgReferee<1>(clearance_prob), Return(0)));

    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    EXPECT_CALL(*testPerson, ClearHCV()).Times(1);

    std::shared_ptr<event::Event> event = efactory.create("Clearance");
    event->Execute(testPerson, event_dm, decider);
}