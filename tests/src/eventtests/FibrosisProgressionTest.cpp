#include "FibrosisProgression.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class FibrosisProgressionTest : public EventTest {};

TEST_F(FibrosisProgressionTest, FibrosisProgression) {
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression");

    std::string add_cost = std::string("false");

    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*event_dm,
                GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(add_cost), Return(0)));

    std::string fib_1 = std::string("1.0");

    EXPECT_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(fib_1), Return(0)));

    testPerson->SetHCV(person::HCV::CHRONIC);
    testPerson->UpdateTrueFibrosis(person::FibrosisState::F0);
    event->Execute(testPerson, event_dm, decider);
    EXPECT_EQ(person::FibrosisState::F1, testPerson->GetTrueFibrosisState());
}