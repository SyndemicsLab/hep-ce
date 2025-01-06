#include "FibrosisProgression.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class FibrosisProgressionTest : public EventTest {};

std::string const COST_QUERY =
    "SELECT hcv_status, fibrosis_state, cost FROM hcv_impacts;";

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
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost Setup
    double cost = 25.25;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        cstorage;
    cstorage[tup_2i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&cstorage), Return(0)));

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
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost Setup
    double cost = 25.25;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        cstorage;
    cstorage[tup_2i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&cstorage), Return(0)));

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
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F12) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
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
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost Setup
    double cost = 25.25;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        cstorage;
    cstorage[tup_2i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&cstorage), Return(0)));

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
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F23) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
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
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost Setup
    double cost = 25.25;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        cstorage;
    cstorage[tup_2i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&cstorage), Return(0)));

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
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F34) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
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
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost Setup
    double cost = 25.25;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        cstorage;
    cstorage[tup_2i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&cstorage), Return(0)));

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
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_F4D) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
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
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost Setup
    double cost = 25.25;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        cstorage;
    cstorage[tup_2i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&cstorage), Return(0)));

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
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisProgressionTest, FibrosisProgression_DECOMP) {
    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillByDefault(DoAll(SetArgReferee<1>("true"), Return(0)));
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
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Cost Setup
    double cost = 25.25;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        cstorage;
    cstorage[tup_2i] = cost;
    ON_CALL(*event_dm, SelectCustomCallback(COST_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&cstorage), Return(0)));

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
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression", event_dm);
    event->Execute(testPerson, event_dm, decider);
}