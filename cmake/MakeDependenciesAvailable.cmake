include(FetchContent)

include(LoadSpdlog)
include(LoadDataManagement)

if(HEPCE_BUILD_TESTS STREQUAL "OFF")
    FetchContent_MakeAvailable(spdlog datamanagement)
elseif(HEPCE_BUILD_TESTS STREQUAL "ON")
    include(LoadGtest)
    FetchContent_MakeAvailable(spdlog googletest datamanagement)
    include(GoogleTest)
endif()
