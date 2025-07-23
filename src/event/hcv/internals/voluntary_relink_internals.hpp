////////////////////////////////////////////////////////////////////////////////
// File: voluntary_relink_internals.hpp                                       //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_VOLUNTARYRELINKINTERNALS_HPP_
#define HEPCE_EVENT_HCV_VOLUNTARYRELINKINTERNALS_HPP_

// File Header
#include <hepce/event/hcv/voluntary_relink.hpp>

// Local Includes
#include "../../internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {

class VoluntaryRelinkImpl : public virtual VoluntaryRelink, public EventBase {
public:
    VoluntaryRelinkImpl(datamanagement::ModelData &model_data,
                        const std::string &log_name = "console");

    ~VoluntaryRelinkImpl() = default;

    void Execute(model::Person &person, model::Sampler &sampler) override;

    void LoadData(datamanagement::ModelData &model_data) override;

private:
    double _relink_probability = 0.0;
    int _voluntary_relink_duration = 0.0;
    double _cost = 0.0;

    inline bool RelinkInTime(const model::Person &person) const {
        return (GetTimeSince(person,
                             person.GetLinkageDetails(data::InfectionType::kHcv)
                                 .time_link_change) <
                _voluntary_relink_duration);
    }

    inline bool Unlinked(const model::Person &person) const {
        return (
            person.GetLinkageDetails(data::InfectionType::kHcv).link_state ==
            data::LinkageState::kUnlinked);
    }
};

} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_VOLUNTARYRELINKINTERNALS_HPP_