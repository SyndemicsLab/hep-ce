////////////////////////////////////////////////////////////////////////////////
// File: config.hpp                                                           //
// Project: hep-ce                                                            //
// Created Date: 2025-04-18                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_CONFIG_HPP_
#define HEPCE_UTILS_CONFIG_HPP_

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

void ThrowPathError(const std::string &key) {
    std::ostringstream msg;
    msg << "Key `" << key << "` not found in config file!";
    throw std::runtime_error(msg.str());
}

void ThrowDataError(const std::string &key) {
    std::ostringstream msg;
    msg << "Unknown error occurred when attempting to get config value `" << key
        << "`!";
    throw std::runtime_error(msg.str());
}

/// @brief
/// @param
/// @return
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
}

/// @brief
/// @param
/// @return
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
}

/// @brief
/// @param
/// @return
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
}

/// @brief
/// @param
/// @return
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
}

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
