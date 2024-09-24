//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License.
// See https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the Unit Tests for Event and its Subclasses.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include <gtest/gtest.h>
#include <memory>

#include "Aging.hpp"
#include "DataManagerMock.hpp"
#include "Decider.hpp"
#include "Event.hpp"
#include "EventFactory.hpp"
#include "Person.hpp"
#include "PersonFactory.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

using ::testing::_;
using ::testing::DoAll;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

struct person_select {
    person::Sex sex = person::Sex::MALE;
    int age = 25;
    bool isAlive = true;
    person::DeathReason deathReason = person::DeathReason::NA;
    int identifiedHCV = false;
    int timeInfectionIdentified = -1;
    person::HCV hcv = person::HCV::NONE;
    person::FibrosisState fibrosisState = person::FibrosisState::NONE;
    bool isGenotypeThree = false;
    bool seropositive = false;
    int timeHCVChanged = -1;
    int timeFibrosisStateChanged = -1;
    person::Behavior drugBehavior = person::Behavior::NEVER;
    int timeLastActiveDrugUse = -1;
    person::LinkageState linkageState = person::LinkageState::NEVER;
    int timeOfLinkChange = -1;
    person::LinkageType linkageType = person::LinkageType::BACKGROUND;
    int linkCount = 0;
    person::MeasuredFibrosisState measuredFibrosisState =
        person::MeasuredFibrosisState::NONE;
    int timeOfLastStaging = -1;
    int timeOfLastScreening = -1;
    int numABTests = 0;
    int numRNATests = 0;
    int timesInfected = 0;
    int timesCleared = 0;
    bool initiatedTreatment = false;
    int timeOfTreatmentInitiation = -1;
    double minUtility = 1.0;
    double multUtility = 1.0;
    int treatmentWithdrawals = 0;
    int treatmentToxicReactions = 0;
    int completedTreatments = 0;
    int svrs = 0;
};
class EventTest : public ::testing::Test {
private:
    void RegisterLogger() {
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        std::vector<spdlog::sink_ptr> sinks{console_sink};
        auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(),
                                                       sinks.end());
        spdlog::register_logger(logger);
        spdlog::flush_every(std::chrono::seconds(3));
    }

protected:
    std::shared_ptr<person::Person> testPerson;
    std::shared_ptr<datamanagement::MOCKDataManager> person_dm;
    event::EventFactory efactory;
    std::unique_ptr<stats::Decider> decider;
    void SetUp() override {
        RegisterLogger();
        person_dm = std::make_unique<datamanagement::MOCKDataManager>();
        decider = std::make_unique<stats::Decider>();
        person::PersonFactory pfactory;
        testPerson = pfactory.create();
        decider = std::make_unique<stats::Decider>();
    }
    void TearDown() override { spdlog::drop("main"); }
};

ACTION_P(SetArg2ToPersonCallbackValue, value) {
    *reinterpret_cast<struct person_select *>(arg2) = *value;
}

TEST_F(EventTest, Aging) {
    struct person_select person;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);
    std::shared_ptr<event::Event> event = efactory.create("Aging");
    int first_age = testPerson->GetAge();
    event->Execute(*testPerson, NULL, decider);
    EXPECT_EQ(first_age + 1, testPerson->GetAge());
}

TEST_F(EventTest, BehaviorChanges) {
    struct person_select person;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);
    std::shared_ptr<event::Event> event = efactory.create("BehaviorChanges");

    // I want to add storage as the argument from SelectCustomCallback but GMock
    // is being a pain
    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(testing::Return(0));

    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(person::Behavior::NONINJECTION, testPerson->GetBehavior());
}

TEST_F(EventTest, Clearance) {
    struct person_select person;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);
    std::shared_ptr<event::Event> event = efactory.create("Clearance");

    std::string clearance_prob = std::string("1.0");

    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*event_dm, GetFromConfig(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(clearance_prob), Return(0)));
    testPerson->SetHCV(person::HCV::ACUTE);

    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(person::HCV::NONE, testPerson->GetHCV());
}

TEST_F(EventTest, Death) {
    struct person_select person;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);
    std::shared_ptr<event::Event> event = efactory.create("Death");

    std::string f4_mort = std::string("1.0");

    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(f4_mort), Return(0)));

    testPerson->UpdateTrueFibrosis(person::FibrosisState::F4);
    EXPECT_EQ(true, testPerson->IsAlive());
    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(false, testPerson->IsAlive());
}

