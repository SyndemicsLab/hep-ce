project("GoogleTest")
enable_testing()
include(GoogleTest)

include_directories(mocks)

# Find Eigen
find_package (Eigen3 3.4 REQUIRED)

add_executable(HEPCESIMULATION_TEST)

target_include_directories(HEPCESIMULATION_TEST INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(HEPCESIMULATION_TEST
  Eigen3::Eigen
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  GTest::gmock_main
)

# create the test data set in the build directory
add_custom_command(
        TARGET HEPCESIMULATION_TEST POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/test/TestInput
                ${CMAKE_BINARY_DIR}/TestInput)

gtest_discover_tests(HEPCESIMULATION_TEST)
