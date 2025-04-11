////////////////////////////////////////////////////////////////////////////////
// File: Event.cpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-01-10                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-11                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "Event.hpp"
#include "Person.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"

namespace event {
int Event::Execute(std::shared_ptr<person::PersonBase> person,
                   std::shared_ptr<datamanagement::DataManagerBase> dm,
                   std::shared_ptr<stats::DeciderBase> decider) {
    if (person->IsAlive()) {
        if (dm == nullptr) {
            spdlog::get("main")->warn(
                "No Data Manager Provided during Construction. No Data "
                "Loaded to Event.");
            return -1;
        }
        this->DoEvent(person, dm, decider);
    }
    return 0;
}

double Event::DiscountEventCost(double cost, double discount_rate, int timestep,
                                bool annual) {
    // dividing discountRate by 12 because discount rates are annual
    discount_rate = annual ? discount_rate / 12 : discount_rate;
    double denominator = std::pow(1 + discount_rate, timestep);
    return cost / denominator;
}

/// @brief
/// @param
/// @return
bool Event::GetBoolFromConfig(
    std::string config_key,
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string config_data;
    dm->GetFromConfig(config_key, config_data);
    if (config_data.empty()) {
        spdlog::get("main")->warn("No `{}' Found! Returning `false`...",
                                  config_key);
        return false;
    }
    bool to_return;
    std::istringstream(config_data) >> std::boolalpha >> to_return;
    return to_return;
}

/// @brief
/// @param
/// @return
int Event::GetIntFromConfig(
    std::string config_key,
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string config_data;
    dm->GetFromConfig(config_key, config_data);
    if (config_data.empty()) {
        spdlog::get("main")->warn("No `{}' Found!", config_key);
        config_data = "0";
    }
    return std::stoi(config_data);
}

/// @brief
/// @param
/// @return
double
Event::GetDoubleFromConfig(std::string config_key,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           bool positive) {
    std::string config_data;
    dm->GetFromConfig(config_key, config_data);
    if (config_data.empty()) {
        spdlog::get("main")->warn("No `{}' Found!", config_key);
        config_data = "0.0";
    }
    if (positive) {
        return Utils::stod_positive(config_data);
    }
    return std::stod(config_data);
}

/// @brief
/// @param
/// @return
std::string Event::GetStringFromConfig(
    std::string config_key,
    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string config_data;
    dm->GetFromConfig(config_key, config_data);
    if (config_data.empty()) {
        spdlog::get("main")->warn("No `{}' Found!", config_key);
    }
    return config_data;
}

Event::Event() {}

Event::~Event() = default;
Event::Event(Event &&) noexcept = default;
Event &Event::operator=(Event &&) noexcept = default;
} // namespace event
