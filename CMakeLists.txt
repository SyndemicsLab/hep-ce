cmake_minimum_required(VERSION 3.27)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(utils)
hepce_extract_version()

project(hepce VERSION ${HEPCE_VERSION} LANGUAGES CXX)

message(STATUS "Build hepce: ${HEPCE_VERSION}")

include(GNUInstallDirs)

# ---------------------------------------------------------------------------------------
# Set default build to release
# ---------------------------------------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose Release or Debug" FORCE)
endif()

# ---------------------------------------------------------------------------------------
# Compiler config
# ---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------------------
# Windows CXX Extensions
# ---------------------------------------------------------------------------------------
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS" OR CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

# ---------------------------------------------------------------------------------------
# Set HEPCE_MASTER_PROJECT to ON if we are building hepce directly
# ---------------------------------------------------------------------------------------
if(NOT DEFINED HEPCE_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(HEPCE_MASTER_PROJECT ON)
    else()
        set(HEPCE_MASTER_PROJECT OFF)
    endif()
endif()

# ---------------------------------------------------------------------------------------
# Set all default options
# ---------------------------------------------------------------------------------------
include(options)

# ---------------------------------------------------------------------------------------
# Set position independent code
# ---------------------------------------------------------------------------------------
if(HEPCE_BUILD_PIC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

# ---------------------------------------------------------------------------------------
# Set if the program stops execution on a warning
# ---------------------------------------------------------------------------------------
if(HEPCE_STOP_ON_WARNINGS)
    add_compile_definitions(EXIT_ON_WARNING)
endif()

# ---------------------------------------------------------------------------------------
# Setting the Source Files
# ---------------------------------------------------------------------------------------
set(HEPCE_DATA
    src/data/writer.cpp
    src/data/types.cpp
)

set(HEPCE_EVENT_BASE
    src/event/base/aging.cpp
    src/event/base/death.cpp
)

set(HEPCE_EVENT_BEHAVIOR
    src/event/behavior/behavior_changes.cpp
    src/event/behavior/pregnancy.cpp
    src/event/behavior/moud.cpp
    src/event/behavior/overdose.cpp
)

set(HEPCE_EVENT_FIBROSIS
    src/event/fibrosis/progression.cpp
    src/event/fibrosis/staging.cpp
)

set(HEPCE_EVENT_HCV
    src/event/hcv/clearance.cpp
    src/event/hcv/infection.cpp
    src/event/hcv/linking.cpp
    src/event/hcv/screening.cpp
    src/event/hcv/treatment.cpp
    src/event/hcv/voluntary_relink.cpp
)

set(HEPCE_EVENT_HIV
    src/event/hiv/infection.cpp
    src/event/hiv/linking.cpp
    src/event/hiv/screening.cpp
    src/event/hiv/treatment.cpp
)

set(HEPCE_EVENT
    ${HEPCE_EVENT_BASE}
    ${HEPCE_EVENT_BEHAVIOR}
    ${HEPCE_EVENT_FIBROSIS}
    ${HEPCE_EVENT_HCV}
    ${HEPCE_EVENT_HIV}
)

set(HEPCE_MODEL
    src/model/costing.cpp
    src/model/person.cpp
    src/model/simulation.cpp
    src/model/utility.cpp
    src/model/sampler.cpp
)

set(HEPCE_UTILS
    src/utils/logging.cpp
)

set(HEPCE_SRCS 
    ${HEPCE_DATA}
    ${HEPCE_EVENT}
    ${HEPCE_MODEL}
    ${HEPCE_UTILS}
)

# ---------------------------------------------------------------------------------------
# Building the Library
# ---------------------------------------------------------------------------------------
if(HEPCE_BUILD_SHARED_LIBS)
    if(WIN32)
        configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in ${CMAKE_CURRENT_BINARY_DIR}/version.rc @ONLY)
        list(APPEND HEPCE_SRCS ${CMAKE_CURRENT_BINARY_DIR}/version.rc)
    endif()
    add_library(hepce SHARED ${HEPCE_SRCS} ${HEPCE_ALL_HEADERS})
    target_compile_definitions(hepce PUBLIC HEPCE_SHARED_LIB)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(hepce PUBLIC $<$<AND:$<CXX_COMPILER_ID:MSVC>,$<NOT:$<COMPILE_LANGUAGE:CUDA>>>:/wd4251
                                             /wd4275>)
    endif()
else()
    add_library(hepce STATIC ${HEPCE_SRCS} ${HEPCE_ALL_HEADERS})
endif()

# ---------------------------------------------------------------------------------------
# Alias for importing
# ---------------------------------------------------------------------------------------
add_library(hepce::hepce ALIAS hepce)

set(HEPCE_INCLUDES_LEVEL "")
if(HEPCE_SYSTEM_INCLUDES)
    set(HEPCE_INCLUDES_LEVEL "SYSTEM")
endif()

target_compile_definitions(hepce PUBLIC HEPCE_COMPILED_LIB)
target_include_directories(hepce ${HEPCE_INCLUDES_LEVEL} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>"
                                                                  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
                                                                  PRIVATE "$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>")

include(MakeDependenciesAvailable)

if(HEPCE_RUN_OMP)
    find_package(OpenMP REQUIRED)
    target_link_libraries(hepce
    PUBLIC
        datamanagement::datamanagement
    PRIVATE
        spdlog::spdlog
        OpenMP::OpenMP_CXX
    )    
elseif(HEPCE_CALCULATE_COVERAGE)
    target_link_libraries(hepce
    PUBLIC
        datamanagement::datamanagement
    PRIVATE
        spdlog::spdlog
        gcov
    )
else()
    target_link_libraries(hepce
    PUBLIC
        datamanagement::datamanagement
    PRIVATE
        spdlog::spdlog
    )
endif()



set_target_properties(hepce PROPERTIES VERSION ${HEPCE_VERSION} SOVERSION ${HEPCE_VERSION_MAJOR}.${HEPCE_VERSION_MINOR})

include(BuildBinaries)

# ---------------------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------------------
if(HEPCE_INSTALL)
    include(InstallHepce)
endif()

