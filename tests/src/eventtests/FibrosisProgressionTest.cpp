#include "FibrosisProgression.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class FibrosisProgressionTest : public EventTest {};

TEST_F(FibrosisProgressionTest, FibrosisProgression_NoHCV) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(Return(0));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, GetHCV()).WillOnce(Return(person::HCV::NONE));
    EXPECT_CALL(*testPerson, GetTrueFibrosisState()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F01) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    std::vector<double> costs = {25.25};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&costs), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F0));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::F1))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F12) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    std::vector<double> costs = {25.25};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&costs), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F1));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::F2))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F23) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    std::vector<double> costs = {25.25};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&costs), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F2));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::F3))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F34) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    std::vector<double> costs = {25.25};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&costs), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F3));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::F4))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F4D) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    std::vector<double> costs = {25.25};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&costs), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F4));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(person::FibrosisState::DECOMP))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_DECOMP) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
    std::vector<double> costs = {25.25};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToDoubleCallbackValue(&costs), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f12", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f23", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f34", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis.f4d", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::DECOMP));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));

    // Expectations
    EXPECT_CALL(*testPerson, UpdateTrueFibrosis(_)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}