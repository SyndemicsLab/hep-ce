////////////////////////////////////////////////////////////////////////////////
// File: ConfigUtils.cpp                                                      //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-11                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-11                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "ConfigUtils.hpp"
#include "Utils.hpp"
#include "spdlog/spdlog.h"
#include <sstream>

namespace Utils {
/// @brief
/// @param
/// @return
bool GetBoolFromConfig(std::string config_key,
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
int GetIntFromConfig(std::string config_key,
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
double GetDoubleFromConfig(std::string config_key,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           bool positive) {
    std::string config_data;
    dm->GetFromConfig(config_key, config_data);
    if (config_data.empty()) {
        spdlog::get("main")->warn("No `{}' Found!", config_key);
        config_data = "0.0";
    }
    if (positive) {
        return stod_positive(config_data);
    }
    return std::stod(config_data);
}

/// @brief
/// @param
/// @return
std::string
GetStringFromConfig(std::string config_key,
                    std::shared_ptr<datamanagement::DataManagerBase> dm) {
    std::string config_data;
    dm->GetFromConfig(config_key, config_data);
    if (config_data.empty()) {
        spdlog::get("main")->warn("No `{}' Found!", config_key);
    }
    return config_data;
}
} // namespace Utils
