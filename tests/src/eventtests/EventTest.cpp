////////////////////////////////////////////////////////////////////////////////
// File: EventTest.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include "DataManagerMock.hpp"
#include "DeciderMock.hpp"
#include "PersonMock.hpp"

#include "Event.hpp"
#include "EventFactory.hpp"
#include "Utils.hpp"

using ::testing::_;
using ::testing::DoAll;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

class EventTest : public ::testing::Test {
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
    std::shared_ptr<NiceMock<person::MOCKPerson>> testPerson;
    std::shared_ptr<NiceMock<datamanagement::MOCKDataManager>> event_dm;
    event::EventFactory efactory;
    std::shared_ptr<NiceMock<stats::MOCKDecider>> decider;
    void SetUp() override {
        RegisterLogger();
        event_dm =
            std::make_unique<NiceMock<datamanagement::MOCKDataManager>>();
        decider = std::make_shared<NiceMock<stats::MOCKDecider>>();
        testPerson = std::make_shared<NiceMock<person::MOCKPerson>>();
        ON_CALL(*testPerson, IsAlive()).WillByDefault(Return(true));
    }
    void TearDown() override { spdlog::drop("main"); }
};

struct person_select {
    person::Sex sex = person::Sex::MALE;
    int age = 300;
    bool isAlive = true;
    person::DeathReason deathReason = person::DeathReason::NA;
    int identifiedHCV = false;
    int timeInfectionIdentified = -1;
    person::HCV hcv = person::HCV::NONE;
    person::FibrosisState fibrosisState = person::FibrosisState::NONE;
    bool isGenotypeThree = false;
    bool seropositive = false;
    int timeHCVChanged = -1;
    int timeFibrosisStateChanged = -1;
    person::Behavior drugBehavior = person::Behavior::NEVER;
    int timeLastActiveDrugUse = -1;
    person::LinkageState linkageState = person::LinkageState::NEVER;
    int timeOfLinkChange = -1;
    person::LinkageType linkageType = person::LinkageType::BACKGROUND;
    int linkCount = 0;
    person::MeasuredFibrosisState measuredFibrosisState =
        person::MeasuredFibrosisState::NONE;
    int timeOfLastStaging = -1;
    int timeOfLastScreening = -1;
    int numABTests = 0;
    int numRNATests = 0;
    int timesInfected = 0;
    int timesCleared = 0;
    bool initiatedTreatment = false;
    int timeOfTreatmentInitiation = -1;
    double minUtility = 1.0;
    double multUtility = 1.0;
    int treatmentWithdrawals = 0;
    int treatmentToxicReactions = 0;
    int completedTreatments = 0;
    int svrs = 0;
};

struct cost_util {
    double cost = 100.00;
    double util = 0.75;
};

struct behavior_transitions {
    double never = 0.0;
    double fni = 0.25;
    double fi = 0.25;
    double ni = 0.25;
    double in = 0.25;
};

struct background_smr {
    double back_mort = 0.0;
    double smr = 0.0;
};

struct cost_svr_select {
    int time = 5;
    double svr = 0.8;
};

ACTION_P(SetArg2ToPersonCallbackValue, value) {
    *reinterpret_cast<struct person_select *>(arg2) = *value;
}

ACTION_P(SetArg2ToUM_T2I_CU, value) {
    *reinterpret_cast<
        std::unordered_map<Utils::tuple_2i, struct cost_util,
                           Utils::key_hash_2i, Utils::key_equal_2i> *>(arg2) =
        *value;
}

ACTION_P(SetArg2ToUM_T2I_Double, value) {
    *reinterpret_cast<std::unordered_map<
        Utils::tuple_2i, double, Utils::key_hash_2i, Utils::key_equal_2i> *>(
        arg2) = *value;
}

ACTION_P(SetArg2ToUM_T3I_CU, value) {
    *reinterpret_cast<
        std::unordered_map<Utils::tuple_3i, struct cost_util,
                           Utils::key_hash_3i, Utils::key_equal_3i> *>(arg2) =
        *value;
}

ACTION_P(SetArg2ToUM_T3I_BS, value) {
    *reinterpret_cast<
        std::unordered_map<Utils::tuple_3i, struct background_smr,
                           Utils::key_hash_3i, Utils::key_equal_3i> *>(arg2) =
        *value;
}

ACTION_P(SetArg2ToUM_T3I_Double, value) {
    *reinterpret_cast<std::unordered_map<
        Utils::tuple_3i, double, Utils::key_hash_3i, Utils::key_equal_3i> *>(
        arg2) = *value;
}

ACTION_P(SetArg2ToUM_T4I_Double, value) {
    *reinterpret_cast<std::unordered_map<
        Utils::tuple_4i, double, Utils::key_hash_4i, Utils::key_equal_4i> *>(
        arg2) = *value;
}

ACTION_P(SetArg2ToUM_T4I_BT, value) {
    *reinterpret_cast<
        std::unordered_map<Utils::tuple_4i, struct behavior_transitions,
                           Utils::key_hash_4i, Utils::key_equal_4i> *>(arg2) =
        *value;
}

ACTION_P(SetArg2ToUM_Int_VecDouble, value) {
    *reinterpret_cast<std::unordered_map<int, std::vector<double>> *>(arg2) =
        *value;
}

ACTION_P(SetArg2ToCostUtilCallbackValue, value) {
    *reinterpret_cast<std::vector<struct cost_util> *>(arg2) = *value;
}

ACTION_P(SetArg2ToPureDoubleCallbackValue, value) {
    *reinterpret_cast<double *>(arg2) = *value;
}

ACTION_P(SetArg2ToDoubleCallbackValue, value) {
    *reinterpret_cast<std::vector<double> *>(arg2) = *value;
}

ACTION_P(SetArg2ToBackgroundSMRMortalityCallbackValue, value) {
    *reinterpret_cast<std::vector<struct background_smr> *>(arg2) = *value;
}

ACTION_P(SetArg2ToCostSVRCallbackValue, value) {
    *reinterpret_cast<std::vector<struct cost_svr_select> *>(arg2) = *value;
}
