#include "Death.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class DeathTest : public EventTest {};

TEST_F(DeathTest, Death_OldAge) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(1200));

    // Data Setup
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, Die(person::DeathReason::AGE)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Liver_F4) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F4));

    // Data Setup
    struct background_smr trans = {0.0, 0.0};
    std::vector<struct background_smr> tstorage = {trans};
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4", _))
        .WillOnce(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToBackgroundSMRMortalityCallbackValue(&tstorage),
                  Return(0)));

    // Expectations
    std::vector<double> probs = {0.0, 1.0, 0.0};
    EXPECT_CALL(*decider, GetDecision(probs)).WillOnce(Return(1));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::LIVER)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Liver_DECOMP) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::DECOMP));

    // Data Setup
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    struct background_smr trans = {0.0, 0.0};
    std::vector<struct background_smr> tstorage = {trans};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToBackgroundSMRMortalityCallbackValue(&tstorage),
                  Return(0)));

    // Expectations
    std::vector<double> probs = {0.0, 1.0, 0.0};
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp", _))
        .WillOnce(DoAll(SetArgReferee<1>("1.0"), Return(0)));
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
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    struct background_smr trans = {1.0, 0.0};
    std::vector<struct background_smr> tstorage = {trans};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToBackgroundSMRMortalityCallbackValue(&tstorage),
                  Return(0)));

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
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp", _))
        .WillOnce(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    struct background_smr trans = {0.0, 0.0};
    std::vector<struct background_smr> tstorage = {trans};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToBackgroundSMRMortalityCallbackValue(&tstorage),
                  Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_)).WillOnce(Return(2));
    EXPECT_CALL(*testPerson, Die(_)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Death", event_dm);
    event->Execute(testPerson, event_dm, decider);
}