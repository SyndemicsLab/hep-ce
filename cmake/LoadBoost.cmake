message(CHECK_START "Fetching Boost...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(BOOST_INCLUDE_LIBRARIES property_tree)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG 460b20ab28ccb347804e5a5cc7ff5ada07028496 # v1.90.0
    GIT_SHALLOW TRUE
)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Boost Fetched")
