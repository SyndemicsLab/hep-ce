////////////////////////////////////////////////////////////////////////////////
// File: DataWriterTest.cpp                                                   //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-12                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-25                                                  //
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
    person::InfectionType hcv_inf = person::InfectionType::HCV;
    std::vector<std::shared_ptr<NiceMock<person::MOCKPerson>>> testPopulation;
    void SetUp() override {
        RegisterLogger();
        for (int i = 0; i < POPULATION_SIZE; ++i) {
            testPopulation.push_back(
                std::make_shared<NiceMock<person::MOCKPerson>>());
        }
        // Population Setup
        for (int id = 0; id < POPULATION_SIZE; ++id) {
            ON_CALL(*(testPopulation[id]), GetID())
                .WillByDefault(Return(id + 1));
            person::Sex sex =
                (id % 2 == 0) ? person::Sex::MALE : person::Sex::FEMALE;
            ON_CALL(*(testPopulation[id]), GetSex()).WillByDefault(Return(sex));
            ON_CALL(*(testPopulation[id]), GetAge()).WillByDefault(Return(300));
            ON_CALL(*(testPopulation[id]), GetDeathReason())
                .WillByDefault(Return(person::DeathReason::NA));
            bool hcv_id = (id % 2 == 0) ? true : false;
            ON_CALL(*(testPopulation[id]), IsIdentifiedAsInfected(hcv_inf))
                .WillByDefault(Return(hcv_id));
            int hcv_time = (id % 2 == 0) ? 1 : -1;
            ON_CALL(*(testPopulation[id]), GetTimeHCVIdentified())
                .WillByDefault(Return(hcv_time));
            person::HCV hcv = static_cast<person::HCV>(
                id % static_cast<int>(person::HCV::COUNT));
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
                    id %
                    static_cast<int>(person::MeasuredFibrosisState::COUNT));
            ON_CALL(*(testPopulation[id]), GetMeasuredFibrosisState())
                .WillByDefault(Return(meas));
            ON_CALL(*(testPopulation[id]), GetTimeOfFibrosisStaging())
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetTimeOfLastScreening(hcv_inf))
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetNumberOfABTests(hcv_inf))
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetNumberOfRNATests(hcv_inf))
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetTimesHCVInfected())
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetAcuteHCVClearances())
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), HasInitiatedTreatment(hcv_inf))
                .WillByDefault(Return(false));
            ON_CALL(*(testPopulation[id]),
                    GetTimeOfTreatmentInitiation(hcv_inf))
                .WillByDefault(Return(id));
            double util = 1 / static_cast<double>(id + 1);
            person::LifetimeUtility lu = {util, util, util, util};
            ON_CALL(*(testPopulation[id]), GetTotalUtility())
                .WillByDefault(Return(lu));
            ON_CALL(*(testPopulation[id]), GetToxicReactions(hcv_inf))
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetCompletedTreatments(hcv_inf))
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetSVRs()).WillByDefault(Return(id));
            std::unordered_map<utility::UtilityCategory, double> current_util;
            for (int cat = 0;
                 cat < static_cast<int>(utility::UtilityCategory::COUNT);
                 ++cat) {
                current_util[static_cast<utility::UtilityCategory>(cat)] = util;
            }
            ON_CALL(*(testPopulation[id]), GetCurrentUtilities())
                .WillByDefault(Return(current_util));
            ON_CALL(*(testPopulation[id]), GetLifeSpan())
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetDiscountedLifeSpan())
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetNumberOfTreatmentStarts(hcv_inf))
                .WillByDefault(Return(id));
            ON_CALL(*(testPopulation[id]), GetRetreatments(hcv_inf))
                .WillByDefault(Return(id));
            person::PregnancyDetails current_preg = {
                person::PregnancyState::NA, 0, id, id, id, id, id, id};
            ON_CALL(*(testPopulation[id]), GetPregnancyDetails())
                .WillByDefault(Return(current_preg));
        }
    }
    void TearDown() override { spdlog::drop("main"); }
};

TEST_F(DataWriterTest, PopulationString) {}
