# additional target to perform clang-tidy run, requires clang-tidy

# Adding clang-tidy target if executable is found
find_program (CLANG_TIDY "clang-tidy")

if (CLANG_TIDY)
  message (STATUS "Clang-tidy is found!")

  set (CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY})
endif ()




