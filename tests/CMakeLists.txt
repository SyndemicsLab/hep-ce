cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
project(hepceTest CXX)

find_package(DataManagement 1 REQUIRED
HINTS "${CMAKE_SOURCE_DIR}/lib/dminstall"
)
find_package(spdlog)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../bin)

enable_testing()

add_executable(${PROJECT_NAME}
  src/PersonTest.cpp
  src/SimTest.cpp
  src/CostTest.cpp
  src/UtilityTest.cpp
  src/DataWriterTest.cpp
  src/eventtests/EventTest.cpp
  src/eventtests/AgingTest.cpp
  src/eventtests/BehaviorChangesTest.cpp
  src/eventtests/ClearanceTest.cpp
  src/eventtests/DeathTest.cpp
  src/eventtests/FibrosisProgressionTest.cpp
  src/eventtests/FibrosisStagingTest.cpp
  src/eventtests/HCVInfectionsTest.cpp
  src/eventtests/HCVLinkingTest.cpp
  src/eventtests/HCVScreeningTest.cpp
  src/eventtests/HCVTreatmentTest.cpp
  src/eventtests/HIVInfectionsTest.cpp
  src/eventtests/HIVLinkingTest.cpp
  src/eventtests/HIVScreeningTest.cpp
  src/eventtests/VoluntaryRelinkingTest.cpp
  src/eventtests/PregnancyTest.cpp
  src/eventtests/OverdoseTest.cpp
  src/eventtests/MOUDTest.cpp
)

# BE VERY CAREFUL WITH GTEST AS ITS A PAIN IN THE BUTT
target_link_libraries(${PROJECT_NAME}
    PUBLIC
        GTest::gtest_main
        GTest::gmock_main
        data
#        fmt # Required for GTest
        person
        simulation
        event
        utils
        cost
        utility
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
