////////////////////////////////////////////////////////////////////////////////
// File: version.hpp                                                          //
// Project: HEPCESimulationv2                                                 //
// Created Date: Th Apr 2025                                                  //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_VERSION_HPP_
#define HEPCE_VERSION_HPP_

#define HEPCE_VER_MAJOR 2
#define HEPCE_VER_MINOR 0
#define HEPCE_VER_PATCH 1

#define HEPCE_TO_VERSION(major, minor, patch)                                  \
    (major * 10000 + minor * 100 + patch)
#define HEPCE_VERSION                                                          \
    HEPCE_TO_VERSION(HEPCE_VER_MAJOR, HEPCE_VER_MINOR, HEPCE_VER_PATCH)

#endif // HEPCE_VERSION_HPP_