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
#include <vector>

#include "AllEvents.hpp"
#include "Simulation.hpp"
#include "Utils.hpp"

class EventTest : public ::testing::Test {
protected:
    std::vector<std::shared_ptr<Person::Person>> livingPopulation;
    std::vector<std::shared_ptr<Person::Person>> deadPopulation;
    std::shared_ptr<Simulation::Simulation> simulation;
    void SetUp() override {
        simulation = std::make_shared<Simulation::Simulation>(0, 0);
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
    Data::Configuration config;
    std::shared_ptr<Event::Aging> agingEvent =
        std::make_shared<Event::Aging>(config);
    agingEvent->execute(livingPopulation, 1);
    EXPECT_DOUBLE_EQ(expectedAge, livingPopulation[0]->age);
}

TEST_F(EventTest, AgingDead) {
    double expectedAge = 0.0;
    Data::Configuration config;
    std::shared_ptr<Event::Aging> agingEvent =
        std::make_shared<Event::Aging>(config);
    agingEvent->execute(deadPopulation, 1);
    EXPECT_DOUBLE_EQ(expectedAge, deadPopulation[0]->age);
}

TEST_F(EventTest, BehaviorChange) {
    Data::DataTable table;
    Data::Configuration config;
    Event::BehaviorChanges behavior(simulation->getGenerator(), table, config);
    behavior.execute(livingPopulation, 1);

    EXPECT_EQ(Person::BehaviorClassification::NEVER,
              livingPopulation.at(0)->getBehaviorClassification());
}

TEST_F(EventTest, Clearance) {
    Data::DataTable table;
    Data::Configuration config;
    Event::Clearance clearance(simulation->getGenerator(), table, config);
    livingPopulation[0]->infect(0);
    clearance.execute(livingPopulation, 1);
    EXPECT_EQ(Person::HEPCState::ACUTE, livingPopulation[0]->getHEPCState());
}

TEST_F(EventTest, DeathByOldAge) {
    Person::Person expectedPerson;
    expectedPerson.die();
    Data::DataTable table;
    Data::Configuration config;
    Event::Death deathEvent(simulation->getGenerator(), table, config);
    livingPopulation[0]->age = 1210;
    deathEvent.execute(livingPopulation, 1);
    EXPECT_EQ(expectedPerson.getIsAlive(), livingPopulation[0]->getIsAlive());
}

TEST_F(EventTest, DiseaseProgression) {
    Data::DataTable table;
    Data::Configuration config;
    Event::DiseaseProgression diseaseProgression(simulation->getGenerator(),
                                                 table, config);
    livingPopulation[0]->infect(0);
    diseaseProgression.execute(livingPopulation, 1);
    EXPECT_EQ(Person::LiverState::F0, livingPopulation[0]->getLiverState());
}

TEST_F(EventTest, Fibrosis) {}

TEST_F(EventTest, Infections) {
    Data::DataTable table;
    Data::Configuration config;
    Event::Infections infections(simulation->getGenerator(), table, config);
    infections.execute(livingPopulation, 1);
    EXPECT_EQ(Person::HEPCState::NONE, livingPopulation[0]->getHEPCState());
}

TEST_F(EventTest, ScreenageLinking) {}

TEST_F(EventTest, Screening) {}

TEST_F(EventTest, Treatment) {}

TEST_F(EventTest, VoluntaryRelinking) {}
