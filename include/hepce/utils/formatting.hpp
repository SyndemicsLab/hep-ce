////////////////////////////////////////////////////////////////////////////////
// File: formatting.hpp                                                       //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-21                                                  //
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

/// Removes leading and trailing spaces from a string in place.
inline void Trim(std::string &str) {
    while (str[0] == ' ')
        str.erase(str.begin());
    while (str[str.size() - 1] == ' ')
        str.pop_back();
}

template <typename T>
/// Splits a delimited string into a vector of values of type `T`.
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

/// Converts a non-negative numeric string to a double.
/// @throws std::invalid_argument if the value is negative or cannot be parsed.
inline double SToDPositive(const std::string &s) {
    double d = std::stod(s);
    if (d < 0) {
        throw std::invalid_argument("Value must be positive");
    }
    return d;
}

/// Converts a string to lowercase.
/// @param string
inline std::string ToLower(std::string string) {
    std::string temp = string;
    std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
    return temp;
}

/// Converts a string representation of a Boolean value to `bool`.
/// @param string
inline bool SToBool(std::string string) {
    std::string temp = ToLower(string);
    std::istringstream is(temp);
    bool b;
    is >> std::boolalpha >> b;
    return b;
}

/// Converts a Boolean value to its lowercase string representation.
inline std::string const BoolToString(bool b) {
    return b ? std::string("true") : std::string("false");
}

/// Converts every string in a vector to lowercase.
inline std::vector<std::string>
ToLowerVector(const std::vector<std::string> &vec_str) {
    std::vector<std::string> copy = vec_str;
    std::transform(copy.begin(), copy.end(), copy.begin(),
                   [](std::string &str) { return ToLower(str); });
    return copy;
}

template <typename T>
/// Returns whether any queried value occurs in the searched vector.
inline bool FindInVector(std::vector<T> searched, std::vector<T> queries) {
    for (const T &query : queries) {
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