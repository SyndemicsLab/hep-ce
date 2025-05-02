////////////////////////////////////////////////////////////////////////////////
// File: config.hpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_CONFIG_HPP_
#define HEPCE_UTILS_CONFIG_HPP_

#include <memory>
#include <sstream>
#include <string>

#include <datamanagement/datamanagement.hpp>
#include <spdlog/spdlog.h>

#include "formatting.hpp"

/// @brief Namespace containing Utility Helper Functions
namespace hepce {
namespace utils {
/// @brief
/// @param
/// @return
inline bool GetBoolFromConfig(std::string config_key,
                              datamanagement::ModelData &model_data) {
    std::string config_data = model_data.GetFromConfig(config_key);
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
inline int GetIntFromConfig(std::string config_key,
                            datamanagement::ModelData &model_data) {
    std::string config_data = model_data.GetFromConfig(config_key);
    if (config_data.empty()) {
        spdlog::get("main")->warn("No `{}' Found!", config_key);
        config_data = "0";
    }
    return std::stoi(config_data);
}

/// @brief
/// @param
/// @return
inline double GetDoubleFromConfig(std::string config_key,
                                  datamanagement::ModelData &model_data,
                                  bool positive = true) {
    std::string config_data = model_data.GetFromConfig(config_key);
    if (config_data.empty()) {
        spdlog::get("main")->warn("No `{}' Found!", config_key);
        config_data = "0.0";
    }
    if (positive) {
        return SToDPositive(config_data);
    }
    return std::stod(config_data);
}

/// @brief
/// @param
/// @return
inline std::string GetStringFromConfig(std::string config_key,
                                       datamanagement::ModelData &model_data) {
    std::string config_data = model_data.GetFromConfig(config_key);
    if (config_data.empty()) {
        spdlog::get("main")->warn("No `{}' Found!", config_key);
    }
    return config_data;
}
} // namespace utils
} // namespace hepce
#endif // HEPCE_UTILS_CONFIG_HPP_
