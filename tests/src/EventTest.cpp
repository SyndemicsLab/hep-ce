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
#include "Event.hpp"
#include "EventFactory.hpp"
#include "Person.hpp"
#include "PersonFactory.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

using ::testing::_;
using ::testing::Return;

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
    void SetUp() override {
        RegisterLogger();
        std::shared_ptr<datamanagement::MOCKDataManager> dm =
            std::make_unique<datamanagement::MOCKDataManager>();
        EXPECT_CALL(*dm, SelectCustomCallback(_, _, _, _))
            .WillRepeatedly(testing::Return(0));
        person::PersonFactory pfactory;
        testPerson = pfactory.create();
        testPerson->CreatePersonFromTable(4321, NULL);
    }
    void TearDown() override { spdlog::drop("main"); }
};

TEST_F(EventTest, Aging) {
    event::EventFactory efactory;
    std::shared_ptr<stats::Decider> decider;
    std::shared_ptr<event::Event> event = efactory.create("Aging");
    int first_age = 25;
    testPerson->SetAge(first_age);
    event->Execute(*testPerson, NULL, decider);
    EXPECT_EQ(first_age + 1, testPerson->GetAge());
}

TEST_F(EventTest, BehaviorChanges) {
    event::EventFactory efactory;
    std::shared_ptr<stats::Decider> decider;
    std::shared_ptr<event::Event> event = efactory.create("BehaviorChanges");

    int before = testPerson->GetAge();
    event->Execute(*testPerson, NULL, decider);
    EXPECT_EQ(before + 1, testPerson->GetAge());
}
