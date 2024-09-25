#include "Infections.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class InfectionsTest : public EventTest {};

TEST_F(InfectionsTest, Infections) {

    std::shared_ptr<event::Event> event = efactory.create("Infections");

    std::vector<double> storage = {1.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(DoAll(SetArg2ToCallbackValue(&storage), Return(0)));

    testPerson->SetHCV(person::HCV::NONE);
    event->Execute(testPerson, event_dm, decider);
    EXPECT_EQ(person::HCV::ACUTE, testPerson->GetHCV());
}