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
/// This file contains the implementation of the Clearance Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Clearance.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include <DataManagement/DataManager.hpp>

namespace event {
    class Clearance::ClearanceIMPL {
    private:
    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) {
            std::string data;
            int rc = dm->GetFromConfig("infection.clearance_prob", data);
            double clearanceProbability = 0.0;
            if (data.empty()) {
                // it's basically universally accepted that 25% of acute hcv
                // infections clear in the 6-month acute infection period
                clearanceProbability = Utils::probabilityToRate(0.25) / 6.0;
            } else {
                // if the user provides a clearance probability, use that value
                // instead
                clearanceProbability = std::stod(data);
            }

            // people infected with hcv have some probability of spontaneous
            // clearance.

            // if person isn't infected or is chronic, nothing to do
            if (person.GetHCV() != person::HCV::ACUTE) {
                return;
            }
            // 1. Get the probability of acute clearance
            std::vector<double> prob = {clearanceProbability};
            // 2. Decide whether the person clears
            int doesNotClear = decider->GetDecision(prob);
            // if you do not clear, return immediately
            if (doesNotClear) {
                return;
            }
            person.ClearHCV();
        }
    };
    Clearance::Clearance() { impl = std::make_unique<ClearanceIMPL>(); }

    Clearance::~Clearance() = default;
    Clearance::Clearance(Clearance &&) noexcept = default;
    Clearance &Clearance::operator=(Clearance &&) noexcept = default;

    void Clearance::doEvent(person::PersonBase &person,
                            std::shared_ptr<datamanagement::DataManager> dm,
                            std::shared_ptr<stats::Decider> decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
