////////////////////////////////////////////////////////////////////////////////
// File: config.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-20                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_CONFIG_HPP_
#define HEPCE_UTILS_CONFIG_HPP_

#include <limits>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include <hepce/data/inputs.hpp>

#include <spdlog/spdlog.h>

#include "formatting.hpp"

/// @brief Namespace containing Utility Helper Functions
namespace hepce {
namespace utils {

/// Throws an error indicating that a configuration key is missing.
/// @param key Missing configuration key.
inline void ThrowPathError(const std::string &key) {
    std::ostringstream msg;
    msg << "Key `" << key << "` not found in config file!";
    throw std::runtime_error(msg.str());
}

/// Throws an error indicating that a configuration value could not be read.
/// @param key Configuration key whose value could not be read.
inline void ThrowDataError(const std::string &key) {
    std::ostringstream msg;
    msg << "Unknown error occurred when attempting to get config value `" << key
        << "`!";
    throw std::runtime_error(msg.str());
}

/// Reads a Boolean value from the simulation configuration.
/// @param config_key Configuration key.
/// @param inputs Simulation inputs containing the configuration.
/// @param def_val Value returned when the configured value has invalid data.
/// @return The configured Boolean value or `def_val`.
inline bool GetBoolFromConfig(const std::string &config_key,
                              const data::Inputs &inputs,
                              bool def_val = false) {
    try {
        return inputs.GetPropertyTree().get<bool>(config_key);
    } catch (boost::property_tree::ptree_bad_data &e) {
        return def_val;
    } catch (boost::property_tree::ptree_bad_path &e) {
        ThrowPathError(config_key);
    } catch (std::exception &e) {
        ThrowDataError(config_key);
    }
    return false;
}

/// Reads an integer value from the simulation configuration.
/// @param config_key Configuration key.
/// @param inputs Simulation inputs containing the configuration.
/// @param def_val Value returned when the configured value has invalid data.
/// @return The configured integer or `def_val`.
inline int GetIntFromConfig(const std::string &config_key,
                            const data::Inputs &inputs, int def_val = -1) {
    try {
        return inputs.GetPropertyTree().get<int>(config_key);
    } catch (boost::property_tree::ptree_bad_data &e) {
        return def_val;
    } catch (boost::property_tree::ptree_bad_path &e) {
        ThrowPathError(config_key);
    } catch (std::exception &e) {
        ThrowDataError(config_key);
    }
    return std::numeric_limits<int>::min();
}

/// Reads a floating-point value from the simulation configuration.
/// @param config_key Configuration key.
/// @param inputs Simulation inputs containing the configuration.
/// @param positive Whether to return the absolute value of the result.
/// @param def_val Value returned when the configured value has invalid data.
/// @return The configured value or `def_val`.
inline double GetDoubleFromConfig(const std::string &config_key,
                                  const data::Inputs &inputs,
                                  bool positive = true, double def_val = -1.0) {
    std::string config_data;

    try {
        auto res = inputs.GetPropertyTree().get<double>(config_key);
        return (positive) ? std::abs(res) : res;
    } catch (boost::property_tree::ptree_bad_data &e) {
        return def_val;
    } catch (boost::property_tree::ptree_bad_path &e) {
        ThrowPathError(config_key);
    } catch (std::exception &e) {
        ThrowDataError(config_key);
    }
    return std::numeric_limits<double>::min();
}

/// Reads a string value from the simulation configuration.
/// @param config_key Configuration key.
/// @param inputs Simulation inputs containing the configuration.
/// @param def_val Value returned when the configured value has invalid data.
/// @return The configured string or `def_val`.
inline std::string GetStringFromConfig(const std::string &config_key,
                                       const data::Inputs &inputs,
                                       std::string def_val = "") {
    try {
        return inputs.GetPropertyTree().get<std::string>(config_key);
    } catch (boost::property_tree::ptree_bad_data &e) {
        return def_val;
    } catch (boost::property_tree::ptree_bad_path &e) {
        ThrowPathError(config_key);
    } catch (std::exception &e) {
        ThrowDataError(config_key);
    }
    return "";
}

/// Tests whether an event name occurs in the configured event list.
/// @param event_name Event name to find, case-insensitively.
/// @param inputs Simulation inputs containing the event list.
/// @return `true` when the event is configured.
inline bool FindInEventList(const std::string &event_name,
                            const data::Inputs &inputs) {
    auto event_list = utils::ToLowerVector(utils::SplitToVecT<std::string>(
        GetStringFromConfig("simulation.events", inputs), ','));
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
