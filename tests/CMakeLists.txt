cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project(hepceTest CXX)


include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
        DataManagement::DataManagement
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "mocks/"
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
