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
/// This file contains the implementation of the VoluntaryRelinking Event
/// Subclass.
///
/// Created Date: Tuesday, August 15th 2023, 8:50:56 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//
#include "VoluntaryRelinking.hpp"
#include "Decider.hpp"
#include "Person.hpp"
#include "spdlog/spdlog.h"
#include <DataManagement/DataManagerBase.hpp>
#include <string>

namespace event {
    class VoluntaryRelinking::VoluntaryRelinkingIMPL {
    private:
    public:
        void doEvent(person::PersonBase &person,
                     std::shared_ptr<datamanagement::DataManagerBase> dm,
                     std::unique_ptr<stats::Decider> &decider) {
            std::string data;
            dm->GetFromConfig("linking.voluntary_relinkage_probability", data);
            double relinkProbability = std::stod(data);

            int relink = decider->GetDecision({relinkProbability});

            data.clear();
            dm->GetFromConfig("linking.voluntary_relink_duration", data);
            int voluntaryRelinkDuration = std::stoi(data);

            if (person.GetLinkState() != person::LinkageState::UNLINKED ||
                (person.GetTimeSinceLinkChange()) > voluntaryRelinkDuration ||
                relink == 1) {
                return; // if linked or never linked OR too long since last
                        // linked OR relink draw is false
            }
            person.Link(person::LinkageType::BACKGROUND);
        }
    };

    VoluntaryRelinking::VoluntaryRelinking() {
        impl = std::make_unique<VoluntaryRelinkingIMPL>();
    }

    VoluntaryRelinking::~VoluntaryRelinking() = default;

    VoluntaryRelinking::VoluntaryRelinking(VoluntaryRelinking &&) noexcept =
        default;
    VoluntaryRelinking &
    VoluntaryRelinking::operator=(VoluntaryRelinking &&) noexcept = default;

    void VoluntaryRelinking::doEvent(
        person::PersonBase &person,
        std::shared_ptr<datamanagement::DataManagerBase> dm,
        std::unique_ptr<stats::Decider> &decider) {
        impl->doEvent(person, dm, decider);
    }
} // namespace event
