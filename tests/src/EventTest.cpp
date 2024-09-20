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
#include "Event.hpp"
#include "EventFactory.hpp"
#include "Person.hpp"
#include "PersonFactory.hpp"

class EventTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EventTest, TestExecution) {
    event::EventFactory efactory;
    std::shared_ptr<stats::Decider> decider;
    std::shared_ptr<event::Event> event = efactory.create("Aging");

    // This will cause an error in the Person constructor but we don't care
    person::PersonFactory pfactory;
    std::shared_ptr<person::PersonBase> person = pfactory.create();
    person->CreatePersonFromTable(1234, NULL);
    person->SetAge(0);
    int before = person->GetAge();
    event->Execute(*person, NULL, decider);
    EXPECT_EQ(before + 1, person->GetAge());
}
