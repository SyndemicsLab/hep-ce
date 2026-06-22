message(CHECK_START "Fetching SQLiteCpp...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
include(FetchContent)
FetchContent_Declare(
        SQLiteCpp
        GIT_REPOSITORY  https://github.com/SRombauts/SQLiteCpp.git
        GIT_TAG         d66a92a53dc4c333e8491584a8ca452dc058c977 
        OVERRIDE_FIND_PACKAGE
)
# Find your own dependencies
# set(SQLITECPP_INTERNAL_SQLITE OFF)
# Dependency, I'm not checking for style
set(SQLITECPP_RUN_CPPLINT OFF)
# Dependency, not checking for static analysis
set(SQLITECPP_RUN_CPPCHECK OFF)
# No tests
set(SQLITECPP_BUILD_TESTS OFF)

list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "SQLiteCpp Fetched")