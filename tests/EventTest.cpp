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

#include "AllEvents.hpp"
#include "Simulation.hpp"
#include "Utils.hpp"
#include "mocks/MockDataTable.hpp"
#include <boost/filesystem.hpp>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <memory>
#include <vector>

using ::testing::_;
using ::testing::Return;

class EventTest : public ::testing::Test {
protected:
    // for temp config files
    std::ofstream outStream;
    boost::filesystem::path tempFilePath =
        boost::filesystem::temp_directory_path();

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

        // for temp config files
        tempFilePath /= boost::filesystem::unique_path("%%%%.conf");
        outStream.open(tempFilePath);
    }
    void TearDown() override {
        if (outStream.is_open()) {
            outStream.close();
        }
    }
};

TEST_F(EventTest, AgingLiving) {
    int expectedAge = 1;

    // make the table for background costs
    std::vector<std::string> tableHeaders = {"age_years", "gender",
                                             "drug_behavior", "cost"};
    std::map<std::string, std::vector<std::string>> tableData;
    tableData["age_years"] = {"0"};
    tableData["gender"] = {"male"};
    tableData["drug_behavior"] = {"never"};
    tableData["cost"] = {"100.00"};
    Data::DataTableShape tableShape(1, 4);
    Data::IDataTablePtr table =
        std::make_shared<Data::DataTable>(tableData, tableShape, tableHeaders);

    Data::Configuration config;
    std::shared_ptr<Event::Aging> agingEvent =
        std::make_shared<Event::Aging>(table, config);
    int ct = 1;
    agingEvent->setCurrentTimestep(ct);
    agingEvent->execute(livingPopulation);
    EXPECT_DOUBLE_EQ(expectedAge, livingPopulation[0]->age);
    auto costs = livingPopulation[0]->getCosts().getTotals();
    std::cout << costs[1] << std::endl;
}

TEST_F(EventTest, AgingDead) {
    int expectedAge = 0;
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Configuration config;
    std::shared_ptr<Event::Aging> agingEvent =
        std::make_shared<Event::Aging>(table, config);
    int ct = 1;
    agingEvent->setCurrentTimestep(ct);
    agingEvent->execute(deadPopulation);
    EXPECT_DOUBLE_EQ(expectedAge, deadPopulation[0]->age);
}

TEST_F(EventTest, BehaviorChange) {

    std::shared_ptr<MockDataTable> table = std::make_shared<MockDataTable>();

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

    EXPECT_CALL((*table), selectWhere(_)).WillRepeatedly(Return(retVal));
    Data::Configuration config;
    Event::BehaviorChanges behavior(simulation->getGenerator(), table, config);
    int ct = 1;
    behavior.setCurrentTimestep(ct);
    behavior.execute(livingPopulation);

    EXPECT_EQ(Person::BehaviorClassification::NEVER,
              livingPopulation.at(0)->getBehaviorClassification());
}

TEST_F(EventTest, Clearance) {
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Configuration config;
    Event::Clearance clearance(simulation->getGenerator(), table, config);
    livingPopulation[0]->infect(0);
    int ct = 1;
    clearance.setCurrentTimestep(ct);
    clearance.execute(livingPopulation);
    EXPECT_EQ(Person::HEPCState::ACUTE, livingPopulation[0]->getHEPCState());
}

TEST_F(EventTest, DeathByOldAge) {
    Person::Person expectedPerson;
    expectedPerson.die();
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Configuration config;
    Event::Death deathEvent(simulation->getGenerator(), table, config);
    livingPopulation[0]->age = 1210;
    int ct = 1;
    deathEvent.setCurrentTimestep(ct);
    deathEvent.execute(livingPopulation);
    EXPECT_EQ(expectedPerson.getIsAlive(), livingPopulation[0]->getIsAlive());
}

TEST_F(EventTest, FibrosisProgression) {
    std::shared_ptr<MockDataTable> table = std::make_shared<MockDataTable>();

    std::map<std::string, std::vector<std::string>> retData;
    retData["initial_state"] = {"f0", "f0"};
    retData["new_state"] = {"f0", "f1"};
    retData["probability"] = {"0.0", "1.0"};

    std::vector<std::string> retHeader = {"initial_state", "new_state",
                                          "probability"};

    Data::DataTableShape retShape(2, 3);

    std::shared_ptr<Data::IDataTable> retVal =
        std::make_shared<Data::DataTable>(retData, retShape, retHeader);

    EXPECT_CALL((*table), selectWhere(_)).WillRepeatedly(Return(retVal));

    Data::Configuration config;
    Event::FibrosisProgression fibrosisProgression(simulation->getGenerator(),
                                                   table, config);
    livingPopulation[0]->infect(0);
    int ct = 1;
    fibrosisProgression.setCurrentTimestep(ct);
    fibrosisProgression.execute(livingPopulation);
    EXPECT_EQ(Person::FibrosisState::F1,
              livingPopulation[0]->getFibrosisState());
}

