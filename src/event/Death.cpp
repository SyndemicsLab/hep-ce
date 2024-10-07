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
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class Death::DeathIMPL {
    private:
        double f4_probability;
        double decomp_probability;
        struct background_smr {
            double back_mort = 0.0;
            double smr = 0.0;
        };
        static int callback(void *storage, int count, char **data,
                            char **columns) {
            std::vector<struct background_smr> *d =
                (std::vector<struct background_smr> *)storage;
            struct background_smr temp;
            temp.back_mort = std::stod(data[0]); // First Column Selected
            temp.smr = std::stod(data[1]);       // Second Column Selected
            d->push_back(temp);
            return 0;
        }
        static int callback_double(void *storage, int count, char **data,
                                   char **columns) {
            std::vector<double> *d = (std::vector<double> *)storage;
            double temp = std::stod(data[0]);
            d->push_back(temp);
            return 0;
        }

        std::string buildSQL(std::shared_ptr<person::PersonBase> person) const {
            int age_years = person->GetAge() / 12.0; // intentional truncation
            std::stringstream sql;
            sql << "SELECT background_mortality, smr";
            sql << " FROM smr ";
            sql << " INNER JOIN background_mortality ON "
                   "(smr.gender = background_mortality.gender) ";
            sql << " WHERE background_mortality.age_years = " << age_years;
            sql << " AND background_mortality.gender = "
                << ((int)person->GetSex());
            sql << " AND SMR.drug_behavior = " << ((int)person->GetBehavior())
                << ";";

            return sql.str();
        }

        std::string
        buildOverdoseSQL(std::shared_ptr<person::PersonBase> person) const {
            int age_years = person->GetAge() / 12.0; // intentional truncation
            std::stringstream sql;
            sql << "SELECT fatality_probability FROM overdoses";
            sql << " WHERE moud = " << ((int)person->GetMoudState());
            sql << " AND drug_behavior = " << ((int)person->GetBehavior())
                << ";";
            return sql.str();
        }

        /// @brief The actual death of a person
        /// @param person Person who dies
        void die(std::shared_ptr<person::PersonBase> person,
                 person::DeathReason deathReason) {
            person->Die(deathReason);
        }

        void getFibrosisMortalityProb(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm, double &prob) {
            std::string data;
            switch (person->GetTrueFibrosisState()) {
            case person::FibrosisState::F4:
                prob = f4_probability;
                break;
            case person::FibrosisState::DECOMP:
                prob = decomp_probability;
                break;
            default:
                prob = 0;
            }
        }

        void getSMRandBackgroundProb(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm,
            double &backgroundMortProb, double &smr) {
            std::string query = this->buildSQL(person);
            std::vector<struct background_smr> storage;
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

        bool ReachedMaxAge(std::shared_ptr<person::PersonBase> person) {
            if (person->GetAge() >= 1200) {
                this->die(person, person::DeathReason::AGE);
                return true;
            }
            return false;
        }

        bool FatalOverdose(std::shared_ptr<person::PersonBase> person,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           std::shared_ptr<stats::DeciderBase> decider) {
            if (!person->GetCurrentlyOverdosing()) {
                return false;
            }

            std::string query = buildOverdoseSQL(person);
            std::vector<double> storage;
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_double,
                                              &storage, error);
            if (rc != 0) {
                spdlog::get("main")->error("Error extracting Fatal Overdose "
                                           "Probability! Error Message: {}",
                                           error);
                return false;
            }
            double probability = 0.0;
            if (!storage.empty()) {
                probability = storage[0];
            } else {
                spdlog::get("main")->warn(
                    "No Fatal Overdose Probability Found!");
            }
            if (decider->GetDecision({probability, 1 - probability}) != 0) {
                person->ToggleOverdose();
                return false;
            }
            this->die(person, person::DeathReason::OVERDOSE);
            return true;
        }

    public:
        void doEvent(std::shared_ptr<person::PersonBase> person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::shared_ptr<stats::DeciderBase> decider) {
            if (ReachedMaxAge(person)) {
                return;
            }
            if (FatalOverdose(person, dm, decider)) {
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
            }
        }
        DeathIMPL(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string data;
            data.clear();
            dm->GetFromConfig("mortality.f4", data);
            this->f4_probability = std::stod(data);

            data.clear();
            dm->GetFromConfig("mortality.decomp", data);
            this->decomp_probability = std::stod(data);
        }
    };

    Death::Death(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<DeathIMPL>(dm);
    }

    Death::~Death() = default;
    Death::Death(Death &&) noexcept = default;
    Death &Death::operator=(Death &&) noexcept = default;

    void Death::doEvent(std::shared_ptr<person::PersonBase> person,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        std::shared_ptr<stats::DeciderBase> decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
