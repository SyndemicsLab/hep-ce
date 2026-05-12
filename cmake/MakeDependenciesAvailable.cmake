include(FetchContent)

include(${PRIVATE_MODULE_PATH}/LoadSpdlog.cmake)
include(${PRIVATE_MODULE_PATH}/LoadBoost.cmake)

if(HEPCE_BUILD_TESTS STREQUAL "OFF")
    FetchContent_MakeAvailable(spdlog Boost)
elseif(HEPCE_BUILD_TESTS STREQUAL "ON")
    include(${PRIVATE_MODULE_PATH}/LoadGtest.cmake)
    FetchContent_MakeAvailable(spdlog googletest Boost)
    include(GoogleTest)
endif()
