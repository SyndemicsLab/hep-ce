////////////////////////////////////////////////////////////////////////////////
// File: config.hpp                                                           //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-18                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-15                                                  //
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
    std::string config_data;
    try {
        config_data = model_data.GetFromConfig(config_key);
    } catch (std::exception &e) {
        return false;
    }
    if (config_data.empty()) {
        return false;
    }
    return SToBool(config_data);
}

/// @brief
/// @param
/// @return
inline int GetIntFromConfig(std::string config_key,
                            datamanagement::ModelData &model_data) {
    std::string config_data;
    try {
        config_data = model_data.GetFromConfig(config_key);
    } catch (std::exception &e) {
        return 0;
    }
    if (config_data.empty()) {
        return 0;
    }
    return std::stoi(config_data);
}

/// @brief
/// @param
/// @return
inline double GetDoubleFromConfig(std::string config_key,
                                  datamanagement::ModelData &model_data,
                                  bool positive = true) {
    std::string config_data;

    try {
        config_data = model_data.GetFromConfig(config_key);
    } catch (std::exception &e) {
        return 0.0; // Should we log this or throw an exception back?
    }
    if (config_data.empty()) {
        return 0.0;
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
    std::string s;
    try {
        s = model_data.GetFromConfig(config_key);
    } catch (std::exception &e) {
        return "";
    }
    return s;
}

inline bool FindInEventList(std::string event_name,
                            datamanagement::ModelData &model_data) {
    auto event_list = utils::ToLowerVector(utils::SplitToVecT<std::string>(
        model_data.GetFromConfig("simulation.event"), ','));
    auto event_name_lower = utils::ToLower(event_name);
    if (std::find(event_list.begin(), event_list.end(), event_name_lower) !=
        event_list.end()) {
        return true;
    }
    return false;
}

} // namespace utils
} // namespace hepce
#endif // HEPCE_UTILS_CONFIG_HPP_
