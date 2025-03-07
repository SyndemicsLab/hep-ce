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
/// This file contains the implementation of the VoluntaryRelinking Event
/// Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "VoluntaryRelinking.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <string>

namespace event {
class VoluntaryRelinking::VoluntaryRelinkingIMPL {
private:
    double relink_probability;
    int voluntary_relink_duration;
    double background_rna_cost;
    double discount = 0.0;

    /// @brief During a voluntary relink, a new RNA test is run on the
    /// person. This has no impact outside of costs to the person.
    /// @param person person who is being relinked
    void AddRNATest(std::shared_ptr<person::PersonBase> person) {
        person->AddRnaScreen();
        double discountAdjustedCost = Event::DiscountEventCost(
            background_rna_cost, discount, person->GetCurrentTimestep());
        person->AddCost(background_rna_cost, discountAdjustedCost,
                        cost::CostCategory::SCREENING);
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // if linked or never linked OR too long since last linked
        if ((person->GetLinkState() == person::LinkageState::UNLINKED) &&
            ((person->GetTimeSinceLinkChange()) < voluntary_relink_duration) &&
            (decider->GetDecision({relink_probability}) == 0) &&
            (person->GetHCV() != person::HCV::NONE)) {
            AddRNATest(person);
            person->Link(person::LinkageType::BACKGROUND);
        }
    }

    VoluntaryRelinkingIMPL(
        std::shared_ptr<datamanagement::DataManagerBase> dm) {
        std::string discount_data;
        int rc = dm->GetFromConfig("cost.discounting_rate", discount_data);
        if (!discount_data.empty()) {
            this->discount = Utils::stod_positive(discount_data);
        }

        std::string data;
        dm->GetFromConfig("linking.voluntary_relinkage_probability", data);
        if (data.empty()) {
            spdlog::get("main")->warn(
                "No Voluntary Relinkage Probability Found!");
            data = "0.00";
        }
        relink_probability = Utils::stod_positive(data);

        dm->GetFromConfig("linking.voluntary_relink_duration", data);
        if (data.empty()) {
            spdlog::get("main")->warn("No Voluntary Relinkage Duration Found!");
            data = "0";
        }
        voluntary_relink_duration = std::stoi(data);

        dm->GetFromConfig("screening_background_rna.cost", data);
        background_rna_cost = Utils::stod_positive(data);
    }
};

VoluntaryRelinking::VoluntaryRelinking(
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<VoluntaryRelinkingIMPL>(dm);
}

VoluntaryRelinking::~VoluntaryRelinking() = default;

VoluntaryRelinking::VoluntaryRelinking(VoluntaryRelinking &&) noexcept =
    default;
VoluntaryRelinking &
VoluntaryRelinking::operator=(VoluntaryRelinking &&) noexcept = default;

void VoluntaryRelinking::DoEvent(
    std::shared_ptr<person::PersonBase> person,
    std::shared_ptr<datamanagement::DataManagerBase> dm,
    std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
