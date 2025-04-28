////////////////////////////////////////////////////////////////////////////////
// File: HCC.cpp                                                              //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-06                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "HCC.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <datamanagement/datamanagement.hpp>
#include <sstream>

namespace event {
class HCC::HCCIMPL {
private:
public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) {
        person::FibrosisState current = person->GetTrueFibrosisState();
        if (current != person::FibrosisState::kF3 &&
            current != person::FibrosisState::kF4 &&
            current != person::FibrosisState::kDecomp) {
            return;
        }
        // Decide if person->DevelopHCC()
        person::HCCState currentHCC = person->GetHCCState();
        if (currentHCC == person::HCCState::kNone) {
            // Decide if person develops early HCC based on probability
        } else if (currentHCC == person::HCCState::kEarly) {
            // Decide if person develops late HCC based on probability
        }

        if (!person->IsDiagnosedWithHCC()) {
            // Decide if we should diagnose them
        }
    }
};
HCC::HCC(datamanagement::ModelData &model_data) {
    impl = std::make_unique<HCCIMPL>();
}

HCC::~HCC() = default;
HCC::HCC(HCC &&) noexcept = default;
HCC &HCC::operator=(HCC &&) noexcept = default;

void HCC::DoEvent(std::shared_ptr<person::PersonBase> person,
                  datamanagement::ModelData &model_data,
                  std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
