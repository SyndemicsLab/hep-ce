include(FetchContent)

include(${PRIVATE_MODULE_PATH}/LoadSpdlog.cmake)
include(${PRIVATE_MODULE_PATH}/LoadSQLiteCpp.cmake)

if(HEPCE_BUILD_TESTS STREQUAL "ON")
    include(${PRIVATE_MODULE_PATH}/LoadGtest.cmake)
    FetchContent_MakeAvailable(SQLiteCpp spdlog googletest)
    include(GoogleTest)
elseif(HEPCE_BUILD_TESTS STREQUAL "OFF")
    FetchContent_MakeAvailable(SQLiteCpp spdlog)
endif()
