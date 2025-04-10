////////////////////////////////////////////////////////////////////////////////
// File: HIVScreeningTest.cpp                                                 //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-17                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-09                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVScreening.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class HIVScreeningTest : public EventTest {
protected:
    person::InfectionType it = person::InfectionType::HIV;
    // pair representing background, intervention
    // actual values don't matter for probabilities because the decider is
    // set up for the intended outcomes
    std::pair<double, double> screen_prob = {0.2, 0.9};
    std::pair<double, double> sensitivity = {0.92, 0.99};
    std::pair<double, double> specificity = {0.95, 0.97};
    std::pair<double, double> cost = {1800.00, 1000.00};
    void SetUp() override {
        // call parent class method
        EventTest::SetUp();

        // Person setup
        ON_CALL(*testPerson, GetHIV()).WillByDefault(Return(person::HIV::HIUN));
        ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
        ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
        ON_CALL(*testPerson, GetBehavior())
            .WillByDefault(Return(person::Behavior::NEVER));
        ON_CALL(*testPerson, GetCurrentTimestep()).WillByDefault(Return(1));
        ON_CALL(*testPerson, GetTimeOfLastScreening(it))
            .WillByDefault(Return(0));

        // DataManager setup
        ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
            .WillByDefault(DoAll(SetArgReferee<1>("0.025"), Return(0)));
        ON_CALL(*event_dm, GetFromConfig("hiv_screening.period", _))
            .WillByDefault(DoAll(SetArgReferee<1>("12"), Return(0)));
        // Background screening
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_background.ab_sensitivity", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(sensitivity.first)),
                      Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_background.ab_specificity", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(specificity.first)),
                      Return(0)));
        ON_CALL(*event_dm, GetFromConfig("hiv_screening_background.ab_cost", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(cost.first)), Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_background.rna_sensitivity", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(sensitivity.first)),
                      Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_background.rna_specificity", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(specificity.first)),
                      Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_background.rna_cost", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(cost.first)), Return(0)));
        // Intervention screening
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_intervention.ab_sensitivity", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(sensitivity.second)),
                      Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_intervention.ab_specificity", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(specificity.second)),
                      Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_intervention.ab_cost", _))
            .WillByDefault(DoAll(SetArgReferee<1>(std::to_string(cost.second)),
                                 Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_intervention.rna_sensitivity", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(sensitivity.second)),
                      Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_intervention.rna_specificity", _))
            .WillByDefault(
                DoAll(SetArgReferee<1>(std::to_string(specificity.second)),
                      Return(0)));
        ON_CALL(*event_dm,
                GetFromConfig("hiv_screening_intervention.rna_cost", _))
            .WillByDefault(DoAll(SetArgReferee<1>(std::to_string(cost.second)),
                                 Return(0)));
    }
};

std::string BGScreenSQL =
    "SELECT at.age_years, sl.gender, sl.drug_behavior, "
    "hiv_background_screen_probability"
    " FROM antibody_testing AS at INNER JOIN "
    "screening_and_linkage AS sl ON ((at.age_years = "
    "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";

std::string INTScreenSQL =
    "SELECT at.age_years, sl.gender, sl.drug_behavior, "
    "hiv_intervention_screen_probability"
    " FROM antibody_testing AS at INNER JOIN "
    "screening_and_linkage AS sl ON ((at.age_years = "
    "sl.age_years) AND (at.drug_behavior = sl.drug_behavior));";

TEST_F(HIVScreeningTest, PeriodicScreening_T_AB_T_RNA) {
    // Person setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening(it))
        .WillByDefault(Return(12));

    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    // Intervention screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        instorage;
    instorage[tup_3i] = screen_prob.second;
    // bg screen prob being set to same as intervention to suppress warning
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));

    // Decider setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    std::vector<double> expected_screen_prob = {screen_prob.second};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob)).Times(1);
    EXPECT_CALL(*testPerson, MarkScreened(it)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(it)).Times(1);
    // because the person should be found antibody positive in the ab test
    EXPECT_CALL(*testPerson, SetAntibodyPositive(true, it)).Times(1);
    // returns false so that antibody screen runs, then returns true after
    // antibody screen tests positive
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(it))
        .Times(1)
        .WillOnce(Return(false));
    double discounted_cost = Utils::discount(cost.second, 0.025, 1);
    // called both for antibody and rna screening tests
    EXPECT_CALL(*testPerson,
                AddCost(cost.second, discounted_cost, cost::CostCategory::HIV))
        .Times(2);
    EXPECT_CALL(*testPerson, AddRnaScreen(it)).Times(1);
    // because person will test rna positive
    EXPECT_CALL(*testPerson, Diagnose(it)).Times(1);
    // because person hiv is HIUN, uses sensitivity. called twice, once for each
    // screening test
    std::vector<double> expected_sensitivity = {sensitivity.second};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(2);
    // because this was an intervention screen
    EXPECT_CALL(*testPerson,
                SetLinkageType(person::LinkageType::INTERVENTION, it))
        .Times(1);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, PeriodicScreening_T_AB_F_RNA) {
    // Person setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening(it))
        .WillByDefault(Return(12));

    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    // Intervention screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        instorage;
    instorage[tup_3i] = screen_prob.second;
    // bg screen prob being set to same as intervention to suppress warning
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));

    // Expectations
    std::vector<double> expected_screen_prob = {screen_prob.second};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob))
        .Times(1)
        .WillOnce(Return(0));
    EXPECT_CALL(*testPerson, MarkScreened(it)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(it)).Times(1);
    // because the person should be found antibody positive in the ab test
    EXPECT_CALL(*testPerson, SetAntibodyPositive(true, it)).Times(1);
    // returns false so that antibody screen runs, then returns true after
    // antibody screen tests positive
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(it))
        .Times(1)
        .WillOnce(Return(false));
    double discounted_cost = Utils::discount(cost.second, 0.025, 1);
    // called both for antibody and rna screening tests
    EXPECT_CALL(*testPerson,
                AddCost(cost.second, discounted_cost, cost::CostCategory::HIV))
        .Times(2);
    EXPECT_CALL(*testPerson, AddRnaScreen(it)).Times(1);
    // because person hiv is HIUN, uses sensitivity. called twice, once for each
    // screening test. the first time it will come back positive (antibody),
    // then it will come back negative (rna)
    std::vector<double> expected_sensitivity = {sensitivity.second};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(1));
    // because person will test rna negative, diagnose is never called
    EXPECT_CALL(*testPerson, Diagnose(it)).Times(0);
    // because the rna test came back negative, never called
    EXPECT_CALL(*testPerson,
                SetLinkageType(person::LinkageType::INTERVENTION, it))
        .Times(0);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, PeriodicScreening_F_AB) {
    // Person setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening(it))
        .WillByDefault(Return(12));

    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    // Intervention screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        instorage;
    instorage[tup_3i] = screen_prob.second;
    // bg screen prob being set to same as intervention to suppress warning
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));

    // Expectations
    std::vector<double> expected_screen_prob = {screen_prob.second};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob))
        .Times(1)
        .WillOnce(Return(0));
    EXPECT_CALL(*testPerson, MarkScreened(it)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(it)).Times(1);
    // because the person should be found antibody negative in the antibody test
    EXPECT_CALL(*testPerson, SetAntibodyPositive(true, it)).Times(0);
    // returns false so that antibody screen runs, but is only called once this
    // time because the antibody screen comes back negative
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(it))
        .Times(1)
        .WillOnce(Return(false));
    double discounted_cost = Utils::discount(cost.second, 0.025, 1);
    // called only for antibody screening
    EXPECT_CALL(*testPerson,
                AddCost(cost.second, discounted_cost, cost::CostCategory::HIV))
        .Times(1);
    // never gets rna screened
    EXPECT_CALL(*testPerson, AddRnaScreen(it)).Times(0);
    // because person hiv is HIUN, uses sensitivity. called once, because
    // ab screen will come back negative
    std::vector<double> expected_sensitivity = {sensitivity.second};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity))
        .Times(1)
        .WillOnce(Return(1));
    // because person will not be rna screened, never diagnosed or given link
    // type
    EXPECT_CALL(*testPerson, Diagnose(it)).Times(0);
    EXPECT_CALL(*testPerson,
                SetLinkageType(person::LinkageType::INTERVENTION, it))
        .Times(0);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, PeriodicScreening_TooSoonToScreen) {
    // Person setup
    ON_CALL(*testPerson, GetTimeSinceLastScreening(it))
        .WillByDefault(Return(11));

    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    // Background screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bgstorage;
    bgstorage[tup_3i] = screen_prob.first;
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));
    // Intervention screening
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        instorage;
    instorage[tup_3i] = screen_prob.second;
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));

    // Decider setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    // person still goes through all of screening as in T_AB_T_RNA case but it
    // is background screening instead, because it has not been long enough
    // since last screening
    // must use screen_prob.first because it's background screening
    std::vector<double> expected_screen_prob = {screen_prob.first};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob)).Times(1);
    EXPECT_CALL(*testPerson, MarkScreened(it)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(it)).Times(1);
    // because the person should be found antibody positive in the ab test
    EXPECT_CALL(*testPerson, SetAntibodyPositive(true, it)).Times(1);
    // returns false so that antibody screen runs, then returns true after
    // antibody screen tests positive
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(it))
        .Times(1)
        .WillOnce(Return(false));
    double discounted_cost = Utils::discount(cost.first, 0.025, 1);
    // called both for antibody and rna screening tests
    // must use cost.first here because it's background screening
    EXPECT_CALL(*testPerson,
                AddCost(cost.first, discounted_cost, cost::CostCategory::HIV))
        .Times(2);
    EXPECT_CALL(*testPerson, AddRnaScreen(it)).Times(1);
    // because person will test rna positive
    EXPECT_CALL(*testPerson, Diagnose(it)).Times(1);
    // because person hiv is HIUN, uses sensitivity. called twice, once for each
    // screening test
    // must use sensitivity.first because it's background screening
    std::vector<double> expected_sensitivity = {sensitivity.first};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(2);
    // because this was a background screen
    EXPECT_CALL(*testPerson,
                SetLinkageType(person::LinkageType::BACKGROUND, it))
        .Times(1);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, OneTimeScreening) {
    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("one-time"), Return(0)));
    // Intervention screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        instorage;
    instorage[tup_3i] = screen_prob.second;
    // bg screen prob being set to same as intervention to suppress warning
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));

    // Decider setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    std::vector<double> expected_screen_prob = {screen_prob.second};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob)).Times(1);
    EXPECT_CALL(*testPerson, MarkScreened(it)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(it)).Times(1);
    // because the person should be found antibody positive in the ab test
    EXPECT_CALL(*testPerson, SetAntibodyPositive(true, it)).Times(1);
    // returns false so that antibody screen runs, then returns true after
    // antibody screen tests positive
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(it))
        .Times(1)
        .WillOnce(Return(false));
    double discounted_cost = Utils::discount(cost.second, 0.025, 1);
    // called both for antibody and rna screening tests
    EXPECT_CALL(*testPerson,
                AddCost(cost.second, discounted_cost, cost::CostCategory::HIV))
        .Times(2);
    EXPECT_CALL(*testPerson, AddRnaScreen(it)).Times(1);
    // because person will test rna positive
    EXPECT_CALL(*testPerson, Diagnose(it)).Times(1);
    // because person hiv is HIUN, uses sensitivity. called twice, once for each
    // screening test
    std::vector<double> expected_sensitivity = {sensitivity.second};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(2);
    // because this was an intervention screen
    EXPECT_CALL(*testPerson,
                SetLinkageType(person::LinkageType::INTERVENTION, it))
        .Times(1);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, OneTimeScreening_NotFirstTimeStep) {
    // Person setup
    ON_CALL(*testPerson, GetCurrentTimestep()).WillByDefault(Return(2));

    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    // Background screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bgstorage;
    bgstorage[tup_3i] = screen_prob.first;
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));
    // Intervention screening
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        instorage;
    instorage[tup_3i] = screen_prob.second;
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));

    // Decider setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    // person still goes through all of screening as in T_AB_T_RNA case but it
    // is background screening instead, because it is not the first timestep
    // must use screen_prob.first because it's background screening
    std::vector<double> expected_screen_prob = {screen_prob.first};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob)).Times(1);
    EXPECT_CALL(*testPerson, MarkScreened(it)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(it)).Times(1);
    // because the person should be found antibody positive in the ab test
    EXPECT_CALL(*testPerson, SetAntibodyPositive(true, it)).Times(1);
    // returns false so that antibody screen runs, then returns true after
    // antibody screen tests positive
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(it))
        .Times(1)
        .WillOnce(Return(false));
    double discounted_cost = Utils::discount(cost.first, 0.025, 2);
    // called both for antibody and rna screening tests
    // must use cost.first here because it's background screening
    EXPECT_CALL(*testPerson,
                AddCost(cost.first, discounted_cost, cost::CostCategory::HIV))
        .Times(2);
    EXPECT_CALL(*testPerson, AddRnaScreen(it)).Times(1);
    // because person will test rna positive
    EXPECT_CALL(*testPerson, Diagnose(it)).Times(1);
    // because person hiv is HIUN, uses sensitivity. called twice, once for each
    // screening test
    // must use sensitivity.first because it's background screening
    std::vector<double> expected_sensitivity = {sensitivity.first};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(2);
    // because this was a background screen
    EXPECT_CALL(*testPerson,
                SetLinkageType(person::LinkageType::BACKGROUND, it))
        .Times(1);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, BackgroundScreening_T_AB_T_RNA) {
    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("null"), Return(0)));
    // Background screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bgstorage;
    bgstorage[tup_3i] = screen_prob.first;
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));
    // intervention screen prob being set to the same as background to suppress
    // warning
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));

    // Decider setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    std::vector<double> expected_screen_prob = {screen_prob.first};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob)).Times(1);
    EXPECT_CALL(*testPerson, MarkScreened(it)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(it)).Times(1);
    // because the person should be found antibody positive in the ab test
    EXPECT_CALL(*testPerson, SetAntibodyPositive(true, it)).Times(1);
    // returns false so that antibody screen runs, then returns true after
    // antibody screen tests positive
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(it))
        .Times(1)
        .WillOnce(Return(false));
    double discounted_cost = Utils::discount(cost.first, 0.025, 1);
    // called both for antibody and rna screening tests
    EXPECT_CALL(*testPerson,
                AddCost(cost.first, discounted_cost, cost::CostCategory::HIV))
        .Times(2);
    EXPECT_CALL(*testPerson, AddRnaScreen(it)).Times(1);
    // because person will test rna positive
    EXPECT_CALL(*testPerson, Diagnose(it)).Times(1);
    // because person hiv is HIUN, uses sensitivity. called twice, once for each
    // screening test
    std::vector<double> expected_sensitivity = {sensitivity.first};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(2);
    // because this was a background screen
    EXPECT_CALL(*testPerson,
                SetLinkageType(person::LinkageType::BACKGROUND, it))
        .Times(1);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, BackgroundScreening_T_AB_F_RNA) {
    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("null"), Return(0)));
    // Background screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bgstorage;
    bgstorage[tup_3i] = screen_prob.first;
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));
    // intervention screen prob being set to the same as background to suppress
    // warning
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));

    // Expectations
    std::vector<double> expected_screen_prob = {screen_prob.first};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob))
        .Times(1)
        .WillOnce(Return(0));
    EXPECT_CALL(*testPerson, MarkScreened(it)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(it)).Times(1);
    // because the person should be found antibody positive in the ab test
    EXPECT_CALL(*testPerson, SetAntibodyPositive(true, it)).Times(1);
    // returns false so that antibody screen runs, then returns true after
    // antibody screen tests positive
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(it))
        .Times(1)
        .WillOnce(Return(false));
    double discounted_cost = Utils::discount(cost.first, 0.025, 1);
    // called both for antibody and rna screening tests
    EXPECT_CALL(*testPerson,
                AddCost(cost.first, discounted_cost, cost::CostCategory::HIV))
        .Times(2);
    EXPECT_CALL(*testPerson, AddRnaScreen(it)).Times(1);
    // because person will test rna negative, never called
    EXPECT_CALL(*testPerson, Diagnose(it)).Times(0);
    // because person hiv is HIUN, uses sensitivity. called twice, once for each
    // screening test
    std::vector<double> expected_sensitivity = {sensitivity.first};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity))
        .Times(2)
        .WillOnce(Return(0))
        .WillOnce(Return(1));
    // never called because person tests rna negative
    EXPECT_CALL(*testPerson,
                SetLinkageType(person::LinkageType::BACKGROUND, it))
        .Times(0);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, BackgroundScreening_F_AB) {
    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("null"), Return(0)));
    // Background screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bgstorage;
    bgstorage[tup_3i] = screen_prob.first;
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));
    // intervention screen prob being set to the same as background to suppress
    // warning
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));

    // Expectations
    std::vector<double> expected_screen_prob = {screen_prob.first};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob))
        .Times(1)
        .WillOnce(Return(0));
    EXPECT_CALL(*testPerson, MarkScreened(it)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(it)).Times(1);
    // because the person should be found antibody negative in the first test
    EXPECT_CALL(*testPerson, SetAntibodyPositive(true, it)).Times(0);
    // returns false so that antibody screen runs, never called again because
    // person is antibody negative
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(it))
        .Times(1)
        .WillOnce(Return(false));
    double discounted_cost = Utils::discount(cost.first, 0.025, 1);
    // called only for antibody test
    EXPECT_CALL(*testPerson,
                AddCost(cost.first, discounted_cost, cost::CostCategory::HIV))
        .Times(1);
    // never called because person is found antibody negative
    EXPECT_CALL(*testPerson, AddRnaScreen(it)).Times(0);
    // because person hiv is HIUN, uses sensitivity
    std::vector<double> expected_sensitivity = {sensitivity.first};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity))
        .Times(1)
        .WillOnce(Return(1));
    // never called because person never gets past ab screening
    EXPECT_CALL(*testPerson, Diagnose(it)).Times(0);
    EXPECT_CALL(*testPerson,
                SetLinkageType(person::LinkageType::BACKGROUND, it))
        .Times(0);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, DeclineScreen) {
    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("null"), Return(0)));
    // Background screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bgstorage;
    bgstorage[tup_3i] = screen_prob.first;
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));
    // intervention screen prob being set to the same as background to suppress
    // warning
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));

    // Expectations
    std::vector<double> expected_screen_prob = {screen_prob.first};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob))
        .Times(1)
        .WillOnce(Return(1));
    // screening methods never called because person does not screen
    EXPECT_CALL(*testPerson, MarkScreened(_)).Times(0);
    EXPECT_CALL(*testPerson, AddAbScreen(_)).Times(0);
    EXPECT_CALL(*testPerson, SetAntibodyPositive(_, _)).Times(0);
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(_)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(*testPerson, AddRnaScreen(_)).Times(0);
    std::vector<double> expected_sensitivity = {sensitivity.first};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(0);
    EXPECT_CALL(*testPerson, Diagnose(_)).Times(0);
    EXPECT_CALL(*testPerson, SetLinkageType(_, _)).Times(0);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(HIVScreeningTest, AlreadyLinked) {
    // Person setup
    ON_CALL(*testPerson, GetLinkState(it))
        .WillByDefault(Return(person::LinkageState::LINKED));

    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("null"), Return(0)));
    // Background screening
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bgstorage;
    bgstorage[tup_3i] = screen_prob.first;
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));
    // intervention screen prob being set to the same as background to suppress
    // warning
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));

    // Expectations
    // all screening methods never called because person does not screen
    EXPECT_CALL(*decider, GetDecision(_)).Times(0);
    EXPECT_CALL(*testPerson, MarkScreened(_)).Times(0);
    EXPECT_CALL(*testPerson, AddAbScreen(_)).Times(0);
    EXPECT_CALL(*testPerson, SetAntibodyPositive(_, _)).Times(0);
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(_)).Times(0);
    EXPECT_CALL(*testPerson, AddCost(_, _, _)).Times(0);
    EXPECT_CALL(*testPerson, AddRnaScreen(_)).Times(0);
    EXPECT_CALL(*testPerson, Diagnose(_)).Times(0);
    EXPECT_CALL(*testPerson, SetLinkageType(_, _)).Times(0);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
