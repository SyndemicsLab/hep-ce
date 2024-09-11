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
#include "DataManager.hpp"
#include "Decider.hpp"
#include "Person.hpp"

namespace event {
    class Death::DeathIMPL {
    private:
        double f4Mort = 0;
        double decompMort = 0;
        /// @brief The actual death of a person
        /// @param person Person who dies
        void die(person::PersonBase &person) { person.Die(); }

        void getMortalityProbabilities(person::PersonBase const &person,
                                       double &backgroundMortProb, double &smr,
                                       double &fibrosisDeathProb) {
            std::unordered_map<std::string, std::string> selectCriteria;

            selectCriteria["age_years"] = std::to_string((int)person->age);
            selectCriteria["gender"] =
                person::person::sexEnumToStringMap[person->getSex()];
            selectCriteria["drug_behavior"] =
                person::person::behaviorEnumToStringMap[person->getBehavior()];

            auto resultTable = table->selectWhere(selectCriteria);
            if (resultTable->empty()) {
                // error
                backgroundMortProb = 0;
                smr = 0;
            } else {
                backgroundMortProb =
                    std::stod((*resultTable)["background_mortality"][0]);
                smr = std::stod((*resultTable)["SMR"][0]);
            }

            switch (person.GetFibrosisState()) {
            case person::FibrosisState::F4:
                fibrosisDeathProb = this->f4Mort;
                break;
            case person::FibrosisState::DECOMP:
                fibrosisDeathProb = this->decompMort;
                break;
            default:
                fibrosisDeathProb = 0;
            }

            // if (person->getOverdose()) {
            //     // we need to figure out how we're gonna make this time based
            //     fatalOverdoseProb =
            //         stod((*resultTable)["fatal_overdose_cycle52"][0]);
            // }
        }

    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) {
            std::string data;
            dm->GetFromConfig("mortality.f4", data);
            f4Mort = std::stod(data);
            data.clear();
            dm->GetFromConfig("mortality.decomp", data);
            decompMort = std::stod(data);

            if (person.GetAge() >= 1200) {
                this->die(person);
                return;
            }

            // "Calculate background mortality rate based on age, gender, and
            // IDU"
            double backgroundMortality = 1.0;
            double backgroundMortProb = 0.0;
            double smr = 0.0;
            double fibrosisDeathProb = 0.0;

            // 2. Get fatal OD probability.
            this->getMortalityProbabilities(person, backgroundMortProb, smr,
                                            fibrosisDeathProb);
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
