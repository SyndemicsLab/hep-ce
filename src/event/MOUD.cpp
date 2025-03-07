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
/// This file contains the implementation of the MOUD Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "MOUD.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
class MOUD::MOUDIMPL {
private:
    static int callback_double(void *storage, int count, char **data,
                               char **columns) {
        std::vector<double> *d = (std::vector<double> *)storage;
        double temp = Utils::stod_positive(data[0]);
        d->push_back(temp);
        return 0;
    }
    std::string buildSQL(std::shared_ptr<person::PersonBase> person) {
        return "SELECT current_moud, age, pregnant, transition_probability "
               "FROM moud;";
        // std::stringstream sql;
        // sql << "SELECT transition_probability FROM moud";
        // sql << " WHERE current_moud = " << ((int)person->GetMoudState());
        // sql << " AND age = " << ((int)(person->GetAge() / 12.0));
        // sql << " AND pregant = " << ((int)(person->GetPregnancyState()));
        // return sql.str();
    }

public:
    void DoEvent(std::shared_ptr<person::PersonBase> person,
                 std::shared_ptr<datamanagement::DataManagerBase> dm,
                 std::shared_ptr<stats::DeciderBase> decider) {
        // if the person has never used, no MOUD change
        if (person->GetBehavior() == person::Behavior::NEVER) {
            return;
        }
        std::string query = buildSQL(person);
        std::vector<double> storage;
        std::string error;
        int rc = dm->SelectCustomCallback(query, this->callback_double,
                                          &storage, error);
        if (rc != 0) {
            spdlog::get("main")->error("Error extracting MOUD Transition "
                                       "Probability! Error Message: {}",
                                       error);
            return;
        }
        double probability = 0.0;
        if (!storage.empty()) {
            probability = storage[0];
        } else {
            spdlog::get("main")->warn("No MOUD Transition Probability Found!");
        }
        if (decider->GetDecision({probability, 1 - probability}) == 0) {
            person->TransitionMOUD();
        }
    }
};
MOUD::MOUD(std::shared_ptr<datamanagement::DataManagerBase> dm) {
    impl = std::make_unique<MOUDIMPL>();
}

MOUD::~MOUD() = default;
MOUD::MOUD(MOUD &&) noexcept = default;
MOUD &MOUD::operator=(MOUD &&) noexcept = default;

void MOUD::DoEvent(std::shared_ptr<person::PersonBase> person,
                   std::shared_ptr<datamanagement::DataManagerBase> dm,
                   std::shared_ptr<stats::DeciderBase> decider) {
    impl->DoEvent(person, dm, decider);
}
} // namespace event
