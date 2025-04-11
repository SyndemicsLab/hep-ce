////////////////////////////////////////////////////////////////////////////////
// File: ConfigUtils.hpp                                                        //
// Project: HEPCESimulationv2                                                 //
// Created: 2025-04-11                                                        //
// Author: Dimitri Baptiste                                                   //
// -----                                                                      //
// Last Modified: 2025-04-11                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_CONFIGUTILS_HPP_
#define UTILS_CONFIGUTILS_HPP_

#include <DataManagement/DataManagerBase.hpp>
#include <memory>
#include <string>

/// @brief Namespace containing Utility Helper Functions
namespace Utils {
bool GetBoolFromConfig(std::string config_key,
                       std::shared_ptr<datamanagement::DataManagerBase> dm);

int GetIntFromConfig(std::string config_key,
                     std::shared_ptr<datamanagement::DataManagerBase> dm);

double GetDoubleFromConfig(std::string config_key,
                           std::shared_ptr<datamanagement::DataManagerBase> dm,
                           bool positive = true);
std::string
GetStringFromConfig(std::string config_key,
                    std::shared_ptr<datamanagement::DataManagerBase> dm);
} // namespace Utils
#endif
