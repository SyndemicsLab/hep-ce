cmake_minimum_required(VERSION 3.20)

project(person)

find_package(spdlog REQUIRED)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

add_library(${PROJECT_NAME} 
    Person.cpp 
    ContainerOperators.cpp
)
target_link_libraries(${PROJECT_NAME} PUBLIC utils)
target_link_libraries(${PROJECT_NAME} PUBLIC cost)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement::DataManagement)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
