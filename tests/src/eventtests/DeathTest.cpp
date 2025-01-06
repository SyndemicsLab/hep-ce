#include "Death.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class DeathTest : public EventTest {};

std::string const OD_QUERY =
    "SELECT moud, drug_behavior, fatality_probability FROM "
    "overdoses;";

TEST_F(DeathTest, Death_OldAge) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(1200));

    // Data Setup
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Mortality Setup
    struct background_smr background = {0.0, 0.0};
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 4);
    std::unordered_map<Utils::tuple_3i, struct background_smr,
                       Utils::key_hash_3i, Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = background;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_BS(&tstorage), Return(0)));

    // Overdose Setup
    double overdose_mortality = 0.0;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        ostorage;
    ostorage[tup_2i] = overdose_mortality;
    ON_CALL(*event_dm, SelectCustomCallback(OD_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&ostorage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, Die(person::DeathReason::AGE)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Liver_F4_Infected) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F4));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::CHRONIC));

    // Data Setup
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Mortality Setup
    struct background_smr background = {0.0, 0.0};
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 4);
    std::unordered_map<Utils::tuple_3i, struct background_smr,
                       Utils::key_hash_3i, Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = background;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_BS(&tstorage), Return(0)));

    // Overdose Setup
    double overdose_mortality = 0.0;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        ostorage;
    ostorage[tup_2i] = overdose_mortality;
    ON_CALL(*event_dm, SelectCustomCallback(OD_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&ostorage), Return(0)));

    // Expectations
    std::vector<double> probs = {0.0, 1.0, 0.0};
    EXPECT_CALL(*decider, GetDecision(probs)).WillOnce(Return(1));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::LIVER)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Liver_F4_Uninfected) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F4));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::NONE));

    // Data Setup
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Mortality Setup
    struct background_smr background = {0.0, 0.0};
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 4);
    std::unordered_map<Utils::tuple_3i, struct background_smr,
                       Utils::key_hash_3i, Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = background;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_BS(&tstorage), Return(0)));

    // Overdose Setup
    double overdose_mortality = 0.0;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        ostorage;
    ostorage[tup_2i] = overdose_mortality;
    ON_CALL(*event_dm, SelectCustomCallback(OD_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&ostorage), Return(0)));

    // Expectations
    std::vector<double> probs = {0.0, 1.0, 0.0};
    EXPECT_CALL(*decider, GetDecision(probs)).WillOnce(Return(1));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::LIVER)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Liver_DECOMP_Infected) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::DECOMP));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::CHRONIC));

    // Data Setup
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Mortality Setup
    struct background_smr background = {0.0, 0.0};
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 4);
    std::unordered_map<Utils::tuple_3i, struct background_smr,
                       Utils::key_hash_3i, Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = background;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_BS(&tstorage), Return(0)));

    // Overdose Setup
    double overdose_mortality = 0.0;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        ostorage;
    ostorage[tup_2i] = overdose_mortality;
    ON_CALL(*event_dm, SelectCustomCallback(OD_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&ostorage), Return(0)));

    // Expectations
    std::vector<double> probs = {0.0, 1.0, 0.0};
    EXPECT_CALL(*decider, GetDecision(probs)).WillOnce(Return(1));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::LIVER)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Liver_DECOMP_Uninfected) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::DECOMP));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::NONE));

    // Data Setup
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("1.0"), Return(0)));

    // Background Mortality Setup
    struct background_smr background = {0.0, 0.0};
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 4);
    std::unordered_map<Utils::tuple_3i, struct background_smr,
                       Utils::key_hash_3i, Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = background;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_BS(&tstorage), Return(0)));

    // Overdose Setup
    double overdose_mortality = 0.0;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        ostorage;
    ostorage[tup_2i] = overdose_mortality;
    ON_CALL(*event_dm, SelectCustomCallback(OD_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&ostorage), Return(0)));

    // Expectations
    std::vector<double> probs = {0.0, 1.0, 0.0};
    EXPECT_CALL(*decider, GetDecision(probs)).WillOnce(Return(1));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::LIVER)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Background) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F0));

    // Data Setup
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Mortality Setup
    struct background_smr background = {1.0, 0.0};
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 4);
    std::unordered_map<Utils::tuple_3i, struct background_smr,
                       Utils::key_hash_3i, Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = background;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_BS(&tstorage), Return(0)));

    // Overdose Setup
    double overdose_mortality = 0.0;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        ostorage;
    ostorage[tup_2i] = overdose_mortality;
    ON_CALL(*event_dm, SelectCustomCallback(OD_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&ostorage), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::BACKGROUND)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_NoDeath) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F0));

    // Data Setup
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_infected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp_uninfected", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Mortality Setup
    struct background_smr background = {0.0, 0.0};
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 4);
    std::unordered_map<Utils::tuple_3i, struct background_smr,
                       Utils::key_hash_3i, Utils::key_equal_3i>
        tstorage;
    tstorage[tup_3i] = background;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_BS(&tstorage), Return(0)));

    // Overdose Setup
    double overdose_mortality = 0.0;
    Utils::tuple_2i tup_2i = std::make_tuple(0, 4);
    std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                       Utils::key_equal_2i>
        ostorage;
    ostorage[tup_2i] = overdose_mortality;
    ON_CALL(*event_dm, SelectCustomCallback(OD_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T2I_Double(&ostorage), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_)).WillOnce(Return(2));
    EXPECT_CALL(*testPerson, Die(_)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}