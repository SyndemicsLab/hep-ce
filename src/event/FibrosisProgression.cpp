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
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class FibrosisProgression::FibrosisProgressionIMPL {
    private:
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            std::vector<double> *d = (std::vector<double> *)storage;
            double temp;
            temp = std::stod(data[0]); // First Column Selected
            d->push_back(temp);
            return 0;
        }
        std::string buildSQL(std::shared_ptr<person::PersonBase> person) {
            std::stringstream sql;
            int hcv_status = (person->GetHCV() == person::HCV::NONE) ? 0 : 1;
            sql << "SELECT cost FROM hcv_impacts";
            sql << " WHERE hcv_status = " << hcv_status;
            sql << " AND fibrosis_state = "
                << ((int)person->GetTrueFibrosisState()) << ";";
            return sql.str();
        }

        std::vector<double>
        getTransition(person::FibrosisState fs,
                      std::shared_ptr<datamanagement::DataManagerBase> dm) {
            // get the probability of transitioning to the next fibrosis state
            std::string data;

            switch (fs) {
            case person::FibrosisState::F0:
                dm->GetFromConfig("fibrosis.f01", data);
                break;
            case person::FibrosisState::F1:
                dm->GetFromConfig("fibrosis.f12", data);
                break;
            case person::FibrosisState::F2:
                dm->GetFromConfig("fibrosis.f23", data);
                break;
            case person::FibrosisState::F3:
                dm->GetFromConfig("fibrosis.f34", data);
                break;
            case person::FibrosisState::F4:
                dm->GetFromConfig("fibrosis.f4d", data);
                break;
            case person::FibrosisState::DECOMP:
                data = "0.0";
                break;
            }

            // to work with getDecision, return the complement to the transition
            // probability
            return {std::stod(data), 1 - std::stod(data)};
        }

        void addLiverDiseaseCost(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string query = this->buildSQL(person);
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, callback, &storage, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "No cost avaliable for Fibrosis Progression!");
                return;
            }
            double c = 0.00;
            if (!storage.empty()) {
                c = storage[0];
            } else {
                spdlog::get("main")->warn(
                    "No cost Found for Fibrosis Progression, setting cost to "
                    "$0.00.");
            }

            cost::Cost liverDiseaseCost = {cost::CostCategory::LIVER,
                                           "Liver Disease Care", c};

            person->AddCost(liverDiseaseCost);
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            std::string data;
            dm->GetFromConfig("fibrosis.add_cost_only_if_identified", data);
            bool costFlag = false;
            if (!data.empty()) {
                std::istringstream(data) >> std::boolalpha >> costFlag;
            }
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
                this->addLiverDiseaseCost(person, dm);
            }
        }
    };

    FibrosisProgression::FibrosisProgression() {
        impl = std::make_unique<FibrosisProgressionIMPL>();
    }

    FibrosisProgression::~FibrosisProgression() = default;
    FibrosisProgression::FibrosisProgression(FibrosisProgression &&) noexcept =
        default;
    FibrosisProgression &
    FibrosisProgression::operator=(FibrosisProgression &&) noexcept = default;

    void FibrosisProgression::doEvent(
        std::shared_ptr<person::PersonBase> person,
        std::shared_ptr<datamanagement::DataManagerBase> dm,
        std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
