//===-------------------------------*- C++ -*------------------------------===//
//-*-===//
//
// Part of the HEP-CE Simulation Module, under the AGPLv3 License. See
// https://www.gnu.org/licenses/ for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the implementation of the FibrosisProgression Event
/// Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "FibrosisProgression.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class FibrosisProgression::FibrosisProgressionIMPL {
    private:
        double discount = 0.0;
        double f01_probability;
        double f12_probability;
        double f23_probability;
        double f34_probability;
        double f4d_probability;
        bool costFlag = false;

        using costmap_t =
            std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                               Utils::key_equal_2i>;
        costmap_t cost_data;

        std::string CostSQL() {
            return "SELECT hcv_status, fibrosis_state, cost FROM hcv_impacts;";
        }

        static int callback_cost(void *storage, int count, char **data,
                                 char **columns) {
            Utils::tuple_2i tup =
                std::make_tuple(std::stoi(data[0]), std::stoi(data[1]));
            (*((costmap_t *)storage))[tup] = std::stod(data[2]);
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

        void AddLiverDiseaseCost(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            int hcv_status = (person->GetHCV() == person::HCV::NONE) ? 0 : 1;
            int fibrosis_state = (int)person->GetTrueFibrosisState();
            Utils::tuple_2i tup = std::make_tuple(hcv_status, fibrosis_state);

            double discountAdjustedCost = Event::DiscountEventCost(
                cost_data[tup], discount, person->GetCurrentTimestep());
            person->AddCost(cost_data[tup], discountAdjustedCost,
                            cost::CostCategory::LIVER);
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

            if (fs == person->GetTrueFibrosisState()) {
                return;
            }

            // 4. Apply the result state
            person->UpdateTrueFibrosis(fs);
            // insert Person's liver-related disease cost (taking the highest
            // fibrosis state)
            if (!costFlag ||
                (costFlag && person->IsIdentifiedAsHCVInfected())) {
                this->AddLiverDiseaseCost(person, dm);
            }
        }
        FibrosisProgressionIMPL(
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string discount_data;
            int rc = dm->GetFromConfig("cost.discounting_rate", discount_data);
            if (!discount_data.empty()) {
                this->discount = std::stod(discount_data);
            }
            std::string data;

            dm->GetFromConfig("fibrosis.f01", data);
            this->f01_probability = std::stod(data);

            dm->GetFromConfig("fibrosis.f12", data);
            this->f12_probability = std::stod(data);

            dm->GetFromConfig("fibrosis.f23", data);
            this->f23_probability = std::stod(data);

            dm->GetFromConfig("fibrosis.f34", data);
            this->f34_probability = std::stod(data);

            dm->GetFromConfig("fibrosis.f4d", data);
            this->f4d_probability = std::stod(data);

            dm->GetFromConfig("fibrosis.add_cost_only_if_identified", data);
            if (!data.empty()) {
                std::istringstream(data) >> std::boolalpha >> costFlag;
            }

            std::string error;
            rc = dm->SelectCustomCallback(CostSQL(), callback_cost, &cost_data,
                                          error);
            if (rc != 0) {
                spdlog::get("main")->error("No cost avaliable for Fibrosis "
                                           "Progression! Error Message: {}",
                                           error);
            }
            if (cost_data.empty()) {
                spdlog::get("main")->warn(
                    "No cost Found for Fibrosis Progression.");
            }
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
