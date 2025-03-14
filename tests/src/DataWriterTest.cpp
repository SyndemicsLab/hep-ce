////////////////////////////////////////////////////////////////////////////////
// File: DataWriterTest.cpp                                                   //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-12                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-14                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "DataWriter.hpp"
#include "PersonMock.hpp"
#include "Utility.hpp"
#include "Utils.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

const int POPULATION_SIZE = 10;
const std::string EXPECTED_POPULATION =
    "id,sex,age,isAlive,deathReason,identifiedHCV,timeInfectionIdentified,HCV,"
    "fibrosisState,isGenotypeThree,seropositive,timeHCVChanged,"
    "timeFibrosisStateChanged,drugBehavior,timeLastActiveDrugUse,linkageState,"
    "timeOfLinkChange,linkageType,linkCount,measuredFibrosisState,"
    "timeOfLastStaging,timeOfLastScreening,numABTests,numRNATests,"
    "timesInfected,timesCleared,initiatedTreatment,timeOfTreatmentInitiation,"
    "minUtility,multUtility,discountMinUtility,discountMultUtility,"
    "treatmentWithdrawals,treatmentToxicReactions,completedTreatments,svrs,"
    "behaviorUtility,liverUtility,treatmentUtility,backgroundUtility,"
    "hivUtility,lifeSpan,discountedLifeSpan,numberOfTreatmentStarts,"
    "numberOfRetreatments,cost,discount_cost\n1,male,300,false,N/"
    "A,true,1,none,f0,false,false,-1,1,never,0,never,0,NA,0,f01,0,0,0,0,0,0,"
    "false,0,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0\n2,female,300,false,N/"
    "A,false,-1,acute,f1,false,false,1,1,former_noninjection,1,linked,1,"
    "background,1,f23,1,1,1,1,1,1,false,1,0.5,0.5,0.5,0.5,0,1,1,1,0.5,0.5,0.5,"
    "0.5,0.5,1,1,1,1,0,0\n3,male,300,false,N/"
    "A,true,1,chronic,f2,false,false,1,1,former_injection,2,unlinked,2,NA,2,f4,"
    "2,2,2,2,2,2,false,2,0.333333,0.333333,0.333333,0.333333,0,2,2,2,0.333333,"
    "0.333333,0.333333,0.333333,0.333333,2,2,2,2,0,0\n4,female,300,false,N/"
    "A,false,-1,none,f3,false,false,-1,1,noninjection,3,never,3,NA,3,decomp,3,"
    "3,3,3,3,3,false,3,0.25,0.25,0.25,0.25,0,3,3,3,0.25,0.25,0.25,0.25,0.25,3,"
    "3,3,3,0,0\n5,male,300,false,N/"
    "A,true,1,acute,f4,false,false,1,1,injection,4,linked,4,background,4,none,"
    "4,4,4,4,4,4,false,4,0.2,0.2,0.2,0.2,0,4,4,4,0.2,0.2,0.2,0.2,0.2,4,4,4,4,0,"
    "0\n6,female,300,false,N/"
    "A,false,-1,chronic,decomp,false,false,1,1,never,5,unlinked,5,NA,5,f01,5,5,"
    "5,5,5,5,false,5,0.166667,0.166667,0.166667,0.166667,0,5,5,5,0.166667,0."
    "166667,0.166667,0.166667,0.166667,5,5,5,5,0,0\n7,male,300,false,N/"
    "A,true,1,none,none,false,false,-1,-1,former_noninjection,6,never,6,NA,6,"
    "f23,6,6,6,6,6,6,false,6,0.142857,0.142857,0.142857,0.142857,0,6,6,6,0."
    "142857,0.142857,0.142857,0.142857,0.142857,6,6,6,6,0,0\n8,female,300,"
    "false,N/"
    "A,false,-1,acute,f0,false,false,1,1,former_injection,7,linked,7,"
    "background,7,f4,7,7,7,7,7,7,false,7,0.125,0.125,0.125,0.125,0,7,7,7,0.125,"
    "0.125,0.125,0.125,0.125,7,7,7,7,0,0\n9,male,300,false,N/"
    "A,true,1,chronic,f1,false,false,1,1,noninjection,8,unlinked,8,NA,8,decomp,"
    "8,8,8,8,8,8,false,8,0.111111,0.111111,0.111111,0.111111,0,8,8,8,0.111111,"
    "0.111111,0.111111,0.111111,0.111111,8,8,8,8,0,0\n10,female,300,false,N/"
    "A,false,-1,none,f2,false,false,-1,1,injection,9,never,9,NA,9,none,9,9,9,9,"
    "9,9,false,9,0.1,0.1,0.1,0.1,0,9,9,9,0.1,0.1,0.1,0.1,0.1,9,9,9,9,0,0\n";

class DataWriterTest : public ::testing::Test {
private:
    void RegisterLogger() {
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        std::vector<spdlog::sink_ptr> sinks{console_sink};
        auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(),
                                                       sinks.end());
        spdlog::register_logger(logger);
        spdlog::flush_every(std::chrono::seconds(3));
    }

protected:
    std::vector<std::shared_ptr<NiceMock<person::MOCKPerson>>> testPopulation;
    void SetUp() override {
        RegisterLogger();
        for (int i = 0; i < POPULATION_SIZE; ++i) {
            testPopulation.push_back(
                std::make_shared<NiceMock<person::MOCKPerson>>());
        }
    }
    void TearDown() override { spdlog::drop("main"); }
};

