cmake_minimum_required(VERSION 3.27)
project(hepce_tests VERSION ${HEPCE_VERSION} LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL OR NOT HEPCE_BUILD_TESTS)
    message(STATUS "Attempted to Build tests, but HEPCE_BUILD_TESTS is OFF!")
    return()
endif()


enable_testing()

add_executable(hepce_tests)

file(GLOB_RECURSE HEPCE_TEST_FILES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB HEPCE_TEST_MOCKS CONFIGURE_DEPENDS mocks/*.hpp)
file(GLOB HEPCE_TEST_CONSTANTS CONFIGURE_DEPENDS constants/*.hpp)

target_sources(hepce_tests
    PRIVATE
        ${HEPCE_TEST_MOCKS}
        ${HEPCE_TEST_CONSTANTS}
        ${HEPCE_TEST_FILES}
    )

target_link_libraries(hepce_tests
    PUBLIC
        hepce_model
        Boost::property_tree
        GTest::gmock_main
        spdlog::spdlog
)

target_include_directories(hepce_tests
    PRIVATE
        "mocks/"
        "constants/"
)

set(HEPCE_TEST_ARTIFACT_DIR ${CMAKE_BINARY_DIR}/test_artifacts)
file(MAKE_DIRECTORY ${HEPCE_TEST_ARTIFACT_DIR})

gtest_discover_tests(${PROJECT_NAME}
    WORKING_DIRECTORY ${HEPCE_TEST_ARTIFACT_DIR}
)
