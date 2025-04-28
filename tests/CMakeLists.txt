cmake_minimum_required(VERSION 3.27)
project(hepce_tests VERSION ${HEPCE_VERSION} LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL OR NOT HEPCE_BUILD_TESTS)
    message(STATUS "Attempted to Build tests, but HEPCE_BUILD_TESTS is OFF!")
    return()
endif()


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
        hepce
        GTest::gmock_main
        datamanagement::datamanagement
        spdlog::spdlog
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        "mocks/"
)

gtest_discover_tests(${PROJECT_NAME})
