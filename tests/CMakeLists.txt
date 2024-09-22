cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project(hepceTest CXX)


include(FetchContent)

# TESTING AND MOCKING DEPENDENCIES
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)


# LOGGING DEPENDENCY
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.x
)
FetchContent_GetProperties(spdlog)
if (NOT spdlog_POPULATED)
    FetchContent_Populate(spdlog)
    add_subdirectory(${spdlog_SOURCE_DIR} ${spdlog_BINARY_DIR})
endif()

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

# BE VERY CAREFUL WITH GTEST AS ITS A PAIN IN THE BUTT
target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
        GTest::gtest_main
        GTest::gmock_main
        fmt # Required for GTest
        person
        simulation
        event
        cost
        stats
        DataManagement::DataManagement
        spdlog::spdlog
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "mocks/"
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
