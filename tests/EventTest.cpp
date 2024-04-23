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
    double expectedCost = 100.00;

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

    Data::Config config;
    std::shared_ptr<Event::Aging> agingEvent =
        std::make_shared<Event::Aging>(table, config);
    int ct = 1;
    agingEvent->setCurrentTimestep(ct);
    agingEvent->execute(livingPopulation);
    EXPECT_DOUBLE_EQ(expectedAge, livingPopulation[0]->age);
    auto costs = livingPopulation.at(0)->getCosts().getTotals();
    // costs track starting from timestep 0, so check index 1
    EXPECT_DOUBLE_EQ(expectedCost, costs[1]);
}

TEST_F(EventTest, AgingDead) {
    int expectedAge = 0;
    double expectedCost = 0;

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

    Data::Config config;
    std::shared_ptr<Event::Aging> agingEvent =
        std::make_shared<Event::Aging>(table, config);
    int ct = 1;
    agingEvent->setCurrentTimestep(ct);
    agingEvent->execute(deadPopulation);
    EXPECT_DOUBLE_EQ(expectedAge, deadPopulation[0]->age);
    // background costs are not accrued when dead
    auto costs = deadPopulation.at(0)->getCosts().getTotals();
    EXPECT_DOUBLE_EQ(expectedCost, costs[1]);
}

TEST_F(EventTest, BehaviorChange) {
    double expectedCost = 100.00;
    std::shared_ptr<MockDataTable> table = std::make_shared<MockDataTable>();

    std::map<std::string, std::vector<std::string>> transitionData;
    transitionData["age_years"] = {"0"};
    transitionData["gender"] = {"male"};
    transitionData["drug_behavior"] = {"never"};
    transitionData["never"] = {"1.0"};
    transitionData["former_noninjection"] = {"0.0"};
    transitionData["former_injection"] = {"0.0"};
    transitionData["noninjection"] = {"0.0"};
    transitionData["injection"] = {"0.0"};

    std::vector<std::string> transitionHeader = {
        "age_years",           "gender",
        "drug_behavior",       "never",
        "former_noninjection", "former_injection",
        "noninjection",        "injection"};

    Data::DataTableShape transitionShape(1, 8);

    std::shared_ptr<Data::IDataTable> transitionVal =
        std::make_shared<Data::DataTable>(transitionData, transitionShape,
                                          transitionHeader);

    std::map<std::string, std::vector<std::string>> costData;
    costData["gender"] = {"male"};
    costData["drug_behavior"] = {"never"};
    costData["cost"] = {"100.00"};

    std::vector<std::string> costHeader = {"gender", "drug_behavior", "cost"};

    Data::DataTableShape costShape(1, 3);

    std::shared_ptr<Data::IDataTable> costVal =
        std::make_shared<Data::DataTable>(costData, costShape, costHeader);

    std::vector<std::string> joinCols = {"gender", "drug_behavior"};

    std::shared_ptr<Data::IDataTable> bcVal =
        transitionVal->innerJoin(costVal, joinCols, joinCols);

    EXPECT_CALL((*table), selectWhere(_)).WillRepeatedly(Return(bcVal));
    Data::Config config;
    Event::BehaviorChanges behavior(simulation->getGenerator(), table, config);
    int ct = 1;
    behavior.setCurrentTimestep(ct);
    behavior.execute(livingPopulation);

    EXPECT_EQ(Person::BehaviorClassification::NEVER,
              livingPopulation.at(0)->getBehaviorClassification());
    auto costs = livingPopulation.at(0)->getCosts().getTotals();
    EXPECT_DOUBLE_EQ(expectedCost, costs[1]);
}

TEST_F(EventTest, Clearance) {
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    // making clearance probability 100% for testing
    outStream << "[infection]" << std::endl
              << "clearance_prob = 1.0" << std::endl;
    Data::Config config(tempFilePath.string());
    Event::Clearance clearance(simulation->getGenerator(), table, config);
    livingPopulation[0]->infect(0);
    EXPECT_EQ(Person::HEPCState::ACUTE, livingPopulation[0]->getHEPCState());
    // current timestep
    int ct = 1;
    clearance.setCurrentTimestep(ct);
    EXPECT_EQ(0, livingPopulation[0]->getClearances());
    clearance.execute(livingPopulation);
    // checking clears of HCV
    EXPECT_EQ(Person::HEPCState::NONE, livingPopulation[0]->getHEPCState());
    // checking that the clearance is correctly counted
    EXPECT_EQ(1, livingPopulation[0]->getClearances());
}

TEST_F(EventTest, ClearanceNoInfection) {
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Config config;
    Event::Clearance clearance(simulation->getGenerator(), table, config);
    EXPECT_EQ(Person::HEPCState::NONE, livingPopulation[0]->getHEPCState());
    // current timestep
    int ct = 1;
    clearance.setCurrentTimestep(ct);
    clearance.execute(livingPopulation);
    // checking that there is no clearance in the count
    EXPECT_EQ(0, livingPopulation[0]->getClearances());
}

