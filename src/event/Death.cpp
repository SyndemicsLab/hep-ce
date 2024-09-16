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

        std::string buildSQL(person::PersonBase const person) const {
            std::stringstream sql;
            sql << "SELECT background_mortality, SMR";
            sql << "FROM SMR ";
            sql << "INNER JOIN background_mortality ON "
                   "(SMR.gender = background_mortality.gender) ";
            sql << "WHERE age_years = " << std::to_string(person.GetAge());
            sql << "AND gender = " << person.GetSex();
            sql << "AND drug_behavior = " << person.GetBehavior();

            return sql.str();
        }
        /// @brief The actual death of a person
        /// @param person Person who dies
        void die(person::PersonBase &person) { person.Die(); }

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
            if (!rc) {
                // error
                backgroundMortProb = 0;
                smr = 0;
            } else {
                backgroundMortProb = storage[0].back_mort;
                smr = storage[0].smr;
            }
        }

        bool ReachedMaxAge(person::PersonBase &person) {
            if (person.GetAge() >= 1200) {
                this->die(person);
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
            if (retIdx != 2) {
                this->die(person);
            }
        }
    };

    Death::Death(std::shared_ptr<stats::Decider> decider,
                 std::shared_ptr<datamanagement::DataManager> dm,
                 std::string name)
        : Event(dm, name), decider(decider) {
        impl = std::make_unique<DeathIMPL>();
    }

    void Death::doEvent(person::PersonBase &person) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
