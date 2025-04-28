////////////////////////////////////////////////////////////////////////////////
// File: PregnancyTest.cpp                                                    //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Pregnancy.hpp"
#include "EventTest.cpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class PregnancyTest : public EventTest {};

TEST_F(PregnancyTest, Males) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kMale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kNone));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(-1));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_)).Times(0); // No Randomness

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, TooYoung) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(120));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPostpartum));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(2));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_)).Times(0); // No Randomness

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, TooOld) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(550));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kNone));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(-1));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_)).Times(0); // No Randomness

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Postpartum) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPostpartum));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(2));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Expectations
    EXPECT_CALL(*decider, GetDecision(_)).Times(0); // No Randomness

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, EndPostpartum_ReImpregnate) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPostpartum));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(3));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, EndPostpartum()).Times(1);
    EXPECT_CALL(*testPerson, Impregnate()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Pregnant_Miscarry) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPregnant));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(5));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, Miscarry()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Pregnant_HealthyTimestep) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPregnant));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(5));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1));

    // Expectations
    EXPECT_CALL(*testPerson, Miscarry()).Times(0);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Pregnant_Stillbirth) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPregnant));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(9));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(0));

    // Expectations
    EXPECT_CALL(*testPerson, Stillbirth()).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Pregnant_Healthy_Birth) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPregnant));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(9));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));

    // Data Setup
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    ON_CALL(*decider, GetDecision(_)).WillByDefault(Return(1));

    // Expectations
    EXPECT_CALL(*testPerson, Stillbirth()).Times(0);
    EXPECT_CALL(*testPerson, AddChild(_, _)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Pregnant_Twins) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPregnant));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(9));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kAcute));

    // Data Setup
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // Not a stillbirth
        .WillOnce(Return(0)); // Have a Multibirth

    // Expectations
    EXPECT_CALL(*testPerson, Stillbirth()).Times(0);
    EXPECT_CALL(*testPerson, AddChild(_, _)).Times(2);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Pregnant_Test_NoInfect_Child) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPregnant));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(9));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kChronic));

    // Data Setup
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // Not a stillbirth
        .WillOnce(Return(1))  // Only 1 child
        .WillOnce(Return(0))  // Test Child
        .WillOnce(Return(1)); // Child Is Not Infected

    // Expectations
    EXPECT_CALL(*testPerson, Stillbirth()).Times(0);
    EXPECT_CALL(*testPerson, AddChild(person::HCV::kNone, true)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Pregnant_NoTest_NoInfect_Child) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPregnant));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(9));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kChronic));

    // Data Setup
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // Not a stillbirth
        .WillOnce(Return(1))  // Only 1 child
        .WillOnce(Return(1))  // Do Not test Child
        .WillOnce(Return(1)); // Child Is Not Infected

    // Expectations
    EXPECT_CALL(*testPerson, Stillbirth()).Times(0);
    EXPECT_CALL(*testPerson, AddChild(person::HCV::kNone, false)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Pregnant_Test_Infect_Child) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPregnant));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(9));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kChronic));

    // Data Setup
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // Not a stillbirth
        .WillOnce(Return(1))  // Only 1 child
        .WillOnce(Return(0))  // Test Child
        .WillOnce(Return(0)); // Child Is Infected

    // Expectations
    EXPECT_CALL(*testPerson, Stillbirth()).Times(0);
    EXPECT_CALL(*testPerson, AddChild(person::HCV::kChronic, true)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}

TEST_F(PregnancyTest, Pregnant_NoTest_Infect_Children) {
    // Person Setup
    ON_CALL(*testPerson, GetSex()).WillByDefault(Return(person::Sex::kFemale));
    ON_CALL(*testPerson, GetAge()).WillByDefault(Return(300));
    ON_CALL(*testPerson, GetPregnancyState())
        .WillByDefault(Return(person::PregnancyState::kPregnant));
    ON_CALL(*testPerson, GetTimeSincePregnancyChange())
        .WillByDefault(Return(9));
    ON_CALL(*testPerson, GetHCV()).WillByDefault(Return(person::HCV::kChronic));

    // Data Setup
    double storage = 1.0;
    ON_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillByDefault(
            DoAll(SetArg2ToPureDoubleCallbackValue(&storage), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.multiple_delivery_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.infant_hcv_tested_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("0.0"), Return(0)));
    ON_CALL(*event_dm,
            GetFromConfig("pregnancy.vertical_hcv_transition_probability", _))
        .WillByDefault(DoAll(SetArgReferee<1>("1.0"), Return(0)));

    // Decider Setup
    EXPECT_CALL(*decider, GetDecision(_))
        .WillOnce(Return(1))  // Not a stillbirth
        .WillOnce(Return(1))  // Only 1 child
        .WillOnce(Return(1))  // Do Not test Child
        .WillOnce(Return(0)); // Child Is Infected

    // Expectations
    EXPECT_CALL(*testPerson, Stillbirth()).Times(0);
    EXPECT_CALL(*testPerson, AddChild(person::HCV::kChronic, false)).Times(1);

    // Running Test
    std::shared_ptr<event::Event> event =
        efactory.create("Pregnancy", event_dm);
    event->Execute(testPerson, event_dm, decider);
}
