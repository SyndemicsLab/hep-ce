message(CHECK_START "Fetching Boost...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(BOOST_INCLUDE_LIBRARIES property_tree)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
    Boost
    GIT_REPOSITORY https://github.com/boostorg/boost.git
    GIT_TAG 1bed2b0712b2119f20d66c5053def9173c8462a5 # v1.90.0
    GIT_SHALLOW TRUE
)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "Boost Fetched")
