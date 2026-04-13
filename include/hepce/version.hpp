////////////////////////////////////////////////////////////////////////////////
// File: version.hpp                                                          //
// Project: hep-ce                                                            //
// Created Date: 2025-04-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-03-19                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025-2026 Syndemics Lab at Boston Medical Center             //
////////////////////////////////////////////////////////////////////////////////
#ifndef HEPCE_VERSION_HPP_
#define HEPCE_VERSION_HPP_

#define HEPCE_VER_MAJOR 2
#define HEPCE_VER_MINOR 1
#define HEPCE_VER_PATCH 0

#define HEPCE_TO_VERSION(major, minor, patch)                                  \
    (major * 10000 + minor * 100 + patch)
#define HEPCE_VERSION                                                          \
    HEPCE_TO_VERSION(HEPCE_VER_MAJOR, HEPCE_VER_MINOR, HEPCE_VER_PATCH)

#endif // HEPCE_VERSION_HPP_