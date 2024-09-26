#include "Treatment.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class TreatmentTest : public EventTest {};

TEST_F(TreatmentTest, NewTreatmentInitiation) {
    // Person Setup
    ON_CALL(*testPerson, HasInitiatedTreatment()).WillByDefault(Return(false));
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(-1));

    // Data Setup
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))        // Is Not Lost to Follow Up
        .WillOnce(Return(0))        // Initiate Treatment
        .WillOnce(Return(1))        // Do Not Withdraw
        .WillOnce(Return(1))        // Do Not Experience Toxicity
        .WillOnce(Return(0))        // Achieve SVR
        .WillRepeatedly(Return(0)); // The Rest

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_)).Times(2);
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(1);
    EXPECT_CALL(*testPerson, InitiateTreatment()).Times(1);
    EXPECT_CALL(*testPerson, AddSVR()).Times(1);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(1);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Treatment");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(TreatmentTest, FinishTreatment) {
    // Person Setup
    ON_CALL(*testPerson, HasInitiatedTreatment()).WillByDefault(Return(true));
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(5));

    // Data Setup
    std::vector<double> storage = {1.0};
    struct cost_svr_select cost = {5, 0.5};
    std::vector<struct cost_svr_select> cstorage = {cost};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(_, _, &cstorage, _))
        .WillByDefault(
            DoAll(SetArg2ToCostSVRCallbackValue(&cstorage), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))        // Do Not Withdraw
        .WillOnce(Return(1))        // Do Not Experience Toxicity
        .WillOnce(Return(0))        // Achieve SVR
        .WillRepeatedly(Return(0)); // The Rest

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_)).Times(2);
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(2);
    EXPECT_CALL(*testPerson, AddSVR()).Times(1);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(1);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(1);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Treatment");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(TreatmentTest, LostToFollowUp) {
    // Person Setup
    ON_CALL(*testPerson, HasInitiatedTreatment()).WillByDefault(Return(false));
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(0));

    // Data Setup
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(0)); // Lost To Follow-Up

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_)).Times(0);
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(1);
    EXPECT_CALL(*testPerson, AddSVR()).Times(0);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(0);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Treatment");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(TreatmentTest, Withdraw) {
    // Person Setup
    ON_CALL(*testPerson, HasInitiatedTreatment()).WillByDefault(Return(true));
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(5));

    // Data Setup
    std::vector<double> storage = {1.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_)).WillOnce(Return(0)); // Withdraw

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_)).Times(2); // Cost of Visit and Course
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(2); // Cost of Visit and Quit
    EXPECT_CALL(*testPerson, AddSVR()).Times(0);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(0);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);
    EXPECT_CALL(*testPerson, AddWithdrawal()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Treatment");
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(TreatmentTest, DevelopToxicity) {
    // Person Setup
    ON_CALL(*testPerson, HasInitiatedTreatment()).WillByDefault(Return(true));
    ON_CALL(*testPerson, GetTimeSinceTreatmentInitiation())
        .WillByDefault(Return(5));

    // Data Setup
    std::vector<double> storage = {1.0};
    struct cost_svr_select cost = {5, 0.5};
    std::vector<struct cost_svr_select> cstorage = {cost};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(_, _, &cstorage, _))
        .WillByDefault(
            DoAll(SetArg2ToCostSVRCallbackValue(&cstorage), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("10.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_initialization", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.treatment_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.ltfu_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("treatment.tox_utility", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // Do Not Withdraw
        .WillOnce(Return(0)); // Experience Toxicity

    // Expectations
    EXPECT_CALL(*testPerson, AddCost(_)).Times(3);
    EXPECT_CALL(*testPerson, SetUtility(_)).Times(3); // Visit, Quit, Toxicity
    EXPECT_CALL(*testPerson, AddSVR()).Times(0);
    EXPECT_CALL(*testPerson, ClearHCV()).Times(0);
    EXPECT_CALL(*testPerson, AddCompletedTreatment()).Times(0);
    EXPECT_CALL(*testPerson, Unlink()).Times(1);
    EXPECT_CALL(*testPerson, AddToxicReaction()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event = efactory.create("Treatment");
    event->Execute(testPerson, event_dm, decider);
}