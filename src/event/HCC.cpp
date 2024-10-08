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
/// This file contains the implementation of the HCC Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "HCC.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class HCC::HCCIMPL {
    private:
    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            person::FibrosisState current = person->GetTrueFibrosisState();
            if (current != person::FibrosisState::F3 &&
                current != person::FibrosisState::F4 &&
                current != person::FibrosisState::DECOMP) {
                return;
            }
            // Decide if person->DevelopHCC()
            person::HCCState currentHCC = person->GetHCCState();
            if (currentHCC == person::HCCState::NONE) {
                // Decide if person develops early HCC based on probability
            } else if (currentHCC == person::HCCState::EARLY) {
                // Decide if person develops late HCC based on probability
            }

            if (!person->IsDiagnosedWithHCC()) {
                // Decide if we should diagnose them
            }
        }
    };
    HCC::HCC(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<HCCIMPL>();
    }

    HCC::~HCC() = default;
    HCC::HCC(HCC &&) noexcept = default;
    HCC &HCC::operator=(HCC &&) noexcept = default;

    void HCC::doEvent(std::shared_ptr<person::PersonBase> person,
                      std::shared_ptr<datamanagement::DataManagerBase> dm,
                      std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
