////////////////////////////////////////////////////////////////////////////////
// File: EventFactory.hpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created: 2024-01-18                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2024-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef EVENTFACTORY_HPP_
#define EVENTFACTORY_HPP_

#include "Event.hpp"
#include <memory>
#include <string>

namespace datamanagement {
class DataManagerBase;
}
namespace event {

class EventFactory {
private:
    /// @brief
    /// @tparam T
    /// @param generator
    /// @param table
    /// @return
    template <typename T>
    std::shared_ptr<Event> makeEvent(datamanagement::ModelData &model_data) {
        return std::make_shared<T>(dm);
    }

public:
    std::shared_ptr<Event> create(std::string eventName,
                                  datamanagement::ModelData &model_data);
};
} // namespace event

#endif
