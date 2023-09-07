project("GoogleTest")
enable_testing()
include(GoogleTest)

include_directories(mocks)

add_executable(test
  PersonTest.cpp
  SimTest.cpp
)

target_include_directories(test INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(test
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
  person
  simulation
)

gtest_discover_tests(test)
