cmake_minimum_required(VERSION 3.19)

project(cost)

if(BUILD_SHARED_LIBS)
  add_library(${PROJECT_NAME} SHARED)
else()
  add_library(${PROJECT_NAME} STATIC)
endif()

target_sources(${PROJECT_NAME}
    PRIVATE
        Cost.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
