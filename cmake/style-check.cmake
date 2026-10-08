# Pre-build code style check (clang-format --dry-run), gated by the
# STYLE_CHECK option (ON by default). Any violation fails the build.

find_program (CLANG_FORMAT "clang-format")

if (STYLE_CHECK)
  if (NOT CLANG_FORMAT)
    message (FATAL_ERROR
      "STYLE_CHECK is ON but clang-format was not found! "
      "Install clang-format or configure with -DSTYLE_CHECK=OFF"
    )
  endif ()

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
