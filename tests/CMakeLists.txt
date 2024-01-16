cmake_minimum_required(VERSION 3.24)
project(hepceTest CXX)

find_package(GTest REQUIRED CONFIG)

find_package(spdlog REQUIRED)

find_package(Boost REQUIRED COMPONENTS filesystem)

find_package(DataManagement REQUIRED)

add_executable(${PROJECT_NAME}
  PersonTest.cpp
  SimTest.cpp
  EventTest.cpp
)

target_include_directories(${PROJECT_NAME} INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${Boost_INCLUDE_DIRS}
  "mocks/"
)

target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest_main)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gmock)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gmock_main)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem)
target_link_libraries(${PROJECT_NAME} PUBLIC person)
target_link_libraries(${PROJECT_NAME} PUBLIC simulation)
target_link_libraries(${PROJECT_NAME} PUBLIC event)
target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog)
target_link_libraries(${PROJECT_NAME} PUBLIC DataManagement)

# gtest_discover_tests(test)
