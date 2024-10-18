#include "Infections.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class InfectionsTest : public EventTest {};

std::string const INCIDENCE_QUERY =
    "SELECT age_years, gender, drug_behavior, incidence FROM "
    "incidence;";

TEST_F(InfectionsTest, Infections_NewInfection) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::NONE));

    // Data Setup
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));

    // Incidence Setup
    double incidence = 1.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0)); // Infect

    // Expectations
    EXPECT_CALL(*testPerson, InfectHCV()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Infections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(InfectionsTest, Infections_DoNotInfect) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::NONE));

    // Data Setup
    // Incidence Setup
    double incidence = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1)); // Do Not Infect

    // Expectations
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Infections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(InfectionsTest, Infections_AcuteProgression) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTimeSinceHCVChanged()).WillByDefault(Return(6));

    // Incidence Setup
    double incidence = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, SetHCV(person::HCV::CHRONIC)).Times(1);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Infections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(InfectionsTest, Infections_NoAcuteProgression) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::ACUTE));
    ON_CALL(*testPerson, GetTimeSinceHCVChanged()).WillByDefault(Return(5));

    // Incidence Setup
    double incidence = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1)); // Do Not Infect

    // Expectations
    EXPECT_CALL(*testPerson, SetHCV(_)).Times(0);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Infections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(InfectionsTest, Infections_HandleChronicHCV) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::CHRONIC));
    ON_CALL(*testPerson, GetTimeSinceHCVChanged()).WillByDefault(Return(5));

    // Incidence Setup
    double incidence = 0.0;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        istorage;
    istorage[tup_3i] = incidence;
    ON_CALL(*event_dm, SelectCustomCallback(INCIDENCE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&istorage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, SetHCV(_)).Times(0);
    EXPECT_CALL(*testPerson, InfectHCV()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Infections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}