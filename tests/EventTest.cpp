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
#include <iostream>
#include <memory>
#include <vector>

#include "AllEvents.hpp"
#include "Simulation.hpp"
#include "Utils.hpp"
#include "mocks/MockDataTable.hpp"

using ::testing::_;
using ::testing::Return;

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
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Configuration config;
    std::shared_ptr<Event::Aging> agingEvent =
        std::make_shared<Event::Aging>(table, config);
    agingEvent->execute(livingPopulation, 1);
    EXPECT_DOUBLE_EQ(expectedAge, livingPopulation[0]->age);
}

TEST_F(EventTest, AgingDead) {
    double expectedAge = 0.0;
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Configuration config;
    std::shared_ptr<Event::Aging> agingEvent =
        std::make_shared<Event::Aging>(table, config);
    agingEvent->execute(deadPopulation, 1);
    EXPECT_DOUBLE_EQ(expectedAge, deadPopulation[0]->age);
}

TEST_F(EventTest, BehaviorChange) {

    std::shared_ptr<MockDataTable> mockedTable =
        std::make_shared<MockDataTable>();

    std::map<std::string, std::vector<std::string>> retData;
    retData["never"] = {"1.0"};
    retData["former_noninjection"] = {"0.0"};
    retData["former_injection"] = {"0.0"};
    retData["noninjection"] = {"0.0"};
    retData["injection"] = {"0.0"};

    std::vector<std::string> retHeader = {"never", "former_noninjection",
                                          "former_injection", "noninjection",
                                          "injection"};

    Data::DataTableShape retShape(1, 5);

    std::shared_ptr<Data::IDataTable> retVal =
        std::make_shared<Data::DataTable>(retData, retShape, retHeader);

    EXPECT_CALL((*mockedTable), selectWhere(_)).WillRepeatedly(Return(retVal));
    Data::Configuration config;
    Event::BehaviorChanges behavior(simulation->getGenerator(), mockedTable,
                                    config);
    behavior.execute(livingPopulation, 1);

    EXPECT_EQ(Person::BehaviorClassification::NEVER,
              livingPopulation.at(0)->getBehaviorClassification());
}

TEST_F(EventTest, Clearance) {
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Configuration config;
    Event::Clearance clearance(simulation->getGenerator(), table, config);
    livingPopulation[0]->infect(0);
    clearance.execute(livingPopulation, 1);
    EXPECT_EQ(Person::HEPCState::ACUTE, livingPopulation[0]->getHEPCState());
}

TEST_F(EventTest, DeathByOldAge) {
    Person::Person expectedPerson;
    expectedPerson.die();
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Configuration config;
    Event::Death deathEvent(simulation->getGenerator(), table, config);
    livingPopulation[0]->age = 1210;
    deathEvent.execute(livingPopulation, 1);
    EXPECT_EQ(expectedPerson.getIsAlive(), livingPopulation[0]->getIsAlive());
}

TEST_F(EventTest, DiseaseProgression) {
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Configuration config;
    Event::DiseaseProgression diseaseProgression(simulation->getGenerator(),
                                                 table, config);
    livingPopulation[0]->infect(0);
    diseaseProgression.execute(livingPopulation, 1);
    EXPECT_EQ(Person::LiverState::F0, livingPopulation[0]->getLiverState());
}

TEST_F(EventTest, Fibrosis) {}

TEST_F(EventTest, Infections) {
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Configuration config;
    Event::Infections infections(simulation->getGenerator(), table, config);
    infections.execute(livingPopulation, 1);
    EXPECT_EQ(Person::HEPCState::NONE, livingPopulation[0]->getHEPCState());
}

TEST_F(EventTest, Linking) {}

TEST_F(EventTest, Screening) {}

TEST_F(EventTest, Treatment) {}

TEST_F(EventTest, VoluntaryRelinking) {}
