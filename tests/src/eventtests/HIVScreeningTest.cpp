////////////////////////////////////////////////////////////////////////////////
// File: HIVScreeningTest.cpp                                                 //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-17                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVScreening.hpp"
#include "EventTest.cpp"
#include "Utils.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class HIVScreeningTest : public EventTest {};

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
    ON_CALL(*testPerson, GetTimeSinceLastScreening(person::InfectionType::HIV))
        .WillByDefault(Return(12));
    ON_CALL(*testPerson, GetTimeOfLastScreening(person::InfectionType::HIV))
        .WillByDefault(Return(0));
    ON_CALL(*testPerson, GetHIV()).WillByDefault(Return(person::HIV::HIUN));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::NEVER));
    ON_CALL(*testPerson, GetCurrentTimestep()).WillByDefault(Return(1));

    // DataManager setup
    ON_CALL(*event_dm, GetFromConfig("cost.discounting_rate", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.025"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.period", _))
        .WillByDefault(DoAll(SetArgReferee<1>("12"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("hiv_screening.intervention_type", _))
        .WillByDefault(DoAll(SetArgReferee<1>("periodic"), Return(0)));
    // Background linkage
    ON_CALL(*event_dm,
            GetFromConfig("hiv_screening_background.ab_sensitivity", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.95"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("hiv_screening_background.ab_specificity", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.90"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("hiv_screening_background.ab_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("800.00"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("hiv_screening_background.rna_sensitivity", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.90"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("hiv_screening_background.rna_specificity", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.98"), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("hiv_screening_background.rna_cost", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1800.00"), Return(0)));
    double screen_prob = 0.2;
    Utils::tuple_3i tup_3i = std::make_tuple(25, 0, 0);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        bgstorage;
    bgstorage[tup_3i] = screen_prob;
    ON_CALL(*event_dm, SelectCustomCallback(BGScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&bgstorage), Return(0)));
    // Intervention linkage
    double sensitivity = 0.99;
    double specificity = 0.97;
    double cost = 1000.00;
    ON_CALL(*event_dm,
            GetFromConfig("hiv_screening_intervention.ab_sensitivity", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(sensitivity)), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("hiv_screening_intervention.ab_specificity", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(specificity)), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("hiv_screening_intervention.ab_cost", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(cost)), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("hiv_screening_intervention.rna_sensitivity", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(sensitivity)), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("hiv_screening_intervention.rna_specificity", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(specificity)), Return(0)));
    ON_CALL(*event_dm, GetFromConfig("hiv_screening_intervention.rna_cost", _))
        .WillByDefault(
            DoAll(SetArgReferee<1>(std::to_string(cost)), Return(0)));
    screen_prob = 0.9;
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        instorage;
    instorage[tup_3i] = screen_prob;
    ON_CALL(*event_dm, SelectCustomCallback(INTScreenSQL, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&instorage), Return(0)));

    // Decider setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    std::vector<double> expected_screen_prob = {screen_prob};
    EXPECT_CALL(*decider, GetDecision(expected_screen_prob)).Times(1);
    EXPECT_CALL(*testPerson, MarkScreened(person::InfectionType::HIV)).Times(1);
    EXPECT_CALL(*testPerson, AddAbScreen(person::InfectionType::HIV)).Times(1);
    // because the person should be found antibody positive in the ab test
    EXPECT_CALL(*testPerson,
                SetAntibodyPositive(true, person::InfectionType::HIV))
        .Times(1);
    // returns false so that antibody screen runs, then returns true after
    // antibody screen tests positive
    EXPECT_CALL(*testPerson, CheckAntibodyPositive(person::InfectionType::HIV))
        .Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(true));
    double discounted_cost = Utils::discount(cost, 0.025, 1);
    // called both for antibody and rna screening tests
    EXPECT_CALL(*testPerson,
                AddCost(cost, discounted_cost, cost::CostCategory::HIV))
        .Times(2);
    EXPECT_CALL(*testPerson, AddRnaScreen(person::InfectionType::HIV)).Times(1);
    // because person will test rna positive
    EXPECT_CALL(*testPerson, Diagnose(person::InfectionType::HIV)).Times(1);
    // because person hiv is HIUN, uses sensitivity. called twice, once for each
    // screening test
    std::vector<double> expected_sensitivity = {sensitivity};
    EXPECT_CALL(*decider, GetDecision(expected_sensitivity)).Times(2);
    // because this was an intervention screen
    EXPECT_CALL(*testPerson, SetLinkageType(person::LinkageType::INTERVENTION,
                                            person::InfectionType::HIV))
        .Times(1);

    // Running test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVScreening", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