TEST_F(DataWriterTest, PopulationString) {
    // Population Setup
    for (int id = 0; id < POPULATION_SIZE; ++id) {
        ON_CALL(*(testPopulation[id]), GetID()).WillByDefault(Return(id + 1));
        person::Sex sex =
            (id % 2 == 0) ? person::Sex::MALE : person::Sex::FEMALE;
        ON_CALL(*(testPopulation[id]), GetSex()).WillByDefault(Return(sex));
        ON_CALL(*(testPopulation[id]), GetAge()).WillByDefault(Return(300));
        ON_CALL(*(testPopulation[id]), GetDeathReason())
            .WillByDefault(Return(person::DeathReason::NA));
        bool hcv_id = (id % 2 == 0) ? true : false;
        ON_CALL(*(testPopulation[id]), IsIdentifiedAsHCVInfected())
            .WillByDefault(Return(hcv_id));
        int hcv_time = (id % 2 == 0) ? 1 : -1;
        ON_CALL(*(testPopulation[id]), GetTimeHCVIdentified())
            .WillByDefault(Return(hcv_time));
        person::HCV hcv =
            static_cast<person::HCV>(id % static_cast<int>(person::HCV::COUNT));
        ON_CALL(*(testPopulation[id]), GetHCV()).WillByDefault(Return(hcv));
        person::FibrosisState fib = static_cast<person::FibrosisState>(
            id % static_cast<int>(person::FibrosisState::COUNT));
        ON_CALL(*(testPopulation[id]), GetTrueFibrosisState())
            .WillByDefault(Return(fib));
        ON_CALL(*(testPopulation[id]), IsGenotypeThree())
            .WillByDefault(Return(false));
        ON_CALL(*(testPopulation[id]), GetSeropositivity())
            .WillByDefault(Return(false));
        int time_hcv_changed = (hcv != person::HCV::NONE) ? 1 : -1;
        ON_CALL(*(testPopulation[id]), GetTimeHCVChanged())
            .WillByDefault(Return(time_hcv_changed));
        int time_fibrosis_changed =
            (fib != person::FibrosisState::NONE) ? 1 : -1;
        ON_CALL(*(testPopulation[id]), GetTimeTrueFibrosisStateChanged())
            .WillByDefault(Return(time_fibrosis_changed));
        person::Behavior behavior = static_cast<person::Behavior>(
            id % static_cast<int>(person::Behavior::COUNT));
        ON_CALL(*(testPopulation[id]), GetBehavior())
            .WillByDefault(Return(behavior));
        ON_CALL(*(testPopulation[id]), GetTimeBehaviorChange())
            .WillByDefault(Return(id));
        person::LinkageState ls = static_cast<person::LinkageState>(
            id % static_cast<int>(person::LinkageState::COUNT));
        ON_CALL(*(testPopulation[id]), GetLinkState(_))
            .WillByDefault(Return(ls));
        ON_CALL(*(testPopulation[id]), GetTimeOfLinkChange(_))
            .WillByDefault(Return(id));
        person::LinkageType lt = (ls == person::LinkageState::LINKED)
                                     ? person::LinkageType::BACKGROUND
                                     : person::LinkageType::NA;
        ON_CALL(*(testPopulation[id]), GetLinkageType(_))
            .WillByDefault(Return(lt));
        ON_CALL(*(testPopulation[id]), GetLinkCount(_))
            .WillByDefault(Return(id));
        person::MeasuredFibrosisState meas =
            static_cast<person::MeasuredFibrosisState>(
                id % static_cast<int>(person::MeasuredFibrosisState::COUNT));
        ON_CALL(*(testPopulation[id]), GetMeasuredFibrosisState())
            .WillByDefault(Return(meas));
        ON_CALL(*(testPopulation[id]), GetTimeOfFibrosisStaging())
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]),
                GetTimeOfLastScreening(person::InfectionType::HCV))
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]),
                GetNumberOfABTests(person::InfectionType::HCV))
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]),
                GetNumberOfRNATests(person::InfectionType::HCV))
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]), GetTimesHCVInfected())
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]), GetHCVClearances())
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]), HasInitiatedTreatment())
            .WillByDefault(Return(false));
        ON_CALL(*(testPopulation[id]), GetTimeOfTreatmentInitiation())
            .WillByDefault(Return(id));
        double util = 1 / static_cast<double>(id + 1);
        person::LifetimeUtility lu = {util, util, util, util};
        ON_CALL(*(testPopulation[id]), GetTotalUtility())
            .WillByDefault(Return(lu));
        ON_CALL(*(testPopulation[id]), GetToxicReactions())
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]), GetCompletedTreatments())
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]), GetSVRs()).WillByDefault(Return(id));
        std::unordered_map<utility::UtilityCategory, double> current_util;
        for (int cat = 0;
             cat < static_cast<int>(utility::UtilityCategory::COUNT); ++cat) {
            current_util[static_cast<utility::UtilityCategory>(cat)] = util;
        }
        ON_CALL(*(testPopulation[id]), GetCurrentUtilities())
            .WillByDefault(Return(current_util));
        ON_CALL(*(testPopulation[id]), GetLifeSpan()).WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]), GetDiscountedLifeSpan())
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]), GetNumberOfTreatmentStarts())
            .WillByDefault(Return(id));
        ON_CALL(*(testPopulation[id]), GetRetreatments())
            .WillByDefault(Return(id));
    }

    // Test Body
    writer::DataWriter writer;
    std::vector<std::shared_ptr<person::PersonBase>> writable_population(
        testPopulation.begin(), testPopulation.end());
    EXPECT_EQ(EXPECTED_POPULATION,
              writer.PopulationToString(writable_population));
}
