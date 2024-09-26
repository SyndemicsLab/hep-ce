#include "Screening.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class ScreeningTest : public EventTest {};

TEST_F(ScreeningTest, FirstPeriodicScreening_FTTtestResults) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(0));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(-1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0))        // Decide to Intervention Screen
        .WillOnce(Return(1))        // AB Test is Negative
        .WillOnce(Return(0))        // Second AB Test is Positive
        .WillOnce(Return(0))        // RNA Test is Positive
        .WillRepeatedly(Return(0)); // Remainder of Test

    // Expectations
    EXPECT_CALL(*testPerson, MarkScreened()).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(2);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::INTERVENTION)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Screening");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(ScreeningTest, FirstPeriodicScreening_TTtestResults) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(0));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(-1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0))        // Decide to Intervention Screen
        .WillOnce(Return(0))        // AB Test is Positive
        .WillOnce(Return(0))        // RNA Test is Positive
        .WillRepeatedly(Return(0)); // Remainder of Test

    // Expectations
    EXPECT_CALL(*testPerson, MarkScreened()).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(1);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::INTERVENTION)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Screening");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(ScreeningTest, FirstPeriodicScreening_TFtestResults) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(0));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(-1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0))        // Decide to Intervention Screen
        .WillOnce(Return(0))        // AB Test is Positive
        .WillOnce(Return(1))        // RNA Test is Negative
        .WillRepeatedly(Return(0)); // Remainder of Test

    // Expectations
    EXPECT_CALL(*testPerson, MarkScreened()).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(1);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    EXPECT_CALL(*testPerson, Link(_)).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Screening");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(ScreeningTest, BackgroundScreening_TFtestResults) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(3));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0))        // Do Background Screening
        .WillOnce(Return(0))        // AB Test is Positive
        .WillOnce(Return(1))        // RNA Test is Negative
        .WillRepeatedly(Return(0)); // Remainder of Test

    // Expectations
    EXPECT_CALL(*testPerson, MarkScreened()).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(1);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    EXPECT_CALL(*testPerson, Link(_)).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Screening");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(ScreeningTest, BackgroundScreening_TTtestResults) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(3));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0))        // Do Background Screening
        .WillOnce(Return(0))        // AB Test is Positive
        .WillOnce(Return(0))        // RNA Test is Negative
        .WillRepeatedly(Return(0)); // Remainder of Test

    // Expectations
    EXPECT_CALL(*testPerson, MarkScreened()).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(1);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(1);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND)).Times(1);
    EXPECT_CALL(*testPerson, Unlink()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Screening");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(ScreeningTest, NoScreen) {
    // Person Setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening()).WillByDefault(Return(3));
    ON_CALL(*testPerson, GetTimeOfLastScreening()).WillByDefault(Return(1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(false));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.5"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("6"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillRepeatedly(Return(1)); // Do Not Background Screen

    // Expectations
    EXPECT_CALL(*testPerson, MarkScreened()).Times(0);
    EXPECT_CALL(*testPerson, AddAbScreen()).Times(0);
    EXPECT_CALL(*testPerson, AddRnaScreen()).Times(0);
    EXPECT_CALL(*testPerson, Link(_)).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Screening");
    event->Execute(testPerson, event_dm, decider);
}