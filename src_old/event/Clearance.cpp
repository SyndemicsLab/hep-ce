////////////////////////////////////////////////////////////////////////////////
// File: Clearance.cpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-09-13                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Clearance.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include <datamanagement/datamanagement.hpp>

namespace event {
class Clearance::ClearanceIMPL {
private:
    double clearanceProbability;

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 datamanagement::ModelData &model_data,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // if person isn't infected or is chronic, nothing to do
        // Also skip if person is already on treatment since we want this to
        // count as SVR
        if (person->GetHCV() != person::HCV::ACUTE &&
            !person->HasInitiatedTreatment()) {
            return;
        }
        if (decider->GetDecision({clearanceProbability}) == 0) {
            person->ClearHCV(true);
        }
    }
    ClearanceIMPL(datamanagement::ModelData &model_data) {
        std::string data;
        int rc = dm->GetFromConfig("infection.clearance_prob", data);
        if (data.empty()) {
            // it's basically universally accepted that 25% of acute hcv
            // infections clear in the 6-month acute infection period
            this->clearanceProbability = Utils::rateToProbability(0.25) / 6.0;
        } else {
            // if the user provides a clearance probability, use that value
            // instead
            this->clearanceProbability = Utils::stod_positive(data);
        }
    }
};
Clearance::Clearance(datamanagement::ModelData &model_data) {
    impl = std::make_unique<ClearanceIMPL>(dm);
}

Clearance::~Clearance() = default;
Clearance::Clearance(Clearance &&) noexcept = default;
Clearance &Clearance::operator=(Clearance &&) noexcept = default;

void Clearance::DoEvent(std::shared_ptr<person::PersonBase> person,
                        datamanagement::ModelData &model_data,
                        std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
