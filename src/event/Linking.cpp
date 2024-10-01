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
/// This file contains the implementation of the Linking Event
/// Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Linking.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class Linking::LinkingIMPL {
    private:
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            std::vector<double> *d = (std::vector<double> *)storage;
            double temp = std::stod(data[0]);
            d->push_back(temp);
            return 0;
        }
        std::string buildSQL(std::shared_ptr<person::PersonBase> person,
                             std::string const column) const {
            std::stringstream sql;
            int age_years = (int)(person->GetAge() / 12.0);
            sql << "SELECT " << column;
            sql << " FROM screening_and_linkage ";
            sql << "WHERE age_years = " << age_years;
            sql << " AND gender = " << ((int)person->GetSex());
            sql << " AND drug_behavior = " << ((int)person->GetBehavior())
                << ";";
            return sql.str();
        }

        std::vector<double>
        getTransitions(std::shared_ptr<person::PersonBase> person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       std::string columnKey) {
            std::unordered_map<std::string, std::string> selectCriteria;

            std::string query = this->buildSQL(person, columnKey);
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback, &storage,
                                              error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Linking Data from screening_and_linkage! "
                    "Error Message: {}",
                    error);
                return {};
            }
            std::vector<double> result;
            if (storage.empty()) {
                spdlog::get("main")->warn("No Values found for Linking Data!");
                result = {0.0, 1.0};
            } else {
                // {0: link, 1: don't link}
                result = {storage[0], 1 - storage[0]};
            }

            return result;
        }

        void addLinkingCost(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::string name) {
            std::string param = "";
            if (name == "Intervention Linking Cost") {
                param = "linking.intervention_cost";
            } else if (name == "False Positive Linking Cost") {
                param = "linking.false_positive_test_cost";
            } else {
                return;
            }

            std::string data = "";
            dm->GetFromConfig(param, data);
            if (data.empty()) {
                spdlog::get("main")->warn("{} Not Found! Assuming "
                                          "0.00...",
                                          param);
                data = "0.00";
            }
            double cost = std::stod(data);
            cost::Cost linkingCost = {cost::CostCategory::LINKING, name, cost};
            person->AddCost(linkingCost);
        }

        bool
        FalsePositive(std::shared_ptr<person::PersonBase> person,
                      std::shared_ptr<datamanagement::DataManagerBase> dm) {
            if (person->GetHCV() != person::HCV::NONE) {
                return false;
            }
            person->Unlink();
            this->addLinkingCost(person, dm, "False Positive Linking Cost");
            return true;
        }

        /// @brief Helper Function to Scale the probabilities if the person is
        /// relinking
        /// @param person Person Under Study
        /// @param dm Data Manager Object
        /// @param probabilities Linking Probabilities (0: link, 1: unlink)
        void ScaleIfRelink(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::vector<double> &probabilities) {
            if (person->GetLinkState() != person::LinkageState::UNLINKED) {
                return; // Not Relinking
            }
            std::string data;
            dm->GetFromConfig("linking.relink_multiplier", data);
            double relinkScalar = std::stod(data);
            probabilities[1] = probabilities[1] * relinkScalar;
            probabilities[0] = 1 - probabilities[1];
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            if (FalsePositive(person, dm)) {
                return;
            }

            if (!person->IsIdentifiedAsHCVInfected()) {
                person->DiagnoseHCV();
            }

            std::vector<double> probs;
            probs =
                (person->GetLinkageType() == person::LinkageType::BACKGROUND)
                    ? getTransitions(person, dm, "background_link_probability")
                    : getTransitions(person, dm,
                                     "intervention_link_probability");

            ScaleIfRelink(person, dm, probs);

            // draw from link probability
            if (decider->GetDecision(probs) == 0) {
                person->Link(person->GetLinkageType());
                if (person->GetLinkageType() ==
                    person::LinkageType::INTERVENTION) {
                    this->addLinkingCost(person, dm,
                                         "Intervention Linking Cost");
                }

            } else if (person->GetLinkState() == person::LinkageState::LINKED) {
                person->Unlink();
            }
        }
    };
    Linking::Linking() { impl = std::make_unique<LinkingIMPL>(); }

    Linking::~Linking() = default;
    Linking::Linking(Linking &&) noexcept = default;
    Linking &Linking::operator=(Linking &&) noexcept = default;

    void Linking::doEvent(std::shared_ptr<person::PersonBase> person,
                          std::shared_ptr<datamanagement::DataManagerBase> dm,
                          std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
