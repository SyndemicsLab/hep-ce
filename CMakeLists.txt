cmake_minimum_required(VERSION 3.27)
set(PRIVATE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(${PRIVATE_MODULE_PATH}/utils.cmake)
hepce_extract_version()

#-------------------------------------------------------------------------------
# Compiler config
#-------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#-------------------------------------------------------------------------------
# Project Setup
#-------------------------------------------------------------------------------
project(hepce VERSION ${HEPCE_VERSION} LANGUAGES CXX)
message(STATUS "Build hepce: ${HEPCE_VERSION}")

#-------------------------------------------------------------------------------
# Make all targets build to top level folders in build directory
#-------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#-------------------------------------------------------------------------------
# Set default build to release
#-------------------------------------------------------------------------------
# This is a standard recipe for setting a default build type
set(_default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${_default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE
        "${_default_build_type}"
        CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

#-------------------------------------------------------------------------------
# Windows CXX Extensions
#-------------------------------------------------------------------------------
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS" OR CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()


# ------------------------------------------------------------------------------
# Set HEPCE_MASTER_PROJECT to ON if we are building hepce directly
# ------------------------------------------------------------------------------
if(NOT DEFINED HEPCE_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(HEPCE_MASTER_PROJECT ON)
    else()
        set(HEPCE_MASTER_PROJECT OFF)
    endif()
endif()

#-------------------------------------------------------------------------------
# Set all default options
#-------------------------------------------------------------------------------
include(${PRIVATE_MODULE_PATH}/options.cmake)

# ------------------------------------------------------------------------------
# Set position independent code
# ------------------------------------------------------------------------------
if(HEPCE_BUILD_PIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# ------------------------------------------------------------------------------
# Set if the program stops execution on a warning
# ------------------------------------------------------------------------------
if(HEPCE_STOP_ON_WARNINGS)
    add_compile_definitions(EXIT_ON_WARNING)
endif()

#-------------------------------------------------------------------------------
# Building the Library
#-------------------------------------------------------------------------------
add_library(hepce_model)
add_library(hepce::hepce_model ALIAS hepce_model)

set(HEPCE_HEADERS
    include/hepce/hepce.hpp
    include/hepce/version.hpp
    include/hepce/data/inputs.hpp
    include/hepce/data/types.hpp
    include/hepce/data/writer.hpp
    include/hepce/event/event.hpp
    include/hepce/event/event_factory.hpp
    include/hepce/model/costing.hpp
    include/hepce/model/person.hpp
    include/hepce/model/sampler.hpp
    include/hepce/model/simulation.hpp
    include/hepce/model/utility.hpp
    include/hepce/utils/config.hpp
    include/hepce/utils/formatting.hpp
    include/hepce/utils/logging.hpp
    include/hepce/utils/math.hpp
    include/hepce/utils/pair_hashing.hpp
)

set(HEPCE_INTERNAL_HEADERS
    src/data/internals/writer_internals.hpp
    src/event/internals/aging_internals.hpp
    src/event/internals/all_events.hpp
    src/event/internals/base_event_internals.hpp
    src/event/internals/base_linking_internals.hpp
    src/event/internals/base_screening_internals.hpp
    src/event/internals/base_treatment_internals.hpp
    src/event/internals/behavior_changes_internals.hpp
    src/event/internals/death_internals.hpp
    src/event/internals/hcc_internals.hpp
    src/event/internals/hcv_clearance_internals.hpp
    src/event/internals/hcv_infection_internals.hpp
    src/event/internals/hcv_linking_internals.hpp
    src/event/internals/hcv_screening_internals.hpp
    src/event/internals/hcv_treatment_internals.hpp
    src/event/internals/hcv_voluntary_relink_internals.hpp
    src/event/internals/hiv_infection_internals.hpp
    src/event/internals/hiv_linking_internals.hpp
    src/event/internals/hiv_screening_internals.hpp
    src/event/internals/hiv_treatment_internals.hpp
    src/event/internals/moud_internals.hpp
    src/event/internals/overdose_internals.hpp
    src/event/internals/pregnancy_internals.hpp
    src/event/internals/progression_internals.hpp
    src/event/internals/staging_internals.hpp
    src/model/internals/costing_internals.hpp
    src/model/internals/person_internals.hpp
    src/model/internals/sampler_internals.hpp
    src/model/internals/simulation_internals.hpp
    src/model/internals/utility_internals.hpp
    src/utils/internals/logging_internals.hpp
)

set(HEPCE_SOURCE_FILES
    src/data/types.cpp
    src/data/writer.cpp
    src/event/aging.cpp
    src/event/behavior_changes.cpp
    src/event/death.cpp
    src/event/event_factory.cpp
    src/event/hcv_clearance.cpp
    src/event/hcv_infection.cpp
    src/event/hcv_linking.cpp
    src/event/hcv_screening.cpp
    src/event/hcv_treatment.cpp
    src/event/hcv_voluntary_relink.cpp
    src/event/hiv_infection.cpp
    src/event/hiv_linking.cpp
    src/event/hiv_screening.cpp
    src/event/hiv_treatment.cpp
    src/event/moud.cpp
    src/event/overdose.cpp
    src/event/pregnancy.cpp
    src/event/progression.cpp
    src/event/staging.cpp
    src/model/costing.cpp
    src/model/person.cpp
    src/model/sampler.cpp
    src/model/simulation.cpp
    src/model/utility.cpp
    src/utils/logging.cpp
)

target_sources(hepce_model
    PRIVATE
        ${HEPCE_INTERNAL_HEADERS}
        ${HEPCE_SOURCE_FILES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            include
        FILES
            ${HEPCE_HEADERS}
    )

#-------------------------------------------------------------------------------
# Linking Libraries
#-------------------------------------------------------------------------------
include(${PRIVATE_MODULE_PATH}/MakeDependenciesAvailable.cmake)

find_package(SQLiteCpp REQUIRED)
find_package(OpenMP REQUIRED)

set(private_deps spdlog::spdlog OpenMP::OpenMP_CXX)
if (HEPCE_CALCULATE_COVERAGE)
    list(APPEND private_deps gcov)
endif()

target_link_libraries(hepce_model
PUBLIC
    Boost::property_tree
    SQLiteCpp
PRIVATE
    ${private_deps}
)

#-------------------------------------------------------------------------------
# Setting library details
#-------------------------------------------------------------------------------
set_target_properties(hepce_model PROPERTIES VERSION ${HEPCE_VERSION} SOVERSION ${HEPCE_VERSION_MAJOR}.${HEPCE_VERSION_MINOR})

include(${PRIVATE_MODULE_PATH}/BuildBinaries.cmake)

#-------------------------------------------------------------------------------
# Install
#-------------------------------------------------------------------------------
include(GNUInstallDirs)
if(HEPCE_INSTALL)
    include(${PRIVATE_MODULE_PATH}/InstallHEPCE.cmake)
endif()

