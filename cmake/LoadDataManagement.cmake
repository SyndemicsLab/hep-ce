message(CHECK_START "Searching for DataManagement...")
include(FetchContent)
FetchContent_Declare(
    datamanagement
    GIT_REPOSITORY https://github.com/SyndemicsLab/DataManagement.git
    GIT_TAG old_dm
)
option(DATAMANAGEMENT_INSTALL "Enable install for datamanagement project" ON)
option(DATAMANAGEMENT_BUILD_TESTS "Disable testing for DataManagement" OFF)
FetchContent_MakeAvailable(datamanagement)
list(APPEND CMAKE_MODULE_PATH ${datamanagement_SOURCE_DIR}/cmake)
message(CHECK_PASS "Found DataManagement")
