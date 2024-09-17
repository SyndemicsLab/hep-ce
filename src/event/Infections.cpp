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
/// This file contains the implementation of the Infections Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Infections.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManager.hpp>
#include <sstream>
namespace event {
    class Infections::InfectionsIMPL {
    private:
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            std::vector<double> *d = (std::vector<double> *)storage;
            double temp = std::stod(data[0]);
            d->push_back(temp);
            return 0;
        }

        std::string buildSQL(person::PersonBase &person) {
            std::stringstream sql;
            std::string age_years =
                std::to_string((int)(person.GetAge() / 12.0));
            sql << "SELECT incidence FROM incidence ";
            sql << "WHERE age_years = '" << age_years << "'";
            sql << " AND gender = '" << person.GetSex() << "'";
            sql << " AND drug_behavior = '" << person.GetBehavior() << "';";
            return sql.str();
        }

        std::vector<double>
        getInfectProb(person::PersonBase &person,
                      std::shared_ptr<datamanagement::DataManager> dm) {
            std::string query = this->buildSQL(person);
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback, &storage,
                                              error);
            if (rc != 0) {
                spdlog::get("main")->error("No Transitions Avaliable for "
                                           "Behavior Change! Error Message: "
                                           "{}",
                                           error);
                spdlog::get("main")->info("Query: {}", query);
                return {};
            }
            std::vector<double> result = {storage[0], 1 - storage[0]};
            return result;
        }

    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) {
            // only those who aren't infected go to the rest of the event.
            // those who are infected transition from acute to chronic after 6
            // months.
            switch (person.GetHCV()) {
            case person::HCV::NONE:
                break;
            case person::HCV::ACUTE:
                if (person.GetTimeSinceHCVChanged() >= 6) {
                    person.SetHCV(person::HCV::CHRONIC);
                }
                return;
            case person::HCV::CHRONIC:
                return;
            }

            // draw new infection probability
            std::vector<double> prob = this->getInfectProb(person, dm);
            // decide whether person is infected; if value == 0, infect
            int value = decider->GetDecision(prob);
            if (value != 0) {
                return;
            }
            person.Infect();
        }
    };
    Infections::Infections(std::shared_ptr<stats::Decider> decider,
                           std::shared_ptr<datamanagement::DataManager> dm,
                           std::string name)
        : Event(dm, name), decider(decider) {
        impl = std::make_unique<InfectionsIMPL>();
    }

    void Infections::doEvent(person::PersonBase &person) {
        impl->doEvent(person, dm, decider);
    }

} // namespace event
