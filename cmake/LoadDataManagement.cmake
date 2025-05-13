message(CHECK_START "Searching for DataManagement...")
include(FetchContent)
FetchContent_Declare(
    datamanagement
    GIT_REPOSITORY https://github.com/SyndemicsLab/DataManagement.git
    GIT_TAG hotfix/sqlitecpp
)
option(DATAMANAGEMENT_INSTALL "Enable install for datamanagement project" ON)
option(DATAMANAGEMENT_BUILD_TESTS "Disable testing for DataManagement" OFF)
message(CHECK_PASS "Found DataManagement")
