include(FetchContent)
message(CHECK_START "SQLiteCpp Not Found, Fetching...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
FetchContent_Declare(
    SQLiteCpp
    GIT_REPOSITORY  https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG         master
    GIT_PROGRESS    TRUE
)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "SQLiteCpp Fetched")