TEST_F(EventTest, Fibrosis) {}

TEST_F(EventTest, Infections) {
    std::shared_ptr<MockDataTable> table = std::make_shared<MockDataTable>();

    std::map<std::string, std::vector<std::string>> retData;
    retData["age_years"] = {"0"};
    retData["gender"] = {"male"};
    retData["drug_behavior"] = {"never"};
    retData["incidence"] = {"0"};

    std::vector<std::string> retHeader = {"age_years", "gender",
                                          "drug_behavior", "incidence"};

    Data::DataTableShape retShape(1, 4);

    std::shared_ptr<Data::IDataTable> retVal =
        std::make_shared<Data::DataTable>(retData, retShape, retHeader);

    EXPECT_CALL((*table), selectWhere(_)).WillRepeatedly(Return(retVal));

    Data::Configuration config;
    Event::Infections infections(simulation->getGenerator(), table, config);
    int ct = 1;
    infections.setCurrentTimestep(ct);
    infections.execute(livingPopulation);
    EXPECT_EQ(Person::HEPCState::NONE, livingPopulation[0]->getHEPCState());
}

TEST_F(EventTest, Linking) {}

TEST_F(EventTest, Screening) {}

TEST_F(EventTest, Treatment) {
    // create config
    outStream << "[treatment]" << std::endl
              << "courses = foo, alpha, foo, alpha, foo, alpha" << std::endl
              << "duration = 7" << std::endl
              << "cost = 150.00" << std::endl
              << "withdrawal_probability = 0.08" << std::endl
              << "tox_probability = 0.01" << std::endl
              << "tox_cost = 1000.00" << std::endl
              << "tox_utility = 0.7" << std::endl
              << "svr_probability = 0.95" << std::endl
              << "initiation_probability = 0.9" << std::endl
              << std::endl
              << "[course_foo]" << std::endl
              << "regimens = bar, bat" << std::endl
              << std::endl
              << "[regimen_bar]" << std::endl
              << "cost = 100.00" << std::endl
              << "utility = 0.9" << std::endl
              << std::endl
              << "[regimen_bat]" << std::endl
              << "utility = 0.95" << std::endl
              << std::endl
              << "[course_alpha]" << std::endl
              << "regimens = beta" << std::endl
              << "duration = 3" << std::endl
              << "utility = 0.95" << std::endl
              << "cost = 250.00" << std::endl
              << std::endl
              << "[regimen_beta]" << std::endl
              << "duration = 5" << std::endl
              << "utility = 0.95" << std::endl;
    std::shared_ptr<MockDataTable> table = std::make_shared<MockDataTable>();
    Data::Configuration config(tempFilePath.string());

    Event::Treatment treatment(simulation->getGenerator(), table, config);
    // checking default value for event name
    EXPECT_EQ("Treatment", treatment.EVENT_NAME);

    std::vector<Event::Course> courses = treatment.getCourses();
    Event::Regimen expectedBar = {7,    100.00, 0.9,     0.08,
                                  0.95, 0.01,   1000.00, 0.7};
    Event::Regimen expectedBat = {7,    150.00, 0.95,    0.08,
                                  0.95, 0.01,   1000.00, 0.7};
    EXPECT_EQ(expectedBar.duration, courses[0].regimens[0].duration);
    EXPECT_EQ(expectedBar.cost, courses[0].regimens[0].cost);
    EXPECT_EQ(expectedBar.utility, courses[0].regimens[0].utility);
    EXPECT_EQ(expectedBar.withdrawalProbability,
              courses[0].regimens[0].withdrawalProbability);
    EXPECT_EQ(expectedBar.svrProbability,
              courses[0].regimens[0].svrProbability);
    EXPECT_EQ(expectedBar.toxicityProbability,
              courses[0].regimens[0].toxicityProbability);
    EXPECT_EQ(expectedBar.toxicityCost, courses[0].regimens[0].toxicityCost);
    EXPECT_EQ(expectedBar.toxicityUtility,
              courses[0].regimens[0].toxicityUtility);

    EXPECT_EQ(expectedBat.duration, courses[0].regimens[1].duration);
    EXPECT_EQ(expectedBat.cost, courses[0].regimens[1].cost);
    EXPECT_EQ(expectedBat.utility, courses[0].regimens[1].utility);
    EXPECT_EQ(expectedBat.withdrawalProbability,
              courses[0].regimens[1].withdrawalProbability);
    EXPECT_EQ(expectedBat.svrProbability,
              courses[0].regimens[1].svrProbability);
    EXPECT_EQ(expectedBat.toxicityProbability,
              courses[0].regimens[1].toxicityProbability);
    EXPECT_EQ(expectedBat.toxicityCost, courses[0].regimens[1].toxicityCost);
    EXPECT_EQ(expectedBat.toxicityUtility,
              courses[0].regimens[1].toxicityUtility);
}

TEST_F(EventTest, VoluntaryRelinking) {}
