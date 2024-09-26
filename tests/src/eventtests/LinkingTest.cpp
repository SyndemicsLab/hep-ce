#include "Linking.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class LinkingTest : public EventTest {};

TEST_F(LinkingTest, Linking) {
    std::shared_ptr<event::Event> event = efactory.create("Linking");

    std::vector<double> storage = {1.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    std::string cost = std::string("20.25");
    EXPECT_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(cost), Return(0)));

    std::string relink = std::string("1.0");
    EXPECT_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(relink), Return(0)));

    testPerson->SetHCV(person::HCV::ACUTE);
    int numLinks = testPerson->GetLinkCount();
    event->Execute(testPerson, event_dm, decider);
    EXPECT_EQ(person::LinkageState::LINKED, testPerson->GetLinkState());
    EXPECT_EQ(numLinks + 1, testPerson->GetLinkCount());
}