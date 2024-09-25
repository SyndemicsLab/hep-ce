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
            std::string age_years =
                std::to_string((int)(person->GetAge() / 12.0));
            sql << "SELECT " << column;
            sql << " FROM screening_and_linkage ";
            sql << "WHERE ((age_years = '" << age_years << "')";
            sql << " AND (gender = '" << person->GetSex() << "')";
            sql << " AND (drug_behavior = '" << person->GetBehavior() << "'));";
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
            // {0: link, 1: don't link}
            std::vector<double> result = {storage[0], 1 - storage[0]};
            return result;
        }

        void addLinkingCost(std::shared_ptr<person::PersonBase> person,
                            std::string name, double cost) {
            cost::Cost linkingCost = {cost::CostCategory::LINKING, name, cost};
            person->AddCost(linkingCost);
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            if (person->GetHCV() == person::HCV::NONE) {
                // add false positive cost
                person->Unlink();
                std::string data;
                dm->GetFromConfig("linking.false_positive_test_cost", data);
                double falsePositiveCost = std::stod(data);
                this->addLinkingCost(person, "False Positive Linking Cost",
                                     falsePositiveCost);
                return;
            }

            if (!person->IsIdentifiedAsHCVInfected()) {
                person->DiagnoseHCV();
            }

            std::vector<double> probs;
            if (person->GetLinkageType() == person::LinkageType::BACKGROUND) {
                // link probability
                probs =
                    getTransitions(person, dm, "background_link_probability");
            } else {
                // add intervention cost
                std::string data;
                dm->GetFromConfig("linking.intervention_cost", data);
                double interventionCost = std::stod(data);
                this->addLinkingCost(person, "Intervention Linking Cost",
                                     interventionCost);
                // link probability
                probs =
                    getTransitions(person, dm, "intervention_link_probability");
            }

            if (person->GetLinkState() == person::LinkageState::UNLINKED) {
                // scale by relink multiplier
                std::string data;
                dm->GetFromConfig("linking.relink_multiplier", data);
                double relinkScalar = std::stod(data);
                probs[1] = probs[1] * relinkScalar;
                probs[0] = 1 - probs[1];
            }

            // draw from link probability
            bool doLink = (decider->GetDecision(probs) == 0) ? true : false;

            if (doLink) {
                // need to figure out how to pass in the LinkageType to the
                // event
                person->Link(person->GetLinkageType());
            } else if (!doLink &&
                       person->GetLinkState() == person::LinkageState::LINKED) {
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
