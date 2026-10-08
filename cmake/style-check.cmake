# Pre-build code style check (clang-format --dry-run), gated by the
# STYLE_CHECK option (ON by default). Any violation fails the build.
# clang-format is located and verified by check-tools.cmake.

if (STYLE_CHECK)
  file (
    GLOB_RECURSE
    STYLE_SOURCE_FILES
      ${CMAKE_SOURCE_DIR}/include/*.hpp
      ${CMAKE_SOURCE_DIR}/tests/*.cpp
      ${CMAKE_SOURCE_DIR}/tests/*.hpp
      ${CMAKE_SOURCE_DIR}/examples/*.cpp
      ${CMAKE_SOURCE_DIR}/examples/*.hpp
  )

  add_custom_target (
    style_check
    ALL
    COMMAND
      ${CLANG_FORMAT} -style=file --dry-run -Werror ${STYLE_SOURCE_FILES}
    COMMENT
      "Checking code style"
    VERBATIM
  )
endif ()
