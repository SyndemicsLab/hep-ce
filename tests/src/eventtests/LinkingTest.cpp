#include "Linking.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class LinkingTest : public EventTest {};

std::string const BACKGROUND_LINK_QUERY =
    "SELECT age_years, gender, drug_behavior, background_link_probability FROM "
    "screening_and_linkage;";

std::string const INTERVENTION_LINK_QUERY =
    "SELECT age_years, gender, drug_behavior, "
    "intervention_link_probability FROM "
    "screening_and_linkage;";

TEST_F(LinkingTest, FalsePositive) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::NONE));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 0.5;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, Unlink()).Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(LinkingTest, BackgroundRelink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::UNLINKED));
    ON_CALL(*testPerson, GetLinkageType())
        .WillByDefault(Return(person::LinkageType::BACKGROUND));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 1.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.0;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(LinkingTest, BackgroundFirstLink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::NEVER));
    ON_CALL(*testPerson, GetLinkageType())
        .WillByDefault(Return(person::LinkageType::BACKGROUND));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 1.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.5;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, Link(person::LinkageType::BACKGROUND)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(LinkingTest, InterventionLink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::NEVER));
    ON_CALL(*testPerson, GetLinkageType())
        .WillByDefault(Return(person::LinkageType::INTERVENTION));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("linking.intervention_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 1.0;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_, _)).Times(1);
    EXPECT_CALL(*testPerson, Link(person::LinkageType::INTERVENTION)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(LinkingTest, DecideToUnlink) {
    // Person Setup
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, IsIdentifiedAsHCVInfected())
        .WillByDefault(Return(true));
    ON_CALL(*testPerson, GetLinkState())
        .WillByDefault(Return(person::LinkageState::LINKED));
    ON_CALL(*testPerson, GetLinkageType())
        .WillByDefault(Return(person::LinkageType::INTERVENTION));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig(_, _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Background Link Setup
    double link_prob = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bstorage;
    bstorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BACKGROUND_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bstorage), Return(0)));

    // Intervention Link Setup
    link_prob = 0.0;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = link_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTERVENTION_LINK_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1));

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_, _)).Times(0);
    EXPECT_CALL(*testPerson, Link(_)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Linking", event_dm);
    event->Execute(testPerson, event_dm, decider);
}