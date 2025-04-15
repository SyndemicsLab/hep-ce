message(CHECK_START "Fetching GTest...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 2ae29b52fdff88c52fef655fa0d245fc514ca35b
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  FIND_PACKAGE_ARGS NAMES GTest
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
