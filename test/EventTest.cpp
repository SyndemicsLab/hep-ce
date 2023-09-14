//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
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
#include <vector>

#include "Aging.hpp"

class EventTest : public ::testing::Test {
protected:
    std::vector<std::shared_ptr<Person::Person>> livingPopulation;
    std::vector<std::shared_ptr<Person::Person>> deadPopulation;
    void SetUp() override {
        std::shared_ptr<Person::Person> livingPerson =
            std::make_shared<Person::Person>();
        livingPopulation.push_back(livingPerson);

        std::shared_ptr<Person::Person> deadPerson =
            std::make_shared<Person::Person>();
        deadPerson->die();
        deadPopulation.push_back(deadPerson);
    }
    void TearDown() override {}
};

TEST_F(EventTest, AgingLiving) {
    double expectedAge = 1.0 / 12.0;
    std::shared_ptr<Event::Aging> agingEvent = std::make_shared<Event::Aging>();
    agingEvent->execute(livingPopulation, 1);
    EXPECT_DOUBLE_EQ(expectedAge, livingPopulation[0]->age);
}

TEST_F(EventTest, AgingDead) {
    double expectedAge = 0.0;
    std::shared_ptr<Event::Aging> agingEvent = std::make_shared<Event::Aging>();
    agingEvent->execute(deadPopulation, 1);
    EXPECT_DOUBLE_EQ(expectedAge, deadPopulation[0]->age);
}

// TEST_F(EventTest, BehaviorChange) {}
