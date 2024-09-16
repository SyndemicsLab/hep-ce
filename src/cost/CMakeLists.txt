cmake_minimum_required(VERSION 3.19)

project(cost)

find_package(spdlog REQUIRED)

add_library(cost
  Cost.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
