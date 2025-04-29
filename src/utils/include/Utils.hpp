////////////////////////////////////////////////////////////////////////////////
// File: Utils.hpp                                                            //
// Project: HEPCESimulationv2                                                 //
// Created: 2023-08-14                                                        //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-28                                                  //
// Modified By: Dimitri Baptiste                                              //
// -----                                                                      //
// Copyright (c) 2023-2025 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILS_UTILS_HPP_
#define UTILS_UTILS_HPP_

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

/// @brief Namespace containing Utility Helper Functions
namespace Utils {
/// @brief Convert Probability to Rate
/// @param prob Probability to Convert
/// @return Converted Rate
double probabilityToRate(double prob);

/// @brief Convert Rate to Probability
/// @param rate Rate to Convert
/// @return Converted Probability
double rateToProbability(double rate);

/// @brief Calculate Discount to Provide
/// @param valueToDiscount Value that the Discount will be applied to
/// @param discountRate The Discount Rate
/// @param timestep The Timestep to Discount During
/// @return
double discount(double valueToDiscount, double discountRate, double timestep);

/// @brief
/// @param string
/// @return
std::string toLower(std::string string);

/// @brief
/// @param string
/// @return
bool stobool(std::string string);

inline std::string const boolToString(bool b) {
    return b ? std::string("true") : std::string("false");
}

// tuple_2i definition

using tuple_2i = std::tuple<int, int>;
struct key_hash_2i {
    std::size_t operator()(const tuple_2i &k) const {
        return std::get<0>(k) ^ std::get<1>(k);
    }
};
struct key_equal_2i {
    bool operator()(const tuple_2i &v0, const tuple_2i &v1) const {
        return v0 == v1;
    }
};

// tuple_3i definition

using tuple_3i = std::tuple<int, int, int>;
struct key_hash_3i {
    std::size_t operator()(const tuple_3i &k) const {
        return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
    }
};
struct key_equal_3i {
    bool operator()(const tuple_3i &v0, const tuple_3i &v1) const {
        return v0 == v1;
    }
};

// tuple_4i definition

using tuple_4i = std::tuple<int, int, int, int>;
struct key_hash_4i {
    std::size_t operator()(const tuple_4i &k) const {
        return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k) ^
               std::get<3>(k);
    }
};
struct key_equal_4i {
    bool operator()(const tuple_4i &v0, const tuple_4i &v1) const {
        return v0 == v1;
    }
};

inline void trim(std::string &str) {
    while (str[0] == ' ')
        str.erase(str.begin());
    while (str[str.size() - 1] == ' ')
        str.pop_back();
}

template <typename T>
inline std::vector<T> split2vecT(const std::string &s, char delim) {
    std::vector<T> elems;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        T temp;
        if constexpr (std::is_same_v<T, std::string>) {
            Utils::trim(item);
            temp = item;
        } else {
            temp << item;
        }
        elems.push_back(temp);
    }
    return elems;
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

inline double stod_positive(const std::string &s) {
    double d = std::stod(s);
    if (d < 0) {
        throw std::invalid_argument("Value must be positive");
    }
    return d;
}
} // namespace Utils

#endif
