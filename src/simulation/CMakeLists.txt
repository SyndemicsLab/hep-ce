cmake_minimum_required(VERSION 3.20)

project(simulation)

include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
)
FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()

find_package(OpenMP REQUIRED)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

target_sources(${PROJECT_NAME} 
    PRIVATE
        Simulation.cpp 
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        data
        person
        event
        stats
        utils
        spdlog::spdlog
        DataManagement::DataManagement
        OpenMP::OpenMP_CXX
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")