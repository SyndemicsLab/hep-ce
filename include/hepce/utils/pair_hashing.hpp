////////////////////////////////////////////////////////////////////////////////
// File: pair_hashing.hpp                                                     //
// Project: HEPCESimulationv2                                                 //
// Created Date: 2025-04-17                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_UTILS_PAIRHASHING_HPP_
#define HEPCE_UTILS_PAIRHASHING_HPP_

#include <cmath>
#include <tuple>

namespace hepce {
namespace utils {
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
} // namespace utils
} // namespace hepce

#endif // HEPCE_UTILS_PAIRHASHING_HPP_