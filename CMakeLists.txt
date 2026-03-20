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

file(GLOB_RECURSE HEPCE_HEADERS include/hepce/*.hpp)
file(GLOB HEPCE_INTERNAL_HEADERS CONFIGURE_DEPENDS src/internals/*.hpp)
file(GLOB HEPCE_SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)

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

