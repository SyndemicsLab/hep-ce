cmake_minimum_required(VERSION 3.20)

project(stats)
find_package(spdlog)

if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

target_sources(${PROJECT_NAME}
    PRIVATE
        Decider.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
