////////////////////////////////////////////////////////////////////////////////
// File: Overdose.cpp                                                         //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-10-19                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-03-10                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#include "Overdose.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
class Overdose::OverdoseIMPL {
private:
    static int callback_double(void *storage, int count, char **data,
                               char **columns) {
        std::vector<double> *d = (std::vector<double> *)storage;
        double temp = Utils::stod_positive(data[0]);
        d->push_back(temp);
        return 0;
    }
    std::string buildSQL(std::shared_ptr<person::PersonBase> person) const {
        int age_years = person->GetAge() / 12.0; // intentional truncation
        std::stringstream sql;
        sql << "SELECT overdose_probability FROM overdoses";
        sql << " WHERE moud = " << ((int)person->GetMoudState());
        sql << " AND drug_behavior = " << ((int)person->GetBehavior()) << ";";
        return sql.str();
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // if not using, return
        if (person->GetBehavior() != person::Behavior::INJECTION &&
            person->GetBehavior() != person::Behavior::NONINJECTION) {
            return;
        }
        std::string query = buildSQL(person);
        std::vector<double> storage;
        std::string error;
        int rc = dm->SelectCustomCallback(query, this->callback_double,
                                          &storage, error);
        if (rc != 0) {
            spdlog::get("main")->error(
                "Error extracting Overdose Probability! Error Message: {}",
                error);
            return;
        }
        double probability = 0.0;
        if (!storage.empty()) {
            probability = storage[0];
        } else {
            spdlog::get("main")->warn("No Overdose Probability Found!");
        }
        if (decider->GetDecision({probability, 1 - probability}) == 0) {
            person->ToggleOverdose();
        }
    }
};

Overdose::Overdose(std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<OverdoseIMPL>();
}

Overdose::~Overdose() = default;
Overdose::Overdose(Overdose &&) noexcept = default;
Overdose &Overdose::operator=(Overdose &&) noexcept = default;

void Overdose::DoEvent(std::shared_ptr<person::PersonBase> person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}

} // namespace event
