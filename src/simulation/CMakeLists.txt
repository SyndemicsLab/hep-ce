cmake_minimum_required(VERSION 3.20)

project(simulation)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

find_package(OpenMP REQUIRED)

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
        spdlog::spdlog
        DataManagement::DataManagement
        OpenMP::OpenMP_CXX
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")