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
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class Linking::LinkingIMPL {
    private:
        double discount = 0.0;
        double intervention_cost;
        double false_positive_test_cost;
        double relink_multiplier;

        typedef std::unordered_map<Utils::tuple_3i, double, Utils::key_hash_3i,
                                   Utils::key_equal_3i>
            linkmap_t;
        linkmap_t background_link_data;
        linkmap_t intervention_link_data;

        static int callback_link(void *storage, int count, char **data,
                                 char **columns) {
            Utils::tuple_3i tup = std::make_tuple(
                std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
            (*((linkmap_t *)storage))[tup] = std::stod(data[3]);
            return 0;
        }
        std::string LinkSQL(std::string const column) const {
            return "SELECT age_years, gender, drug_behavior, " + column +
                   " FROM screening_and_linkage;";
        }

        double
        getLinkProbability(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::string columnKey) {
            int age_years = (int)(person->GetAge() / 12.0);
            int gender = (int)person->GetSex();
            int drug_behavior = (int)person->GetBehavior();
            Utils::tuple_3i tup =
                std::make_tuple(age_years, gender, drug_behavior);
            if (columnKey == "background_link_probability") {
                return background_link_data[tup];
            } else if (columnKey == "intervention_link_probability") {
                return intervention_link_data[tup];
            }
            return 0.0;
        }

        void addLinkingCost(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::string name) {
            double cost;
            if (name == "Intervention Linking Cost") {
                cost = intervention_cost;
            } else if (name == "False Positive Linking Cost") {
                cost = false_positive_test_cost;
            } else {
                return;
            }

            double discountAdjustedCost = Event::DiscountEventCost(
                cost, discount, person->GetCurrentTimestep());
            person->AddCost(discountAdjustedCost, cost::CostCategory::LINKING);
        }

        bool
        FalsePositive(std::shared_ptr<person::PersonBase> person,
                      std::shared_ptr<datamanagement::DataManagerBase> dm) {
            if (person->GetHCV() == person::HCV::NONE) {
                // person->Unlink();
                this->addLinkingCost(person, dm, "False Positive Linking Cost");
                return true;
            }
            return false;
        }

        double ParseDoublesFromConfig(
            std::string configKey,
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string data;
            dm->GetFromConfig(configKey, data);
            if (data.empty()) {
                spdlog::get("main")->warn("No {} Found!", configKey);
                data = "0.0";
            }
            return std::stod(data);
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            // If the person is already linked, is not identifed as infected,
            // has been longer than one month since screening, or a false
            // positive, exit the event
            if (person->GetLinkState() == person::LinkageState::LINKED ||
                (!person->IsIdentifiedAsHCVInfected()) ||
                (person->GetTimeSinceLastScreening() > 0) ||
                FalsePositive(person, dm)) {
                return;
            }

            double prob =
                (person->GetLinkageType() == person::LinkageType::BACKGROUND)
                    ? getLinkProbability(person, dm,
                                         "background_link_probability")
                    : getLinkProbability(person, dm,
                                         "intervention_link_probability");

            // draw from link probability
            if (decider->GetDecision({prob}) == 0) {
                person->Link(person->GetLinkageType());
                if (person->GetLinkageType() ==
                    person::LinkageType::INTERVENTION) {
                    this->addLinkingCost(person, dm,
                                         "Intervention Linking Cost");
                }
            }
        }
        LinkingIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string discount_data;
            int rc = dm->GetFromConfig("cost.discounting_rate", discount_data);
            if (discount_data.empty()) {
                this->discount = std::stod(discount_data);
            }
            intervention_cost =
                ParseDoublesFromConfig("linking.intervention_cost", dm);

            false_positive_test_cost =
                ParseDoublesFromConfig("linking.false_positive_test_cost", dm);

            std::string error;
            rc = dm->SelectCustomCallback(
                LinkSQL("background_link_probability"), this->callback_link,
                &background_link_data, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Background Linking Data from "
                    "screening_and_linkage! "
                    "Error Message: {}",
                    error);
            }

            rc = dm->SelectCustomCallback(
                LinkSQL("intervention_link_probability"), this->callback_link,
                &intervention_link_data, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Intervention Linking Data from "
                    "screening_and_linkage! "
                    "Error Message: {}",
                    error);
            }
        }
    };
    Linking::Linking(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<LinkingIMPL>(dm);
    }

    Linking::~Linking() = default;
    Linking::Linking(Linking &&) noexcept = default;
    Linking &Linking::operator=(Linking &&) noexcept = default;

    void Linking::doEvent(std::shared_ptr<person::PersonBase> person,
                          std::shared_ptr<datamanagement::DataManagerBase> dm,
                          std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
