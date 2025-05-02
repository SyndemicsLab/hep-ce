cmake_minimum_required(VERSION 3.27)
project(hepce_tests VERSION ${HEPCE_VERSION} LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL OR NOT HEPCE_BUILD_TESTS)
    message(STATUS "Attempted to Build tests, but HEPCE_BUILD_TESTS is OFF!")
    return()
endif()


enable_testing()

add_executable(${PROJECT_NAME}
    src/event/aging_test.cpp
    src/event/death_test.cpp
    src/data/writer_test.cpp
    src/utils/logging_test.cpp

#   src/model/person_test.cpp
#   src/model/simulation_test.cpp
#   src/model/cost_test.cpp
#   src/event/BehaviorChangesTest.cpp
#   src/event/ClearanceTest.cpp
#   src/event/DeathTest.cpp
#   src/event/FibrosisProgressionTest.cpp
#   src/event/FibrosisStagingTest.cpp
#   src/event/HCVInfectionsTest.cpp
#   src/event/HCVLinkingTest.cpp
#   src/event/HCVScreeningTest.cpp
#   src/event/HCVTreatmentTest.cpp
#   src/event/HIVInfectionsTest.cpp
#   src/event/HIVLinkingTest.cpp
#   src/event/HIVScreeningTest.cpp
#   src/event/VoluntaryRelinkingTest.cpp
#   src/event/PregnancyTest.cpp
#   src/event/OverdoseTest.cpp
#   src/event/MOUDTest.cpp
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
    PRIVATE
        "mocks/"
        "constants/"
)

gtest_discover_tests(${PROJECT_NAME})
