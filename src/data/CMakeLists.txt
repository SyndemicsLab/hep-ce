cmake_minimum_required(VERSION 3.20)

project(data)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_library(data DataWriter.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC cost)
target_link_libraries(${PROJECT_NAME} PUBLIC person)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement::DataManagement)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
