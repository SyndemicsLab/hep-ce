////////////////////////////////////////////////////////////////////////////////
// File: FibrosisProgression.cpp                                              //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-21                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "FibrosisProgression.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utility.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
class FibrosisProgression::FibrosisProgressionIMPL {
private:
    utility::UtilityCategory util_category = utility::UtilityCategory::LIVER;
    double discount = 0.0;
    double f01_probability;
    double f12_probability;
    double f23_probability;
    double f34_probability;
    double f4d_probability;
    bool costFlag = false;
    struct cost_util {
        double cost;
        double util;
    };

    using map_t = std::unordered_map<Utils::tuple_2i, struct cost_util,
                                     Utils::key_hash_2i, Utils::key_equal_2i>;
    map_t costutil_data;

    std::string SQLQuery() {
        return "SELECT hcv_status, fibrosis_state, cost, utility FROM "
               "hcv_impacts;";
    }

    static int callback(void *storage, int count, char **data, char **columns) {
        Utils::tuple_2i tup =
            std::make_tuple(std::stoi(data[0]), std::stoi(data[1]));
        (*((map_t *)storage))[tup] = {Utils::stod_positive(data[2]),
                                      Utils::stod_positive(data[3])};
        return 0;
    }

    std::vector<double>
    getTransition(person::FibrosisState fs,
                  std::shared_ptr<datamanagement::DataManagerBase> dm) {
        // get the probability of transitioning to the next fibrosis state
        double data;

        switch (fs) {
        case person::FibrosisState::F0:
            data = f01_probability;
            break;
        case person::FibrosisState::F1:
            data = f12_probability;
            break;
        case person::FibrosisState::F2:
            data = f23_probability;
            break;
        case person::FibrosisState::F3:
            data = f34_probability;
            break;
        case person::FibrosisState::F4:
            data = f4d_probability;
            break;
        case person::FibrosisState::DECOMP:
            data = 0.0;
            break;
        }

        // to work with getDecision, return the complement to the transition
        // probability
        return {data, 1 - data};
    }

    void AddLiverDiseaseCost(std::shared_ptr<person::PersonBase> person) {
        int hcv_status = (person->GetHCV() == person::HCV::NONE) ? 0 : 1;
        int fibrosis_state = (int)person->GetTrueFibrosisState();
        Utils::tuple_2i tup = std::make_tuple(hcv_status, fibrosis_state);

        double discountAdjustedCost = Event::DiscountEventCost(
            costutil_data[tup].cost, discount, person->GetCurrentTimestep());
        person->AddCost(costutil_data[tup].cost, discountAdjustedCost,
                        cost::CostCategory::LIVER);
    }

    void SetLiverUtility(std::shared_ptr<person::PersonBase> person) {
        int hcv_status = (person->GetHCV() == person::HCV::NONE) ? 0 : 1;
        int fibrosis_state = (int)person->GetTrueFibrosisState();
        Utils::tuple_2i tup = std::make_tuple(hcv_status, fibrosis_state);

        person->SetUtility(costutil_data[tup].util, util_category);
    }

    void
    GetLiverFibrosisData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string error;
        int rc = dm->SelectCustomCallback(SQLQuery(), callback, &costutil_data,
                                          error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "No cost and/or utility avaliable for Fibrosis "
                "Progression! Error Message: {}",
                error);
        }
        if (costutil_data.empty()) {
            spdlog::get("main")->warn(
                "No cost and/or utility found for Fibrosis Progression.");
        }
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // can only progress in fibrosis state if actively infected with HCV
        if (person->GetHCV() == person::HCV::NONE) {
            return;
        }
        // 1. Get current fibrosis status
        person::FibrosisState fs = person->GetTrueFibrosisState();
        // 2. Get the transition probability
        std::vector<double> prob = getTransition(fs, dm);
        // 3. Draw whether the person's fibrosis state progresses (0
        // progresses)
        (decider->GetDecision(prob) == 0) ? ++fs : fs;

        if (fs != person->GetTrueFibrosisState()) {
            // 4. Apply the result state
            person->UpdateTrueFibrosis(fs);
        }

        // insert Person's liver-related disease cost (taking the highest
        // fibrosis state) only if the person is identified as infected
        if (costFlag && person->IsIdentifiedAsInfected()) {
            this->AddLiverDiseaseCost(person);
        }
        this->SetLiverUtility(person);
    }
    FibrosisProgressionIMPL(
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        this->discount =
            Utils::GetDoubleFromConfig("cost.discounting_rate", dm);

        this->f01_probability = Utils::GetDoubleFromConfig("fibrosis.f01", dm);
        this->f12_probability = Utils::GetDoubleFromConfig("fibrosis.f12", dm);
        this->f23_probability = Utils::GetDoubleFromConfig("fibrosis.f23", dm);
        this->f34_probability = Utils::GetDoubleFromConfig("fibrosis.f34", dm);
        this->f4d_probability = Utils::GetDoubleFromConfig("fibrosis.f4d", dm);

        this->costFlag = Utils::GetBoolFromConfig(
            "fibrosis.add_cost_only_if_identified", dm);

        GetLiverFibrosisData(dm);
    }
};

FibrosisProgression::FibrosisProgression(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<FibrosisProgressionIMPL>(dm);
}

FibrosisProgression::~FibrosisProgression() = default;
FibrosisProgression::FibrosisProgression(FibrosisProgression &&) noexcept =
    default;
FibrosisProgression &
FibrosisProgression::operator=(FibrosisProgression &&) noexcept = default;

void FibrosisProgression::DoEvent(
    std::shared_ptr<person::PersonBase> person,
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
