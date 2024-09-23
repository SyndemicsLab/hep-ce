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

struct behavior_trans_select {
    double never = 0.0;
    double fni = 0.0;
    double fi = 0.0;
    double ni = 0.0;
    double in = 1.0;
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
    std::shared_ptr<person::PersonBase> testPerson;
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

TEST_F(EventTest, Aging) {
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(testing::Return(0));
    testPerson->CreatePersonFromTable(4321, NULL);
    std::shared_ptr<event::Event> event = efactory.create("Aging");
    int first_age = 25;
    testPerson->SetAge(first_age);
    event->Execute(*testPerson, NULL, decider);
    EXPECT_EQ(first_age + 1, testPerson->GetAge());
}

TEST_F(EventTest, BehaviorChanges) {
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(testing::Return(0));
    testPerson->CreatePersonFromTable(4321, person_dm);
    std::shared_ptr<event::Event> event = efactory.create("BehaviorChanges");

    struct behavior_trans_select storage;

    // I want to add storage as the argument from SelectCustomCallback but GMock
    // is being a pain
    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(testing::Return(0));
    // .WillRepeatedly(SetArgPointee<2>(SetBehaviorTransition(storage)));

    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(person::Behavior::NONINJECTION, testPerson->GetBehavior());
}

TEST_F(EventTest, Clearance) {
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(testing::Return(0));
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
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(testing::Return(0));
    testPerson->CreatePersonFromTable(4321, person_dm);
    std::shared_ptr<event::Event> event = efactory.create("Death");

    std::string f4_mort = std::string("1.0");

    std::shared_ptr<datamanagement::MOCKDataManager> event_dm =
        std::make_unique<datamanagement::MOCKDataManager>();
    EXPECT_CALL(*event_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(Return(0));
    EXPECT_CALL(*event_dm, GetFromConfig("mortality.f4", _))
        .WillRepeatedly(DoAll(SetArgReferee<1>(f4_mort), Return(0)));

    testPerson->SetTrueFibrosisState(person::FibrosisState::F4);
    EXPECT_EQ(true, testPerson->IsAlive());
    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(false, testPerson->IsAlive());
}

TEST_F(EventTest, FibrosisProgression) {
    EXPECT_CALL(*person_dm, SelectCustomCallback(_, _, _, _))
        .WillRepeatedly(testing::Return(0));
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
    testPerson->SetTrueFibrosisState(person::FibrosisState::F0);
    event->Execute(*testPerson, event_dm, decider);
    EXPECT_EQ(person::FibrosisState::F1, testPerson->GetTrueFibrosisState());
}
