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
#include <DataManagement/DataManager.hpp>
#include <sstream>

namespace event {
    class FibrosisProgression::FibrosisProgressionIMPL {
    private:
        struct fib_prog_select {
            double cost;
        };
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            std::vector<struct fib_prog_select> *d =
                (std::vector<struct fib_prog_select> *)storage;
            struct fib_prog_select temp;
            temp.cost = std::stod(data[0]); // First Column Selected
            d->push_back(temp);
            return 0;
        }
        std::string buildSQL(person::PersonBase &person) {
            std::stringstream sql;
            std::string hcv_status = (person.GetHCV() == person::HCV::NONE)
                                         ? "negative"
                                         : "positive";
            sql << "SELECT cost FROM hcv_costs";
            sql << " WHERE hcv_status = '" << hcv_status << "'";
            sql << " AND metavir_stage = '" << person.GetFibrosisState()
                << "';";
            return sql.str();
        }

        std::vector<double>
        getTransition(person::FibrosisState fs,
                      std::shared_ptr<datamanagement::DataManager> dm) {
            // get the probability of transitioning to the next fibrosis state
            std::string data;

            data.clear();
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
            return {1 - std::stod(data)};
        }

        void
        addLiverDiseaseCost(person::PersonBase &person,
                            std::shared_ptr<datamanagement::DataManager> dm) {
            std::string query = this->buildSQL(person);
            std::vector<struct fib_prog_select> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, callback, &storage, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "No cost avaliable for Fibrosis Progression!");
                return;
            }
            cost::Cost liverDiseaseCost = {cost::CostCategory::LIVER,
                                           "Liver Disease Care",
                                           storage[0].cost};

            person.AddCost(liverDiseaseCost);
        }

    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) {
            std::string data;
            dm->GetFromConfig("fibrosis.add_cost_only_if_identified", data);
            bool costFlag = false;
            if (!data.empty()) {
                std::istringstream(data) >> std::boolalpha >> costFlag;
            }
            // can only progress in fibrosis state if actively infected with HCV
            if (person.GetHCV() == person::HCV::NONE) {
                return;
            }
            // 1. Get current fibrosis status
            person::FibrosisState fs = person.GetFibrosisState();
            // 2. Get the transition probability
            std::vector<double> prob = getTransition(fs, dm);
            // 3. Draw whether the person's fibrosis state progresses
            int res = ((int)fs + decider->GetDecision(prob));
            if (res >= (int)person::FibrosisState::COUNT) {
                spdlog::get("main")->error(
                    "Fibrosis Progression Decision returned "
                    "value outside bounds");
                return;
            }
            person::FibrosisState toFS = (person::FibrosisState)res;
            // 4. Apply the result state
            person.UpdateFibrosis(toFS);

            // insert Person's liver-related disease cost (taking the highest
            // fibrosis state)
            if (costFlag && !person.IsIdentifiedAsInfected()) {
                return;
            }
            this->addLiverDiseaseCost(person, dm);
        }
    };
    FibrosisProgression::FibrosisProgression(
        std::shared_ptr<stats::Decider> decider,
        std::shared_ptr<datamanagement::DataManager> dm, std::string name)
        : Event(dm, name), decider(decider) {
        impl = std::make_unique<FibrosisProgressionIMPL>();
    }

    void FibrosisProgression::doEvent(person::PersonBase &person) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
