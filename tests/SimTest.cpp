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
/// This file contains the Unit Tests for Simulation and its Subclasses.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "AllEvents.hpp"
#include "Person.hpp"
#include "Simulation.hpp"
#include <gtest/gtest.h>

TEST(SimulationCreation, DefaultConstructor) {
    Simulation::Simulation *sim = new Simulation::Simulation();
    EXPECT_TRUE(sim);
    delete (sim);
}

class SimulationTest : public ::testing::Test {
protected:
    Simulation::Simulation sim;
    int N = 10;
    void SetUp() override { sim.createPopulation(N); }
    void TearDown() override {}
};

TEST_F(SimulationTest, CreatePopulation) {
    // the population size should match the arg passed to createPopulation
    const std::vector<std::shared_ptr<Person::Person>> &population =
        sim.getPopulation();
    EXPECT_EQ(population.size(), N);
    EXPECT_EQ(Person::count, N);
}

TEST_F(SimulationTest, AddPerson) {
    Person::Person person;
    sim.addPerson(person);

    const std::vector<std::shared_ptr<Person::Person>> &population =
        sim.getPopulation();

    // check that the population size increased
    EXPECT_EQ(population[N]->getID(), N);
    EXPECT_EQ(population.size(), N + 1);
}

TEST_F(SimulationTest, AddEventToEnd) {
    Data::DataTable table;
    Data::Configuration config;
    std::shared_ptr<Event::Event> event =
        std::make_shared<Event::Aging>(table, config);

    sim.addEventToEnd(nullptr);
    sim.addEventToEnd(event);
    sim.addEventToEnd(nullptr);

    const auto &events = sim.getEvents();
    EXPECT_EQ(events.size(), 3);
    EXPECT_EQ(events[1], event);
    EXPECT_EQ(events[2], nullptr);
}

TEST_F(SimulationTest, AddEventToBeginning) {
    sim.addEventToBeginning(nullptr);
    Data::DataTable table;
    Data::Configuration config;

    std::shared_ptr<Event::Event> event =
        std::make_shared<Event::BehaviorChanges>(sim.getGenerator(), table,
                                                 config);
    sim.addEventToBeginning(event);
    EXPECT_EQ(sim.getEvents()[0], event);
}

TEST_F(SimulationTest, AddEventAtIndex) {
    sim.addEventToEnd(nullptr);
    sim.addEventToEnd(nullptr);
    sim.addEventToEnd(nullptr);

    Data::DataTable table;
    Data::Configuration config;

    std::shared_ptr<Event::Event> event =
        std::make_shared<Event::Aging>(table, config);
    sim.addEventAtIndex(event, 1);
    EXPECT_EQ(sim.getEvents()[1], event);
    EXPECT_EQ(sim.getEvents().size(), 4);
}

TEST_F(SimulationTest, AddEventAtInvalidIndex) {
    sim.addEventToEnd(nullptr);
    sim.addEventToEnd(nullptr);
    sim.addEventToEnd(nullptr);

    Data::DataTable table;
    Data::Configuration config;

    std::shared_ptr<Event::Event> event =
        std::make_shared<Event::Aging>(table, config);
    // index out of range
    EXPECT_FALSE(sim.addEventAtIndex(event, 4));
    EXPECT_EQ(sim.getEvents().size(), 3);
}