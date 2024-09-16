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

class EventTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(EventTest, TestExecution) {
    event::EventFactory factory;
    std::shared_ptr<stats::Decider> decider;
    std::shared_ptr<event::Event> event =
        factory.create(decider, NULL, "Aging");

    // This will cause an error in the Person constructor but we don't care
    person::PersonBase person(1234, NULL);
    person.SetAge(0);
    int before = person.GetAge();
    event->Execute(person);
    EXPECT_EQ(before + 1, person.GetAge());
}
