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

#include <hepce/data/writer.hpp>

#include <memory>
#include <vector>

#include <gtest/gtest.h>

#include <person_mock.hpp>

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

const int POPULATION_SIZE = 10;

using namespace hepce::data;
using namespace hepce::model;

namespace hepce {
namespace testing {

class DataWriterTest : public ::testing::Test {
protected:
    InfectionType hcv_inf = InfectionType::kHcv;
    void SetUp() override {
        auto person = NiceMock<MockPerson>();
        // Population Setup
        ON_CALL(person, GetSex()).WillByDefault(Return(Sex::kMale));
        ON_CALL(person, GetAge()).WillByDefault(Return(300));
        ON_CALL(person, GetDeathReason())
            .WillByDefault(Return(DeathReason::kNa));

        LifetimeUtility lu = {.8, .7, .8, .7};
        ON_CALL(person, GetTotalUtility()).WillByDefault(Return(lu));

        ON_CALL(person, GetLifeSpan()).WillByDefault(Return(45));
        ON_CALL(person, GetDiscountedLifeSpan()).WillByDefault(Return(40));
        PregnancyDetails current_preg = {
            PregnancyState::kNa, 0, 40, 40, 40, 40, 40, 40};
        ON_CALL(person, GetPregnancyDetails())
            .WillByDefault(Return(current_preg));
    }
    void TearDown() override {}
};

TEST_F(DataWriterTest, PopulationString) {}

} // namespace testing
} // namespace hepce
