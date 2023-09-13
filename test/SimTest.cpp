//===-- SimTest.cpp - Instruction class definition -------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, August 15th 2023, 8:42:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Person.hpp"
#include "Simulation.hpp"
#include <gtest/gtest.h>

TEST(SimulationCreation, DefaultConstructor) {
    Simulation *sim = new Simulation();
    EXPECT_TRUE(sim);
    delete (sim);
}

TEST(SimulationPopulation, CreatePopulation) {
    Simulation sim;
    int N = 10;
    sim.createPopulation(N);

    // the population size should match the arg passed to createPopulation
    const std::vector<std::shared_ptr<Person::Person>> population =
        sim.getPopulation();
    EXPECT_EQ(population.size(), N);
    EXPECT_EQ(Person::count, N);
}
