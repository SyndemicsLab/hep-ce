cmake_minimum_required(VERSION 3.19)

project(cost)

find_package(spdlog REQUIRED)

# find_package(DataManagement REQUIRED)

add_library(cost
  Cost.cpp
)

set(COST_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")

target_include_directories(cost PUBLIC ${COST_INCLUDES} ${DataManagement_INCLUDE_DIR})

# target_link_libraries(${PROJECT_NAME} PUBLIC person)
target_link_libraries(${PROJECT_NAME} PUBLIC utils)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
# target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement)
