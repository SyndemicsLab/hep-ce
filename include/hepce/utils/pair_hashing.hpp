////////////////////////////////////////////////////////////////////////////////
// File: pair_hashing.hpp                                                     //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
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
/// Hashes a two-integer tuple for use in an unordered container.
struct key_hash_2i {
    /// Returns the tuple hash.
    std::size_t operator()(const tuple_2i &k) const {
        return std::get<0>(k) ^ std::get<1>(k);
    }
};
/// Compares two two-integer tuples for equality.
struct key_equal_2i {
    /// Returns whether both tuples contain the same values.
    bool operator()(const tuple_2i &v0, const tuple_2i &v1) const {
        return v0 == v1;
    }
};

// tuple_3i definition

using tuple_3i = std::tuple<int, int, int>;
/// Hashes a three-integer tuple for use in an unordered container.
struct key_hash_3i {
    /// Returns the tuple hash.
    std::size_t operator()(const tuple_3i &k) const {
        return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k);
    }
};
/// Compares two three-integer tuples for equality.
struct key_equal_3i {
    /// Returns whether both tuples contain the same values.
    bool operator()(const tuple_3i &v0, const tuple_3i &v1) const {
        return v0 == v1;
    }
};

// tuple_4i definition

using tuple_4i = std::tuple<int, int, int, int>;
/// Hashes a four-integer tuple for use in an unordered container.
struct key_hash_4i {
    /// Returns the tuple hash.
    std::size_t operator()(const tuple_4i &k) const {
        return std::get<0>(k) ^ std::get<1>(k) ^ std::get<2>(k) ^
               std::get<3>(k);
    }
};
/// Compares two four-integer tuples for equality.
struct key_equal_4i {
    /// Returns whether both tuples contain the same values.
    bool operator()(const tuple_4i &v0, const tuple_4i &v1) const {
        return v0 == v1;
    }
};
} // namespace utils
} // namespace hepce

#endif // HEPCE_UTILS_PAIRHASHING_HPP_