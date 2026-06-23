message(CHECK_START "Fetching GoogleTest...")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  OVERRIDE_FIND_PACKAGE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
list(POP_BACK CMAKE_MESSAGE_INDENT)
message(CHECK_PASS "GoogleTest Fetched")
