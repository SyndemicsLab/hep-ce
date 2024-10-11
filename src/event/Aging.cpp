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
/// This file contains the implementation of the Aging Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Aging.hpp"
#include "Cost.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>
#include <tuple>
#include <unordered_map>

namespace event {
    class Aging::AgingIMPL {
    private:
        struct cost_util {
            double cost;
            double util;
        };

        typedef std::unordered_map<Utils::tuple_3i, struct cost_util,
                                   Utils::key_hash_3i, Utils::key_equal_3i>
            agemap_t;
        agemap_t data;

        static int callback(void *storage, int count, char **data,
                            char **columns) {
            Utils::tuple_3i tup = std::make_tuple(
                std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
            struct cost_util cu = {std::stod(data[3]), std::stod(data[4])};
            (*((agemap_t *)storage))[tup] = cu;
            return 0;
        }

        std::string buildSQL() const {
            return "SELECT age_years, gender, drug_behavior, cost, utility "
                   "FROM background_impacts;";
        }

        /// @brief Adds person's background cost
        /// @param person The person to whom cost will be added
        void addBackgroundCostAndUtility(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            int age_years = (int)(person->GetAge() / 12.0);
            int gender = ((int)person->GetSex());
            int behavior = ((int)person->GetBehavior());
            Utils::tuple_3i tup = std::make_tuple(age_years, gender, behavior);

            cost::Cost backgroundCost = {cost::CostCategory::MISC,
                                         "Background Cost", data[tup].cost};
            person->AddCost(backgroundCost);
            person->SetUtility(data[tup].util);
        }

        int LoadData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string query = this->buildSQL();
            std::string error;
            int rc =
                dm->SelectCustomCallback(query, this->callback, &data, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Aging Data from background costs and "
                    "background behaviors! Error Message: {}",
                    error);
            }
            if (data.empty()) {
                spdlog::get("main")->warn(
                    "No Background Cost found for Aging!");
            }
            return rc;
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm) {
            person->Grow();
            this->addBackgroundCostAndUtility(person, dm);
        }
        AgingIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            data = {};
            if (dm == nullptr) {
                spdlog::get("main")->warn(
                    "No Data Manager Provided during Construction. No Data "
                    "Loaded to Aging.");
                return;
            }
            int rc = LoadData(dm);
        }
    };

    Aging::Aging(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<AgingIMPL>(dm);
    }

    Aging::~Aging() = default;
    Aging::Aging(Aging &&) noexcept = default;
    Aging &Aging::operator=(Aging &&) noexcept = default;

    void Aging::doEvent(std::shared_ptr<person::PersonBase> person,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm);
    }

} // namespace event
