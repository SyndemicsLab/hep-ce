#include "Screening.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class ScreeningTest : public EventTest {};

TEST_F(ScreeningTest, Screening) {

    std::shared_ptr<event::Event> event = efactory.create("Screening");

    std::vector<double> storage = {1.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    std::string specificity = std::string("1.0");
    EXPECT_CALL(*event_dm, GetFromConfig(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(specificity), Return(0)));

    std::string intervention = std::string("periodic");
    EXPECT_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(intervention), Return(0)));

    std::string period = std::string("6");
    EXPECT_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(period), Return(0)));

    event->Execute(testPerson, event_dm, decider);
    EXPECT_EQ(0, testPerson->GetTimeSinceLastScreening());
}