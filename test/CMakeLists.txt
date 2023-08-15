project("GoogleTest")
enable_testing()
include(GoogleTest)

include_directories(mocks)

add_executable(HEPCESIMULATION_TEST
  PersonTest.cpp
)

target_include_directories(HEPCESIMULATION_TEST INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(HEPCESIMULATION_TEST
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
)

gtest_discover_tests(HEPCESIMULATION_TEST)
