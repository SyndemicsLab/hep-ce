#include "Death.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class DeathTest : public EventTest {};

TEST_F(DeathTest, Death_OldAge) {
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(1200));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::AGE)).Times(1);
    std::shared_ptr<event::Event> event = efactory.create("Death");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Liver_F4) {
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F4));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4", _))
        .WillOnce(DoAll(SetArgReferee<1>("1.0"), Return(0)));

    struct background_smr trans = {0.0, 0.0};
    std::vector<struct background_smr> tstorage = {trans};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToBackgroundSMRMortalityCallbackValue(&tstorage),
                  Return(0)));

    std::vector<double> probs = {0.0, 1.0, 0.0};
    EXPECT_CALL(*decider, GetDecision(probs)).WillOnce(Return(1));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::LIVER)).Times(1);

    std::shared_ptr<event::Event> event = efactory.create("Death");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Liver_DECOMP) {
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::DECOMP));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.decomp", _))
        .WillOnce(DoAll(SetArgReferee<1>("1.0"), Return(0)));

    struct background_smr trans = {0.0, 0.0};
    std::vector<struct background_smr> tstorage = {trans};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToBackgroundSMRMortalityCallbackValue(&tstorage),
                  Return(0)));

    std::vector<double> probs = {0.0, 1.0, 0.0};
    EXPECT_CALL(*decider, GetDecision(probs)).WillOnce(Return(1));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::LIVER)).Times(1);

    std::shared_ptr<event::Event> event = efactory.create("Death");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_Background) {
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F0));

    struct background_smr trans = {1.0, 0.0};
    std::vector<struct background_smr> tstorage = {trans};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToBackgroundSMRMortalityCallbackValue(&tstorage),
                  Return(0)));

    EXPECT_CALL(*decider, GetDecision(_)).WillOnce(Return(0));
    EXPECT_CALL(*testPerson, Die(person::DeathReason::BACKGROUND)).Times(1);

    std::shared_ptr<event::Event> event = efactory.create("Death");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(DeathTest, Death_NoDeath) {
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F0));

    struct background_smr trans = {0.0, 0.0};
    std::vector<struct background_smr> tstorage = {trans};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToBackgroundSMRMortalityCallbackValue(&tstorage),
                  Return(0)));

    EXPECT_CALL(*decider, GetDecision(_)).WillOnce(Return(2));
    EXPECT_CALL(*testPerson, Die(_)).Times(0);

    std::shared_ptr<event::Event> event = efactory.create("Death");
    event->Execute(testPerson, event_dm, decider);
}