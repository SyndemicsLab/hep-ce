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
#include "spdlog/spdlog.h"

namespace event {
    class MOUD::MOUDIMPL {
    private:
        /// @brief Retrieve Transition Rates for MOUD for the
        /// individual Person from the SQL Table
        /// @param person Person to retrieve transition rates for
        /// @return Vector of Transition Rates for each MOUD status
        std::vector<double> getTransitions(person::Person &person);

        /// @brief Add cost based on person's current treatment status upon
        /// "experiencing" this event
        /// @param person Person accruing cost
        void insertMOUDCost(person::Person &person);

    public:
        void doEvent(person::Person &person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::unique_ptr<stats::Decider> &decider) {

            person::Behavior bc = person.GetBehavior();

            // Can only enter MOUD if in an active use state.
            if (!(bc >= person::Behavior::NONINJECTION)) {
                // 1. Check the person's current MOUD status
                person::MOUD moud = person.GetMoudState();
                // 2. Draw probability of changing MOUD state.
                // TODO: MAKE THIS A REAL TRANSITION RATE
                std::vector<double> probs = {0.50, 0.25, 0.25};
                // 3. Make a transition decision.
                int res = decider->GetDecision(probs);
                if (res >= (int)person::MOUD::COUNT) {
                    spdlog::get("main")->error("MOUD Decision returned "
                                               "value outside bounds");
                    return;
                }
                person::MOUD toMoud = (person::MOUD)res;
                if (toMoud == person::MOUD::CURRENT) {
                    if (moud != toMoud) {
                        // new treatment start
                        person.SetMoudState(toMoud);
                    }
                    // person continuing treatment
                    // add treatment cost
                } else {
                    // person discontinuing treatment
                    // or going from post-treatment to no treatment
                    person.SetMoudState(toMoud);
                    // figure out if we want to update timestartedmoud to an
                    // impossible value, e.g. -1
                }
            }
        }
    };
    MOUD::MOUD() { impl = std::make_unique<MOUDIMPL>(); }

    MOUD::~MOUD() = default;
    MOUD::MOUD(MOUD &&) noexcept = default;
    MOUD &MOUD::operator=(MOUD &&) noexcept = default;

    void MOUD::doEvent(person::Person &person,
                       std::shared_ptr<datamanagement::DataManagerBase> dm,
                       std::unique_ptr<stats::Decider> &decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
