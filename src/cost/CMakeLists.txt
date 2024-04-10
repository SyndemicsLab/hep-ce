cmake_minimum_required(VERSION 3.19)

project(cost)

find_package(spdlog REQUIRED)

add_library(cost
  Cost.cpp
)

set(COST_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(${PROJECT_NAME} PUBLIC ${COST_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC utils)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
