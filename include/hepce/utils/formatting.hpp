////////////////////////////////////////////////////////////////////////////////
// File: formatting.hpp                                                       //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_FORMATTING_HPP_
#define HEPCE_UTILS_FORMATTING_HPP_

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace hepce {
namespace utils {

inline void Trim(std::string &str) {
    while (str[0] == ' ')
        str.erase(str.begin());
    while (str[str.size() - 1] == ' ')
        str.pop_back();
}

template <typename T>
inline std::vector<T> SplitToVecT(const std::string &s, char delim) {
    std::vector<T> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        T temp;
        if constexpr (std::is_same_v<T, std::string>) {
            utils::Trim(item);
            temp = item;
        } else {
            temp << item;
        }
        elems.push_back(temp);
    }
    return elems;
}

inline double SToDPositive(const std::string &s) {
    double d = std::stod(s);
    if (d < 0) {
        throw std::invalid_argument("Value must be positive");
    }
    return d;
}

/// @brief
/// @param string
/// @return
inline std::string ToLower(std::string string) {
    std::string temp = string;
    std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    return temp;
}

/// @brief
/// @param string
/// @return
inline bool SToBool(std::string string) {
    std::string temp = ToLower(string);
    std::istringstream is(temp);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}

inline std::string const BoolToString(bool b) {
    return b ? std::string("true") : std::string("false");
}

template <typename T>
inline bool FindInVector(std::vector<T> searched, std::vector<T> queries) {
    for (T &query : queries) {
        if (std::find(searched.begin(), searched.end(), query) !=
            searched.end()) {
            return true;
        }
    }
    return false;
}

} // namespace utils
} // namespace hepce

#endif // HEPCE_UTILS_FORMATTING_HPP_