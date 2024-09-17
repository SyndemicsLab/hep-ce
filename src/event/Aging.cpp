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
#include "spdlog/spdlog.h"
#include <DataManagement/DataManager.hpp>
#include <sstream>

namespace event {
    class Aging::AgingIMPL {
    private:
        struct cost_util {
            double cost;
            double util;
        };
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            struct cost_util temp;
            for (int idx = 0; idx < count; idx++) {
                spdlog::get("main")->info("The data in column {} is: {}",
                                          columns[idx], data[idx]);
            }
            temp.cost = std::stod(data[0]); // First Column Selected
            temp.util = std::stod(data[1]); // Second Column Selected
            ((std::vector<struct cost_util> *)storage)->push_back(temp);
            return 0;
        }

        std::string buildSQL(person::PersonBase const person) const {
            std::string a = std::to_string((int)(person.GetAge() / 12.0));
            std::stringstream sql;
            sql << "SELECT cost, utility FROM background_costs ";
            sql << "INNER JOIN background_utilities ON "
                   "((background_costs.age_years = "
                   "background_utilities.age_years) AND "
                   "(background_costs.gender = background_utilities.gender) "
                   "AND (background_costs.drug_behavior = "
                   "background_utilities.drug_behavior)) ";
            sql << "WHERE background_costs.age_years = '" << a << "'"
                << " AND background_costs.gender = '" << person.GetSex() << "'"
                << " AND background_costs.drug_behavior = '"
                << person.GetBehavior() << "';";
            return sql.str();
        }

        /// @brief Adds person's background cost
        /// @param person The person to whom cost will be added
        void addBackgroundCostAndUtility(
            person::PersonBase &person,
            std::shared_ptr<datamanagement::DataManager> dm) {
            std::string query = this->buildSQL(person);
            std::vector<struct cost_util> storage = {};
            std::string error;
            if (dm == nullptr) {
                // Let default values stay
                return;
            }
            int rc = dm->SelectCustomCallback(query, this->callback, &storage,
                                              error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Aging Data from background costs and "
                    "background behaviors! Error Message: {}",
                    error);
                // spdlog::get("main")->info("Query: {}", query);
                return;
            }
            cost::Cost backgroundCost = {cost::CostCategory::MISC,
                                         "Background Cost", storage[0].cost};
            person.AddCost(backgroundCost);
            person.SetUtility(storage[0].util);
        }

    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm) {
            person.Grow();
            this->addBackgroundCostAndUtility(person, dm);
        }
    };

    Aging::Aging(std::shared_ptr<stats::Decider> decider,
                 std::shared_ptr<datamanagement::DataManager> dm,
                 std::string name)
        : Event(dm, name) {
        impl = std::make_unique<AgingIMPL>();
    }

    void Aging::doEvent(person::PersonBase &person) {
        impl->doEvent(person, dm);
    }

} // namespace event
