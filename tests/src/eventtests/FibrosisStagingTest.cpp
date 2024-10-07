#include "FibrosisStaging.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class FibrosisStagingTest : public EventTest {};

TEST_F(FibrosisStagingTest, FibrosisStaging_NoFibrosis) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::NONE));
    ON_CALL(*event_dm, GetFromConfig(_, _)).WillByDefault(Return(0));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one", _))
        .WillByDefault(DoAll(SetArgReferee<1>("test_one"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two", _))
        .WillByDefault(DoAll(SetArgReferee<1>("test_two"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.multitest_result_method", _))
        .WillByDefault(DoAll(SetArgReferee<1>("maximum"), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, DiagnoseFibrosis(_)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisStagingTest, FibrosisStaging_RecentlyStaged) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F0));
    ON_CALL(*testPerson, GetTimeSinceFibrosisStaging())
        .WillByDefault(Return(2));
    ON_CALL(*testPerson, GetTimeOfFibrosisStaging()).WillByDefault(Return(5));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one", _))
        .WillByDefault(DoAll(SetArgReferee<1>("test_one"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two", _))
        .WillByDefault(DoAll(SetArgReferee<1>("test_two"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.multitest_result_method", _))
        .WillByDefault(DoAll(SetArgReferee<1>("maximum"), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, DiagnoseFibrosis(_)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisStagingTest, FibrosisStaging_FirstStagingNoTestTwo) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F0));
    ON_CALL(*testPerson, GetTimeSinceFibrosisStaging())
        .WillByDefault(Return(2));
    ON_CALL(*testPerson, GetTimeOfFibrosisStaging()).WillByDefault(Return(-1));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.multitest_result_method", _))
        .WillByDefault(DoAll(SetArgReferee<1>("maximum"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one", _))
        .WillByDefault(DoAll(SetArgReferee<1>("colname"), Return(0)));
    std::vector<double> storage = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("15.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("30.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two", _))
        .WillByDefault(DoAll(SetArgReferee<1>(""), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0)); // F0

    // Expectations
    EXPECT_CALL(*testPerson,
                DiagnoseFibrosis(person::MeasuredFibrosisState::F01))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisStagingTest, FibrosisStaging_FirstStagingTestTwoMaxChoice) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F0));
    ON_CALL(*testPerson, GetTimeSinceFibrosisStaging())
        .WillByDefault(Return(2));
    ON_CALL(*testPerson, GetTimeOfFibrosisStaging()).WillByDefault(Return(-1));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one", _))
        .WillByDefault(DoAll(SetArgReferee<1>("colname"), Return(0)));
    std::vector<double> storage = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("15.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("30.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two", _))
        .WillByDefault(DoAll(SetArgReferee<1>("colname"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.multitest_result_method", _))
        .WillByDefault(DoAll(SetArgReferee<1>("maximum"), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // F1
        .WillOnce(Return(0)); // F0
    EXPECT_CALL(*testPerson, GiveSecondScreeningTest(true)).Times(1);
    EXPECT_CALL(*testPerson,
                DiagnoseFibrosis(person::MeasuredFibrosisState::F23))
        .Times(2);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(2);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisStagingTest, FibrosisStaging_FirstStagingTestTwoLatestChoice) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::F0));
    ON_CALL(*testPerson, GetTimeSinceFibrosisStaging())
        .WillByDefault(Return(2));
    ON_CALL(*testPerson, GetTimeOfFibrosisStaging()).WillByDefault(Return(-1));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one", _))
        .WillByDefault(DoAll(SetArgReferee<1>("colname"), Return(0)));
    std::vector<double> storage = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("15.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("30.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two", _))
        .WillByDefault(DoAll(SetArgReferee<1>("colname"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.multitest_result_method", _))
        .WillByDefault(DoAll(SetArgReferee<1>("latest"), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // F1
        .WillOnce(Return(0)); // F0
    EXPECT_CALL(*testPerson, GiveSecondScreeningTest(true)).Times(1);
    EXPECT_CALL(*testPerson,
                DiagnoseFibrosis(person::MeasuredFibrosisState::F23))
        .Times(1);
    EXPECT_CALL(*testPerson,
                DiagnoseFibrosis(person::MeasuredFibrosisState::F01))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_)).Times(2);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}