////////////////////////////////////////////////////////////////////////////////
// File: config.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                    //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-22                                                  //
// Modified By: Dimitri Baptiste                                              //
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
                              datamanagement::ModelData &model_data,
                              bool def_val = false) {
    std::string config_data;
    try {
        config_data = model_data.GetFromConfig(config_key);
    } catch (std::exception &e) {
        return def_val;
    }
    if (config_data.empty()) {
        return def_val;
    }
    return SToBool(config_data);
}

/// @brief
/// @param
/// @return
inline int GetIntFromConfig(std::string config_key,
                            datamanagement::ModelData &model_data,
                            int def_val = -1) {
    std::string config_data;
    try {
        config_data = model_data.GetFromConfig(config_key);
    } catch (std::exception &e) {
        return def_val;
    }
    if (config_data.empty()) {
        return def_val;
    }
    return std::stoi(config_data);
}

/// @brief
/// @param
/// @return
inline double GetDoubleFromConfig(std::string config_key,
                                  datamanagement::ModelData &model_data,
                                  bool positive = true, double def_val = -1.0) {
    std::string config_data;

    try {
        config_data = model_data.GetFromConfig(config_key);
    } catch (std::exception &e) {
        return def_val; // Should we log this or throw an exception back?
    }
    if (config_data.empty()) {
        return def_val;
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
                                       datamanagement::ModelData &model_data,
                                       std::string def_val = "") {
    std::string s;
    try {
        s = model_data.GetFromConfig(config_key);
    } catch (std::exception &e) {
        return def_val;
    }
    return s;
}

inline bool FindInEventList(std::string event_name,
                            datamanagement::ModelData &model_data) {
    auto event_list = utils::ToLowerVector(utils::SplitToVecT<std::string>(
        model_data.GetFromConfig("simulation.events"), ','));
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