TEST_F(EventTest, DeathByOldAge) {
    Person::Person expectedPerson;
    expectedPerson.die();
    Data::IDataTablePtr table = std::make_shared<MockDataTable>();
    Data::Config config;
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

    std::map<std::string, std::vector<std::string>> costData;
    costData["hcv_status"] = {"none"};
    costData["initial_state"] = {"f0"};
    costData["cost"] = {"100.00"};

    std::vector<std::string> costHeader = {"hcv_status", "initial_state",
                                           "cost"};

    Data::DataTableShape costShape(1, 3);

    std::shared_ptr<Data::IDataTable> costVal =
        std::make_shared<Data::DataTable>(costData, costShape, costHeader);

    std::string joinCol = "initial_state";

    std::shared_ptr<Data::IDataTable> fpVal =
        retVal->innerJoin(costVal, joinCol, joinCol);

    EXPECT_CALL((*table), selectWhere(_)).WillRepeatedly(Return(fpVal));

    Data::Config config;
    Event::FibrosisProgression fibrosisProgression(simulation->getGenerator(),
                                                   table, config);
    livingPopulation[0]->infect(0);
    int ct = 1;
    fibrosisProgression.setCurrentTimestep(ct);
    fibrosisProgression.execute(livingPopulation);
    EXPECT_EQ(Person::FibrosisState::F1,
              livingPopulation[0]->getFibrosisState());
    auto costs = livingPopulation.at(0)->getCosts().getTotals();
    EXPECT_DOUBLE_EQ(100.00, costs[1]);
}

TEST_F(EventTest, FibrosisStagingSingleTest) {
    std::shared_ptr<MockDataTable> table = std::make_shared<MockDataTable>();
    outStream << "[fibrosis_staging]" << std::endl
              << "period = 12" << std::endl
              << "test_one = fib4" << std::endl;
}

TEST_F(EventTest, Infections) {
    std::shared_ptr<MockDataTable> table = std::make_shared<MockDataTable>();

    std::map<std::string, std::vector<std::string>> retData;
    retData["age_years"] = {"0"};
    retData["gender"] = {"male"};
    retData["drug_behavior"] = {"never"};
    retData["incidence"] = {"1.0"};

    std::vector<std::string> retHeader = {"age_years", "gender",
                                          "drug_behavior", "incidence"};

    Data::DataTableShape retShape(1, 4);

    std::shared_ptr<Data::IDataTable> retVal =
        std::make_shared<Data::DataTable>(retData, retShape, retHeader);

    EXPECT_CALL((*table), selectWhere(_)).WillRepeatedly(Return(retVal));

    Data::Config config;
    Event::Infections infections(simulation->getGenerator(), table, config);
    // current timestep
    int ct = 1;
    infections.setCurrentTimestep(ct);
    EXPECT_EQ(0, livingPopulation[0]->getNumInfections());
    infections.execute(livingPopulation);
    // check that person is now infected
    EXPECT_EQ(Person::HEPCState::ACUTE, livingPopulation[0]->getHEPCState());
    // check that the infection counts correctly
    EXPECT_EQ(1, livingPopulation[0]->getNumInfections());
}

TEST_F(EventTest, Linking) {}

TEST_F(EventTest, Screening) {
    outStream << "[screening]" << std::endl
              << "intervention_type = periodic" << std::endl
              << "period = 12" << std::endl
              << "seropositivity_multiplier = 1.0" << std::endl
              << "[screening_background_ab]" << std::endl
              << "cost = 50.00" << std::endl
              << "acute_sensitivity = 1.0" << std::endl
              << "chronic_sensitivity = 1.0" << std::endl
              << "specificity = 1.0" << std::endl
              << "[screening_background_rna]" << std::endl
              << "cost = 25.00" << std::endl
              << "acute_sensitivity = 1.0" << std::endl
              << "chronic_sensitivity = 1.0" << std::endl
              << "specificity = 1.0" << std::endl
              << "[screening_intervention_ab]" << std::endl
              << "cost = 1000.00" << std::endl
              << "acute_sensitivity = 1.0" << std::endl
              << "chronic_sensitivity = 1.0" << std::endl
              << "specificity = 1.0" << std::endl
              << "[screening_intervention_rna]" << std::endl
              << "cost = 100.00" << std::endl
              << "acute_sensitivity = 1.0" << std::endl
              << "chronic_sensitivity = 1.0" << std::endl
              << "specificity = 1.0" << std::endl;

    std::shared_ptr<MockDataTable> table = std::make_shared<MockDataTable>();
    std::map<std::string, std::vector<std::string>> screeningData;
    screeningData["age_years"] = {"0"};
    screeningData["gender"] = {"male"};
    screeningData["drug_behavior"] = {"never"};
    screeningData["background_screen_probability"] = {"1.0"};
    screeningData["background_link_probability"] = {"1.0"};
    screeningData["intervention_screen_probability"] = {"1.0"};
    screeningData["intervention_link_probability"] = {"1.0"};
    std::vector<std::string> screeningHeader = {
        "age_years",
        "gender",
        "drug_behavior",
        "background_screen_probability",
        "background_link_probability"
        "intervention_screen_probability",
        "intervention_link_probability"};
    Data::DataTableShape screeningShape(1, 7);
    std::shared_ptr<Data::IDataTable> screeningVal =
        std::make_shared<Data::DataTable>(screeningData, screeningShape,
                                          screeningHeader);

    EXPECT_CALL((*table), selectWhere(_)).WillRepeatedly(Return(screeningVal));
    Data::Config config(tempFilePath.string());
    Event::Screening screening(simulation->getGenerator(), table, config);
    int ct = 1;
    livingPopulation[0]->infect(ct);
    screening.execute(livingPopulation);
}

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
    Data::Config config(tempFilePath.string());

    Event::Treatment treatment(simulation->getGenerator(), table, config);
    // checking default value for event name
    EXPECT_EQ("Treatment", treatment.EVENT_NAME);

    // checking treatment course values assigned from parsing sim.conf
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
