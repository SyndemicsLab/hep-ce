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
/// This file contains the declaration of the Treatment Event Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#ifndef EVENT_TREATMENT_HPP_
#define EVENT_TREATMENT_HPP_

#include "Event.hpp"

/// @brief Namespace containing the Events that occur during the simulation
namespace event {
    static std::vector<std::string>
    setupTreatmentSections(std::vector<std::string> vector1,
                           const std::vector<std::string> &vector2) {
        vector1.insert(vector1.end(), vector2.begin(), vector2.end());
        return vector1;
    }

    /// @brief Subclass of Event used to Provide Treatment to People
    class Treatment : public Event {
    private:
        class TreatmentIMPL;
        std::shared_ptr<TreatmentIMPL> impl;
        std::shared_ptr<stats::Decider> decider;

        /// @brief Implementation of Virtual Function doEvent
        /// @param person Individual Person undergoing Event
        void doEvent(person::PersonBase &person) override;

    public:
        Treatment(std::shared_ptr<stats::Decider> decider,
                  std::shared_ptr<datamanagement::DataManager> dm,
                  std::string name = std::string("Treatment"));
        virtual ~Treatment() = default;
    };
} // namespace event
#endif
