////////////////////////////////////////////////////////////////////////////////
// File: FibrosisStagingTest.cpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "FibrosisStaging.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class FibrosisStagingTest : public EventTest {};

std::string const TEST_ONE_QUERY =
    "SELECT fibrosis_state, test_one FROM fibrosis;";

std::string const TEST_TWO_QUERY =
    "SELECT fibrosis_state, test_two FROM fibrosis;";

TEST_F(FibrosisStagingTest, FibrosisStaging_NoFibrosis) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::kNone));
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
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.test_two_eligible_stages", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>("f0, f1, f2, f3, f4, decomp"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Test One Data Setup
    std::unordered_map<int, std::vector<double>> t1storage;
    t1storage[6] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_ONE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t1storage), Return(0)));

    // Test Two Data Setup
    std::unordered_map<int, std::vector<double>> t2storage;
    t2storage[6] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_TWO_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t2storage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, DiagnoseFibrosis(_)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisStagingTest, FibrosisStaging_RecentlyStaged) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::kF0));
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
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.test_two_eligible_stages", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>("f0, f1, f2, f3, f4, decomp"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Test One Data Setup
    std::unordered_map<int, std::vector<double>> t1storage;
    t1storage[0] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_ONE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t1storage), Return(0)));

    // Test Two Data Setup
    std::unordered_map<int, std::vector<double>> t2storage;
    t2storage[0] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_TWO_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t2storage), Return(0)));

    // Expectations
    EXPECT_CALL(*testPerson, DiagnoseFibrosis(_)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisStagingTest, FibrosisStaging_FirstStagingNoTestTwo) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::kF0));
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
        .WillByDefault(DoAll(SetArgReferee<1>("test_one"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("15.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("30.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two", _))
        .WillByDefault(DoAll(SetArgReferee<1>(""), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.test_two_eligible_stages", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>("f0, f1, f2, f3, f4, decomp"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Test One Data Setup
    std::unordered_map<int, std::vector<double>> t1storage;
    t1storage[0] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_ONE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t1storage), Return(0)));

    // Test Two Data Setup
    std::unordered_map<int, std::vector<double>> t2storage;
    t2storage[0] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_TWO_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t2storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0)); // F0

    // Expectations
    EXPECT_CALL(*testPerson,
                DiagnoseFibrosis(person::MeasuredFibrosisState::kF01))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisStagingTest, FibrosisStaging_FirstStagingTestTwoMaxChoice) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::kF0));
    ON_CALL(*testPerson, GetTimeSinceFibrosisStaging())
        .WillByDefault(Return(13));
    ON_CALL(*testPerson, GetTimeOfFibrosisStaging()).WillByDefault(Return(-1));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one", _))
        .WillByDefault(DoAll(SetArgReferee<1>("test_one"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("15.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("30.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two", _))
        .WillByDefault(DoAll(SetArgReferee<1>("test_two"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.multitest_result_method", _))
        .WillByDefault(DoAll(SetArgReferee<1>("maximum"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.test_two_eligible_stages", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>("f0, f1, f2, f3, f4, decomp"), Return(0)));

    // Test One Data Setup
    std::unordered_map<int, std::vector<double>> t1storage;
    t1storage[0] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_ONE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t1storage), Return(0)));

    // Test Two Data Setup
    std::unordered_map<int, std::vector<double>> t2storage;
    t2storage[0] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_TWO_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t2storage), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // F1
        .WillOnce(Return(0)); // F0
    EXPECT_CALL(*testPerson, GiveSecondScreeningTest(true)).Times(1);
    EXPECT_CALL(*testPerson,
                DiagnoseFibrosis(person::MeasuredFibrosisState::kF23))
        .Times(2);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(2);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(FibrosisStagingTest, FibrosisStaging_FirstStagingTestTwoLatestChoice) {
    // Person Setup
    ON_CALL(*testPerson, GetTrueFibrosisState())
        .WillByDefault(Return(person::FibrosisState::kF0));
    ON_CALL(*testPerson, GetTimeSinceFibrosisStaging())
        .WillByDefault(Return(13));
    ON_CALL(*testPerson, GetTimeOfFibrosisStaging()).WillByDefault(Return(-1));

    // Data Setup
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one", _))
        .WillByDefault(DoAll(SetArgReferee<1>("test_one"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("15.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("30.00"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_two", _))
        .WillByDefault(DoAll(SetArgReferee<1>("test_two"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.multitest_result_method", _))
        .WillByDefault(DoAll(SetArgReferee<1>("latest"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("fibrosis_staging.test_two_eligible_stages", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>("f0, f1, f2, f3, f4, decomp"), Return(0)));

    // Test One Data Setup
    std::unordered_map<int, std::vector<double>> t1storage;
    t1storage[0] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_ONE_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t1storage), Return(0)));

    // Test Two Data Setup
    std::unordered_map<int, std::vector<double>> t2storage;
    t2storage[0] = {0.0, 0.0, 1.0, 0.0};
    ON_CALL(*event_dm, SelectCustomCallback(TEST_TWO_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_Int_VecDouble(&t2storage), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // F1
        .WillOnce(Return(0)); // F0
    EXPECT_CALL(*testPerson, GiveSecondScreeningTest(true)).Times(1);
    EXPECT_CALL(*testPerson,
                DiagnoseFibrosis(person::MeasuredFibrosisState::kF23))
        .Times(1);
    EXPECT_CALL(*testPerson,
                DiagnoseFibrosis(person::MeasuredFibrosisState::kF01))
        .Times(1);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(2);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisStaging", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
