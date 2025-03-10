////////////////////////////////////////////////////////////////////////////////
// File: HCC.cpp                                                              //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

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
    void DoEvent(std::shared_ptr<person::PersonBase> person,
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

void HCC::DoEvent(std::shared_ptr<person::PersonBase> person,
                  std::shared_ptr<datamanagement::DataManagerBase> dm,
                  std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
