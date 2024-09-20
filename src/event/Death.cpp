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
/// This file contains the implementation of the Death Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "Death.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManager.hpp>
#include <sstream>

namespace event {
    class Death::DeathIMPL {
    private:
        struct death_select {
            double back_mort;
            double smr;
        };
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            std::vector<struct death_select> *d =
                (std::vector<struct death_select> *)storage;
            struct death_select temp;
            temp.back_mort = std::stod(data[0]); // First Column Selected
            temp.smr = std::stod(data[1]);       // Second Column Selected
            d->push_back(temp);
            return 0;
        }

        std::string buildSQL(person::PersonBase const &person) const {
            int age_years = person.GetAge() / 12.0; // intentional truncation
            std::stringstream sql;
            sql << "SELECT background_mortality, SMR";
            sql << " FROM SMR ";
            sql << " INNER JOIN background_mortality ON "
                   "(SMR.gender = background_mortality.gender) ";
            sql << " WHERE background_mortality.age_years = '"
                << std::to_string(age_years) << "'";
            sql << " AND background_mortality.gender = '" << person.GetSex()
                << "'";
            sql << " AND SMR.drug_behavior = '" << person.GetBehavior() << "';";

            return sql.str();
        }
        /// @brief The actual death of a person
        /// @param person Person who dies
        void die(person::PersonBase &person, person::DeathReason deathReason) {
            person.Die(deathReason);
        }

        void getFibrosisMortalityProb(
            person::PersonBase const &person,
            std::shared_ptr<datamanagement::DataManager> dm, double &prob) {
            std::string data;
            switch (person.GetFibrosisState()) {
            case person::FibrosisState::F4:
                data.clear();
                dm->GetFromConfig("mortality.f4", data);
                prob = std::stod(data);
                break;
            case person::FibrosisState::DECOMP:
                data.clear();
                dm->GetFromConfig("mortality.decomp", data);
                prob = std::stod(data);
                break;
            default:
                prob = 0;
            }
        }

        void
        getSMRandBackgroundProb(person::PersonBase const &person,
                                std::shared_ptr<datamanagement::DataManager> dm,
                                double &backgroundMortProb, double &smr) {
            std::string query = this->buildSQL(person);
            std::vector<struct death_select> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback, &storage,
                                              error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Death Data from background_mortality and "
                    "SMR! Error Message: {}",
                    error);
                spdlog::get("main")->info("Query: {}", query);
                return;
            }
            if (storage.empty()) {
                // error
                spdlog::get("main")->warn(
                    "Setting background death probability to zero. No Data "
                    "Found for background_mortality and "
                    "SMR with the query: {}",
                    query);
                backgroundMortProb = 0;
                smr = 0;
            } else {
                backgroundMortProb = storage[0].back_mort;
                smr = storage[0].smr;
            }
        }

        bool ReachedMaxAge(person::PersonBase &person) {
            if (person.GetAge() >= 1200) {
                this->die(person, person::DeathReason::AGE);
                return true;
            }
            return false;
        }

    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) {
            if (ReachedMaxAge(person)) {
                return;
            }

            // "Calculate background mortality rate based on age, gender, and
            // IDU"
            double fibrosisDeathProb = 0.0;
            getFibrosisMortalityProb(person, dm, fibrosisDeathProb);

            // 2. Get fatal OD probability.
            double backgroundMortProb = 0.0;
            double smr = 0.0;
            getSMRandBackgroundProb(person, dm, backgroundMortProb, smr);

            // 3. Decide whether the person dies. If not, unset their overdose
            // property.
            double totalProb = (backgroundMortProb * smr) + fibrosisDeathProb;

            std::vector<double> probVec = {(backgroundMortProb * smr),
                                           fibrosisDeathProb, 1 - totalProb};

            int retIdx = decider->GetDecision(probVec);
            if (retIdx == 0) {
                this->die(person, person::DeathReason::BACKGROUND);
            } else if (retIdx == 1) {
                this->die(person, person::DeathReason::LIVER);
            } else {
                // person.ToggleOverdose();
            }
        }
    };

    Death::Death() { impl = std::make_unique<DeathIMPL>(); }

    Death::~Death() = default;
    Death::Death(Death &&) noexcept = default;
    Death &Death::operator=(Death &&) noexcept = default;

    void Death::doEvent(person::PersonBase &person,
                        std::shared_ptr<datamanagement::DataManager> dm,
                        std::shared_ptr<stats::Decider> decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
