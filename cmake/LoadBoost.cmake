message(CHECK_START "Fetching Boost...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(BOOST_ENABLE_CMAKE ON)

message(STATUS "Downloading and extracting boost library sources. This will take some time...")
FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.91.0-1/boost-1.91.0-1-cmake.7z # downloading a zip release speeds up the download
    USES_TERMINAL_DOWNLOAD TRUE 
    GIT_PROGRESS TRUE   
    DOWNLOAD_NO_EXTRACT FALSE
    OVERRIDE_FIND_PACKAGE TRUE
)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Boost Fetched")
