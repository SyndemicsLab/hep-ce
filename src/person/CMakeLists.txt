cmake_minimum_required(VERSION 3.20)

project(person)

find_package(spdlog REQUIRED)

find_package(DataManagement REQUIRED)

add_library(person Person.cpp)

set(PERSON_INCLUDES "${CMAKE_CURRENT_LIST_DIR}/include")
target_include_directories(person PUBLIC ${PERSON_INCLUDES} ${DataManagement_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} PUBLIC utils)

target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement)