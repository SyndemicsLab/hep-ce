cmake_minimum_required(VERSION 3.27)
project(hepce_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard to compile with, 17")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if(CMAKE_SYSTEM_NAME MATCHES "CYGWIN" OR CMAKE_SYSTEM_NAME MATCHES "MSYS" OR CMAKE_SYSTEM_NAME MATCHES "MINGW")
    set(CMAKE_CXX_EXTENSIONS ON)
endif()

if($ENV{FORCE_COLOR})
    if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif()
endif()

if(PROJECT_IS_TOP_LEVEL OR NOT HEPCE_BUILD_TESTS)
    message(STATUS "Attempted to Build tests, but HEPCE_BUILD_TESTS is OFF!")
    return()
endif()


file(GLOB HEPCE_TEST_MOCKS CONFIGURE_DEPENDS mocks/*.hpp)
file(GLOB HEPCE_TEST_CONSTANTS CONFIGURE_DEPENDS constants/*.hpp)
file(GLOB_RECURSE HEPCE_UNIT_TESTS CONFIGURE_DEPENDS unit/*.cpp)

add_executable(hepce_tests)
# Not sure why, but CMake is not inferring the linker language correctly
set_target_properties(hepce_tests PROPERTIES LINKER_LANGUAGE CXX)

target_sources(hepce_tests
    PRIVATE
        ${HEPCE_TEST_MOCKS}
        ${HEPCE_TEST_CONSTANTS}
        ${HEPCE_UNIT_TESTS}
    )

target_include_directories(hepce_tests
    PUBLIC
        ${Boost_INCLUDE_DIRS}
    PRIVATE
        "mocks/"
        "constants/"
)

target_link_libraries(hepce_tests
    PUBLIC
        hepce_model
        GTest::gmock_main
        spdlog::spdlog
)

gtest_discover_tests(hepce_tests
    TEST_PREFIX "hepce_"
)
