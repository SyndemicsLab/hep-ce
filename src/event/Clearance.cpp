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
#include <DataManagement/DataManagerBase.hpp>

namespace event {
    class Clearance::ClearanceIMPL {
    private:
        double clearanceProbability;

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            // if person isn't infected or is chronic, nothing to do
            if (person->GetHCV() == person::HCV::ACUTE &&
                decider->GetDecision({clearanceProbability}) == 0) {
                person->ClearHCV();
            }
        }
        ClearanceIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string data;
            int rc = dm->GetFromConfig("infection.clearance_prob", data);
            if (data.empty()) {
                // it's basically universally accepted that 25% of acute hcv
                // infections clear in the 6-month acute infection period
                this->clearanceProbability =
                    Utils::rateToProbability(0.25) / 6.0;
            } else {
                // if the user provides a clearance probability, use that value
                // instead
                this->clearanceProbability = std::stod(data);
            }
        }
    };
    Clearance::Clearance(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<ClearanceIMPL>(dm);
    }

    Clearance::~Clearance() = default;
    Clearance::Clearance(Clearance &&) noexcept = default;
    Clearance &Clearance::operator=(Clearance &&) noexcept = default;

    void Clearance::doEvent(std::shared_ptr<person::PersonBase> person,
                            std::shared_ptr<datamanagement::DataManagerBase> dm,
                            std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
