////////////////////////////////////////////////////////////////////////////////
// File: DataWriterTest.cpp                                                   //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-12                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
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
    person::InfectionType hcv_inf = person:: ::kHcv;
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
                (id % 2 == 0) ? person::Sex::kMale : person::Sex::kFemale;
            ON_CALL(*(testPopulation[id]), GetSex()).WillByDefault(Return(sex));
            ON_CALL(*(testPopulation[id]), GetAge()).WillByDefault(Return(300));
            ON_CALL(*(testPopulation[id]), GetDeathReason())
                .WillByDefault(Return(person::DeathReason::kNa));
            bool hcv_id = (id % 2 == 0) ? true : false;
            ON_CALL(*(testPopulation[id]), IsIdentifiedAsInfected(hcv_inf))
                .WillByDefault(Return(hcv_id));
            int hcv_time = (id % 2 == 0) ? 1 : -1;
            ON_CALL(*(testPopulation[id]), GetTimeHCVIdentified())
                .WillByDefault(Return(hcv_time));
            person::HCV hcv = static_cast<person::HCV>(
                id % static_cast<int>(person::HCV::kCount));
            ON_CALL(*(testPopulation[id]), GetHCV()).WillByDefault(Return(hcv));
            person::FibrosisState fib = static_cast<person::FibrosisState>(
                id % static_cast<int>(person::FibrosisState::kCount));
            ON_CALL(*(testPopulation[id]), GetTrueFibrosisState())
                .WillByDefault(Return(fib));
            ON_CALL(*(testPopulation[id]), IsGenotypeThree())
                .WillByDefault(Return(false));
            ON_CALL(*(testPopulation[id]), GetSeropositivity())
                .WillByDefault(Return(false));
            int time_hcv_changed = (hcv != person::HCV::kNone) ? 1 : -1;
            ON_CALL(*(testPopulation[id]), GetTimeHCVChanged())
                .WillByDefault(Return(time_hcv_changed));
            int time_fibrosis_changed =
                (fib != person::FibrosisState::kNone) ? 1 : -1;
            ON_CALL(*(testPopulation[id]), GetTimeTrueFibrosisStateChanged())
                .WillByDefault(Return(time_fibrosis_changed));
            person::Behavior behavior = static_cast<person::Behavior>(
                id % static_cast<int>(person::Behavior::kCount));
            ON_CALL(*(testPopulation[id]), GetBehavior())
                .WillByDefault(Return(behavior));
            ON_CALL(*(testPopulation[id]), GetTimeBehaviorChange())
                .WillByDefault(Return(id));
            person::LinkageState ls = static_cast<person::LinkageState>(
                id % static_cast<int>(person::LinkageState::kCount));
            ON_CALL(*(testPopulation[id]), GetLinkState(_))
                .WillByDefault(Return(ls));
            ON_CALL(*(testPopulation[id]), GetTimeOfLinkChange(_))
                .WillByDefault(Return(id));
            person::LinkageType lt = (ls == person::LinkageState::kLinked)
                                         ? person::LinkageType::kBackground
                                         : person::LinkageType::kNa;
            ON_CALL(*(testPopulation[id]), GetLinkageType(_))
                .WillByDefault(Return(lt));
            ON_CALL(*(testPopulation[id]), GetLinkCount(_))
                .WillByDefault(Return(id));
            person::MeasuredFibrosisState meas =
                static_cast<person::MeasuredFibrosisState>(
                    id %
                    static_cast<int>(person::MeasuredFibrosisState::kCount));
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
                 cat < static_cast<int>(utility::UtilityCategory::kCount);
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
                person::PregnancyState::kNa, 0, id, id, id, id, id, id};
            ON_CALL(*(testPopulation[id]), GetPregnancyDetails())
                .WillByDefault(Return(current_preg));
        }
    }
    void TearDown() override { spdlog::drop("main"); }
};

TEST_F(DataWriterTest, PopulationHeaders) {
    const std::string expected =
        "id,sex,age,is_alive,boomer_classification,death_reason,drug_behavior,"
        "time_last_active_drug_use,hcv,fibrosis_state,is_genotype_three,"
        "seropositive,time_hcv_changed,time_fibrosis_state_changed,"
        "times_hcv_infected,times_acute_cleared,svrs,hiv,time_hiv_changed,"
        "low_cd4_months_count,hcc_state,hcc_diagnosed,currently_overdosing,"
        "num_overdoses,moud_state,time_started_moud,"
        "current_moud_state_concurrent_months,total_moud_months,"
        "pregnancy_state,time_of_pregnancy_change,pregnancy_count,num_infants,"
        "num_miscarriages,num_infant_hcv_exposures,num_infant_hcv_infections,"
        "num_infant_hcv_tests,measured_fibrosis_state,had_second_test,"
        "time_of_last_staging,hcv_link_state,time_of_hcv_link_change,"
        "hcv_link_type,hcv_link_count,hiv_link_state,time_of_hiv_link_change,"
        "hiv_link_type,hiv_link_count,time_of_last_hcv_screening,"
        "num_hcv_ab_tests,num_hcv_rna_tests,hcv_antibody_positive,"
        "hcv_identified,time_hcv_identified,time_of_last_hiv_screening,"
        "num_hiv_ab_tests,num_hiv_rna_tests,hiv_antibody_positive,"
        "hiv_identified,time_hiv_identified,initiated_hcv_treatment,"
        "time_of_hcv_treatment_initiation,num_hcv_treatment_starts,"
        "num_hcv_treatment_withdrawals,num_hcv_treatment_toxic_reactions,"
        "num_completed_hcv_treatments,num_hcv_retreatments,in_hcv_retreatment,"
        "initiated_hiv_treatment,time_of_hiv_treatment_initiation,"
        "num_hiv_treatment_starts,num_hiv_treatment_withdrawals,"
        "num_hiv_treatment_toxic_reactions,behavior_utility,liver_utility,"
        "treatment_utility,background_utility,hiv_utility,min_utility,"
        "mult_utility,discounted_min_utility,discounted_mult_utility,life_span,"
        "discounted_life_span,cost,discount_cost\n";
    writer::DataWriter writer;
    EXPECT_EQ(writer.PopulationToString({}), expected);
}
