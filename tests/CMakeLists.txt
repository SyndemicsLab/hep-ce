cmake_minimum_required(VERSION 3.24)
project(hepceTest CXX)

find_package(GTest REQUIRED)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)

enable_testing()

add_executable(${PROJECT_NAME}
  src/PersonTest.cpp
  src/SimTest.cpp
  src/EventTest.cpp
  src/CostTest.cpp
)

target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
        GTest::gtest 
        GTest::gtest_main
        person
        simulation
        event
        cost
        DataManagement::DataManagement
)

# gtest_discover_tests(test)