TEST_F(EventTest, FibrosisProgression) {
    struct person_select person;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);
    std::shared_ptr<event::Event> event =
        efactory.create("FibrosisProgression");

    std::string add_cost = std::string("false");

    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*event_dm,
                GetFromConfig("fibrosis.add_cost_only_if_identified", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(add_cost), Return(0)));

    std::string fib_1 = std::string("1.0");

    EXPECT_CALL(*event_dm, GetFromConfig("fibrosis.f01", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(fib_1), Return(0)));

    testPerson->SetHCV(person::HCV::CHRONIC);
    testPerson->UpdateTrueFibrosis(person::FibrosisState::F0);
    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(person::FibrosisState::F1, testPerson->GetTrueFibrosisState());
}

ACTION_P(SetArg2ToCallbackValue, value) {
    *reinterpret_cast<std::vector<double> *>(arg2) = *value;
}

TEST_F(EventTest, FibrosisStaging) {
    struct person_select person;
    person.fibrosisState = person::FibrosisState::F4;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);

    std::shared_ptr<event::Event> event = efactory.create("FibrosisStaging");

    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();

    std::vector<double> storage = {0.0, 0.0, 1.0, 0.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(DoAll(SetArg2ToCallbackValue(&storage), Return(0)));

    std::string t1 = std::string("0.0");
    EXPECT_CALL(*event_dm, GetFromConfig(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(t1), Return(0)));

    std::string period = std::string("12");
    EXPECT_CALL(*event_dm, GetFromConfig("fibrosis_staging.period", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(period), Return(0)));

    std::string col = std::string("colname");
    EXPECT_CALL(*event_dm, GetFromConfig("fibrosis_staging.test_one", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(col), Return(0)));

    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(person::MeasuredFibrosisState::F4,
              testPerson->GetMeasuredFibrosisState());
}

TEST_F(EventTest, Infections) {
    struct person_select person;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);

    std::shared_ptr<event::Event> event = efactory.create("Infections");

    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();

    std::vector<double> storage = {1.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(DoAll(SetArg2ToCallbackValue(&storage), Return(0)));

    testPerson->SetHCV(person::HCV::NONE);
    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(person::HCV::ACUTE, testPerson->GetHCV());
}

TEST_F(EventTest, Linking) {
    struct person_select person;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);

    std::shared_ptr<event::Event> event = efactory.create("Linking");

    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();

    std::vector<double> storage = {1.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(DoAll(SetArg2ToCallbackValue(&storage), Return(0)));

    std::string cost = std::string("20.25");
    EXPECT_CALL(*event_dm, GetFromConfig("linking.false_positive_test_cost", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(cost), Return(0)));

    std::string relink = std::string("1.0");
    EXPECT_CALL(*event_dm, GetFromConfig("linking.relink_multiplier", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(relink), Return(0)));

    testPerson->SetHCV(person::HCV::ACUTE);
    int numLinks = testPerson->GetLinkCount();
    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(person::LinkageState::LINKED, testPerson->GetLinkState());
    EXPECT_EQ(numLinks + 1, testPerson->GetLinkCount());
}

TEST_F(EventTest, Screening) {
    struct person_select person;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);

    std::shared_ptr<event::Event> event = efactory.create("Screening");

    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();

    std::vector<double> storage = {1.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(DoAll(SetArg2ToCallbackValue(&storage), Return(0)));

    std::string specificity = std::string("1.0");
    EXPECT_CALL(*event_dm, GetFromConfig(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(specificity), Return(0)));

    std::string intervention = std::string("periodic");
    EXPECT_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(intervention), Return(0)));

    std::string period = std::string("6");
    EXPECT_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(period), Return(0)));

    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(0, testPerson->GetTimeSinceLastScreening());
}

TEST_F(EventTest, Treatment) {
    struct person_select person;
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(
            DoAll(SetArg2ToPersonCallbackValue(&person), Return(0)));
    testPerson->CreatePersonFromTable(4321, person_dm);

    std::shared_ptr<event::Event> event = efactory.create("Treatment");

    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();

    std::vector<double> storage = {1.0};
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(DoAll(SetArg2ToCallbackValue(&storage), Return(0)));

    std::string specificity = std::string("1.0");
    EXPECT_CALL(*event_dm, GetFromConfig(_, _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(specificity), Return(0)));

    std::string intervention = std::string("periodic");
    EXPECT_CALL(*event_dm, GetFromConfig("screening.intervention_type", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(intervention), Return(0)));

    std::string period = std::string("6");
    EXPECT_CALL(*event_dm, GetFromConfig("screening.period", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(period), Return(0)));

    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(0, testPerson->GetTimeSinceLastScreening());
}
