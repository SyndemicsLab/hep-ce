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
/// This file contains the implementation of the BehaviorChanges Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Overdose.hpp"
#include "Person.hpp"

namespace event {
    class Overdose::OverdoseIMPL {
    private:
        double getProbability(person::PersonBase &person) {
            // overdose probability is stratified by behavior classification and
            // MOUD state
            return 0.0;
        }

    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManager> dm,
                     std::shared_ptr<stats::Decider> decider) {
            person::Behavior bc = person.GetBehavior();
            // return immediately if not in active use state
            if (bc < person::Behavior::NONINJECTION) {
                return;
            }
            // check od probability
            double overdoseProbability = this->getProbability(person);
            // determine if person overdoses
            if (decider->GetDecision(
                    {1.0 - overdoseProbability, overdoseProbability})) {
                person.ToggleOverdose();
            }
        }
    };

    Overdose::Overdose(std::shared_ptr<stats::Decider> decider,
                       std::shared_ptr<datamanagement::DataManager> dm,
                       std::string name)
        : Event(dm, name), decider(decider) {
        impl = std::make_unique<OverdoseIMPL>();
    }

    void Overdose::doEvent(person::PersonBase &person) {
        impl->doEvent(person, dm, decider);
    }

} // namespace event
