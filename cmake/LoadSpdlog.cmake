message(CHECK_START "Fetching spdlog...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
include(FetchContent)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY  https://github.com/gabime/spdlog.git
        GIT_TAG         v1.x
        GIT_PROGRESS    TRUE
        FIND_PACKAGE_ARGS NAMES spdlog
)
set(SPDLOG_INSTALL ON)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "spdlog Fetched")