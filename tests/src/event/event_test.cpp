////////////////////////////////////////////////////////////////////////////////
// File: EventTest.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-03-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

// Testing File
#include <hepce/event/event.hpp>

// STL Includes
#include <memory>
#include <string>
#include <vector>

// 3rd Party Dependencies
#include <gtest/gtest.h>

// Library Headers
#include <hepce/utils/pair_hashing.hpp>

// Local Includes
#include <person_mock.hpp>
#include <sampler_mock.hpp>

using ::testing::_;
using ::testing::DoAll;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::SetArgPointee;
using ::testing::SetArgReferee;

using namespace hepce::model;
using namespace hepce::event;

class EventTest : public ::testing::Test {
private:
protected:
    NiceMock<MockPerson> mock_person;
    NiceMock<MockSampler> mock_sampler;
    void SetUp() override {
        ON_CALL(mock_person, IsAlive()).WillByDefault(Return(true));
    }
    void TearDown() override {}
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

ACTION_P(SetArg2ToUM_T2I_CU, value) {
    *reinterpret_cast<std::unordered_map<
        hepce::utils::tuple_2i, struct cost_util, hepce::utils::key_hash_2i,
        hepce::utils::key_equal_2i> *>(arg2) = *value;
}

ACTION_P(SetArg2ToUM_T2I_Double, value) {
    *reinterpret_cast<std::unordered_map<hepce::utils::tuple_2i, double,
                                         hepce::utils::key_hash_2i,
                                         hepce::utils::key_equal_2i> *>(arg2) =
        *value;
}

ACTION_P(SetArg2ToUM_T3I_CU, value) {
    *reinterpret_cast<std::unordered_map<
        hepce::utils::tuple_3i, struct cost_util, hepce::utils::key_hash_3i,
        hepce::utils::key_equal_3i> *>(arg2) = *value;
}

ACTION_P(SetArg2ToUM_T3I_BS, value) {
    *reinterpret_cast<std::unordered_map<
        hepce::utils::tuple_3i, struct background_smr,
        hepce::utils::key_hash_3i, hepce::utils::key_equal_3i> *>(arg2) =
        *value;
}

ACTION_P(SetArg2ToUM_T3I_Double, value) {
    *reinterpret_cast<std::unordered_map<hepce::utils::tuple_3i, double,
                                         hepce::utils::key_hash_3i,
                                         hepce::utils::key_equal_3i> *>(arg2) =
        *value;
}

ACTION_P(SetArg2ToUM_T4I_Double, value) {
    *reinterpret_cast<std::unordered_map<hepce::utils::tuple_4i, double,
                                         hepce::utils::key_hash_4i,
                                         hepce::utils::key_equal_4i> *>(arg2) =
        *value;
}

ACTION_P(SetArg2ToUM_T4I_BT, value) {
    *reinterpret_cast<std::unordered_map<
        hepce::utils::tuple_4i, struct behavior_transitions,
        hepce::utils::key_hash_4i, hepce::utils::key_equal_4i> *>(arg2) =
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
