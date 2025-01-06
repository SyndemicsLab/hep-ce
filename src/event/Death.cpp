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
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <sstream>

namespace event {
    class Death::DeathIMPL {
    private:
        double f4_infected_probability;
        double f4_uninfected_probability;
        double decomp_infected_probability;
        double decomp_uninfected_probability;

        struct background_smr {
            double back_mort = 0.0;
            double smr = 0.0;
        };

        using backgroundmap_t =
            std::unordered_map<Utils::tuple_3i, struct background_smr,
                               Utils::key_hash_3i, Utils::key_equal_3i>;
        backgroundmap_t background_data;

        std::string BackgroundMortalitySQL() const {
            std::stringstream sql;
            sql << "SELECT bm.age_years, bm.gender, "
                   "smr.drug_behavior, bm.background_mortality, smr.smr";
            sql << " FROM smr AS smr";
            sql << " INNER JOIN background_mortality AS bm ON "
                   "smr.gender = bm.gender;";

            return sql.str();
        }

        static int callback_background(void *storage, int count, char **data,
                                       char **columns) {
            Utils::tuple_3i tup = std::make_tuple(
                std::stoi(data[0]), std::stoi(data[1]), std::stoi(data[2]));
            struct background_smr temp = {Utils::stod_positive(data[3]),
                                          Utils::stod_positive(data[4])};
            (*((backgroundmap_t *)storage))[tup] = temp;
            return 0;
        }

        int LoadBackgroundMortality(
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string query = this->BackgroundMortalitySQL();
            std::string error;
            int rc = dm->SelectCustomCallback(query, this->callback_background,
                                              &background_data, error);
            if (rc != 0) {
                spdlog::get("main")->error(
                    "Error extracting Death Data from background_mortality and "
                    "SMR! Error Message: {}",
                    error);
                spdlog::get("main")->info("Query: {}", query);
            }
            if (background_data.empty()) {
                spdlog::get("main")->warn(
                    "Setting background death probability to zero. No Data "
                    "Found for background_mortality and "
                    "SMR with the query: {}",
                    query);
            }
            return rc;
        }

        using overdosemap_t =
            std::unordered_map<Utils::tuple_2i, double, Utils::key_hash_2i,
                               Utils::key_equal_2i>;
        overdosemap_t overdose_data;

        std::string OverdoseSQL() const {
            return "SELECT moud, drug_behavior, fatality_probability FROM "
                   "overdoses;";
        }

        static int callback_overdose(void *storage, int count, char **data,
                                     char **columns) {
            Utils::tuple_2i tup =
                std::make_tuple(std::stoi(data[0]), std::stoi(data[1]));
            (*((overdosemap_t *)storage))[tup] = Utils::stod_positive(data[2]);
            return 0;
        }

        int CheckOverdoseTable(
            std::shared_ptr<datamanagement::DataManagerBase> dm) {
            std::string query =
                "SELECT name FROM sqlite_master WHERE type='table' AND "
                "name='overdoses';";
            datamanagement::Table table;
            int rc = dm->Select(query, table);
            if (rc == 0 && table.empty()) {
                spdlog::get("main")->info(
                    "No Overdoses table Found During Death. Ignoring.");
                rc = -1;
            }
            return rc;
        }

        int
        LoadOverdoseData(std::shared_ptr<datamanagement::DataManagerBase> dm) {
            int rc = CheckOverdoseTable(dm);
            if (rc != 0) {
                return rc;
            }
            std::string error;
            rc = dm->SelectCustomCallback(
                OverdoseSQL(), this->callback_overdose, &overdose_data, error);
            if (rc != 0) {
                spdlog::get("main")->error("Error extracting Fatal Overdose "
                                           "Probability! Error Message: {}",
                                           error);
            }
            return rc;
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

            if (person->GetTrueFibrosisState() == person::FibrosisState::F4) {
                if (person->GetHCV() == person::HCV::NONE) {
                    prob = f4_uninfected_probability;
                } else {
                    prob = f4_infected_probability;
                }

            } else if (person->GetTrueFibrosisState() ==
                       person::FibrosisState::DECOMP) {
                if (person->GetHCV() == person::HCV::NONE) {
                    prob = decomp_uninfected_probability;
                } else {
                    prob = decomp_infected_probability;
                }
            } else {
                prob = 0;
            }
        }

        void getSMRandBackgroundProb(
            std::shared_ptr<person::PersonBase> person,
            std::shared_ptr<datamanagement::DataManagerBase> dm,
            double &backgroundMortProb, double &smr) {

            if (background_data.empty()) {
                spdlog::get("main")->warn(
                    "Setting background death probability to zero. No Data "
                    "Found for background_mortality and "
                    "SMR");
                backgroundMortProb = 0;
                smr = 0;
                return;
            }
            // age, gender, drug
            int age_years = (int)(person->GetAge() / 12.0);
            int gender = (int)person->GetSex();
            int drug_behavior = (int)person->GetBehavior();
            Utils::tuple_3i tup =
                std::make_tuple(age_years, gender, drug_behavior);
            backgroundMortProb = background_data[tup].back_mort;
            smr = background_data[tup].smr;
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

            if (overdose_data.empty()) {
                spdlog::get("main")->warn(
                    "No Fatal Overdose Probability Found!");
                person->ToggleOverdose();
                return false;
            }

            int moud = (int)person->GetMoudState();
            int drug_behavior = (int)person->GetBehavior();
            Utils::tuple_2i tup = std::make_tuple(moud, drug_behavior);
            double probability = overdose_data[tup];

            if (decider->GetDecision({probability, 1 - probability}) != 0) {
                person->ToggleOverdose();
                return false;
            }
            this->die(person, person::DeathReason::OVERDOSE);
            return true;
        }

    public:
        void DoEvent(std::shared_ptr<person::PersonBase> person,
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

            dm->GetFromConfig("mortality.f4_infected", data);
            this->f4_infected_probability = Utils::stod_positive(data);

            dm->GetFromConfig("mortality.f4_uninfected", data);
            this->f4_uninfected_probability = Utils::stod_positive(data);

            dm->GetFromConfig("mortality.decomp_infected", data);
            this->decomp_infected_probability = Utils::stod_positive(data);

            dm->GetFromConfig("mortality.decomp_uninfected", data);
            this->decomp_uninfected_probability = Utils::stod_positive(data);

            int rc = LoadOverdoseData(dm);
            rc = LoadBackgroundMortality(dm);
        }
    };

    Death::Death(std::shared_ptr<datamanagement::DataManagerBase> dm) {
        impl = std::make_unique<DeathIMPL>(dm);
    }

    Death::~Death() = default;
    Death::Death(Death &&) noexcept = default;
    Death &Death::operator=(Death &&) noexcept = default;

    void Death::DoEvent(std::shared_ptr<person::PersonBase> person,
                        std::shared_ptr<datamanagement::DataManagerBase> dm,
                        std::shared_ptr<stats::DeciderBase> decider) {
        impl->DoEvent(person, dm, decider);
    }
} // namespace event
