////////////////////////////////////////////////////////////////////////////////
// File: voluntary_relink_internals.hpp                                       //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_EVENT_HCV_VOLUNTARYRELINKINTERNALS_HPP_
#define HEPCE_EVENT_HCV_VOLUNTARYRELINKINTERNALS_HPP_

#include <hepce/event/hcv/voluntary_relink.hpp>

#include "internals/event_internals.hpp"

namespace hepce {
namespace event {
namespace hcv {

class VoluntaryRelinkImpl : public virtual VoluntaryRelink, public EventBase {
public:
    VoluntaryRelinkImpl(datamanagement::ModelData &model_data,
                        const std::string &log_name = "console");

    ~VoluntaryRelinkImpl() = default;

    int Execute(model::Person &person, model::Sampler &sampler) override;

private:
    double _relink_probability = 0.0;
    int _voluntary_relink_duration = 0.0;
    double _cost = 0.0;

    /// @brief During a voluntary relink, a new RNA test is run on the
    /// person. This has no impact outside of costs to the person.
    /// @param person person who is being relinked
    inline void AddRNATest(model::Person &person) {
        person.AddRnaScreen(data::InfectionType::kHcv);
        SetEventCost(_cost);
        AddEventCost(person);
    }
};

} // namespace hcv
} // namespace event
} // namespace hepce

#endif // HEPCE_EVENT_HCV_VOLUNTARYRELINKINTERNALS_HPP_