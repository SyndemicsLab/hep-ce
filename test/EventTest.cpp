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

#include "AllEvents.hpp"
#include "Simulation.hpp"
#include "Utils.hpp"

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

TEST_F(EventTest, BehaviorChange) {
    Simulation sim(0, 0);
    Data::Database db("HEP-CE.db");
    Event::BehaviorChanges behavior(sim.getGenerator(), db);
    behavior.execute(livingPopulation, 1);

    Simulation expectedSim(0, 0);
    Person::BehaviorClassification expectedClassification;
    std::vector<double> probs = {0.25, 0.25, 0.25, 0.25};
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    double value = uniform(expectedSim.getGenerator());
    double reference = 0.0;
    for (int i = 0; i < probs.size(); ++i) {
        reference += probs[i];
        if (value < reference) {
            expectedClassification = (Person::BehaviorClassification)i;
            break;
        }
    }
    if (value > reference) {
        expectedClassification =
            (Person::BehaviorClassification)(int)probs.size();
    }

    EXPECT_EQ(expectedClassification,
              livingPopulation.at(0)->getBehaviorClassification());
}

TEST_F(EventTest, Clearance) {
    Simulation sim(0, 0);
    Data::Database db("HEP-CE.db");
    Event::Clearance clearance(sim.getGenerator(), db);
    livingPopulation[0]->infect();
    clearance.execute(livingPopulation, 1);
    EXPECT_EQ(Person::HEPCState::NONE, livingPopulation[0]->getHEPCState());
}

TEST_F(EventTest, DeathByOldAge) {
    Person::Person expectedPerson;
    expectedPerson.die();
    std::shared_ptr<Event::Death> deathEvent = std::make_shared<Event::Death>();
    livingPopulation[0]->age = 1210;
    deathEvent->execute(livingPopulation, 1);
    EXPECT_EQ(expectedPerson.getIsAlive(), livingPopulation[0]->getIsAlive());
}

TEST_F(EventTest, DiseaseProgression) {
    Simulation sim(0, 0);
    Data::Database db("HEP-CE.db");
    Event::DiseaseProgression diseaseProgression(sim.getGenerator(), db);
    livingPopulation[0]->infect();
    diseaseProgression.execute(livingPopulation, 1);
    EXPECT_EQ(Person::LiverState::F0, livingPopulation[0]->getLiverState());
}

TEST_F(EventTest, Fibrosis) {}

TEST_F(EventTest, Infections) {
    Simulation sim(0, 0);
    Data::Database db("HEP-CE.db");
    Event::Infections infections(sim.getGenerator(), db);
    infections.execute(livingPopulation, 1);
    EXPECT_EQ(Person::HEPCState::NONE, livingPopulation[0]->getHEPCState());
}

TEST_F(EventTest, ScreenageLinking) {}

TEST_F(EventTest, Screening) {}

TEST_F(EventTest, Treatment) {}

TEST_F(EventTest, VoluntaryRelinking) {}
