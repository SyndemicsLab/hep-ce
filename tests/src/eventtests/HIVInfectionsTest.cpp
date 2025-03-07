////////////////////////////////////////////////////////////////////////////////
// File: HIVInfectionsTest.cpp                                                //
// Project: HEP-CE                                                            //
// Created: 2025-02-28                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HIVInfections.hpp"
#include "EventTest.cpp"
#include "Utils.hpp"

class HIVInfectionsTest : public EventTest {};

std::string SQL_QUERY =
    "SELECT age_years, gender, drug_behavior, incidence FROM "
    "hiv_incidence;";

TEST_F(HIVInfectionsTest, Infect) {
    // Person Setup
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::MALE));
    ON_CALL(*testPerson, GetBehavior())
        .WillByDefault(Return(person::Behavior::INJECTION));

    // Data Setup
    double hiv_infection_prob = 0.8;
    Utils::tuple_3i tup = std::make_tuple(25, 0, 4);
    std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                       Utils::key_equal_3i>
        storage;
    storage[tup] = hiv_infection_prob;
    ON_CALL(*event_dm, SelectCustomCallback(SQL_QUERY, _, _, _))
        .WillByDefault(DoAll(SetArg2ToUM_T3I_Double(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    std::vector<double> expected_infection_prob = {hiv_infection_prob};
    EXPECT_CALL(*decider, GetDecision(expected_infection_prob)).Times(1);
    EXPECT_CALL(*testPerson, GetHIV())
        .Times(1)
        .WillOnce(Return(person::HIV::NONE));
    EXPECT_CALL(*testPerson, InfectHIV()).Times(1);

    // Run Test
    std::shared_ptr<event::Event> event =
        efactory.create("HIVInfections", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
