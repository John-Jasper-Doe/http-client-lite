# Sanitizer instrumentation: the ENABLE_SANITIZER option (e.g.
# "address,undefined") adds -fsanitize flags to the current build, and the
# `sanitizer` build target performs a dedicated nested Debug build with
# ASan+UBSan and runs the tests under it. The nested build lives in
# <build>/sanitizer and never overwrites the main build binaries.
#
# NOTE: this module must be included before the test/example targets are
# created (the flags are directory properties); the `sanitizer` target is
# created after all directories are processed (DEFER), when the global
# test list is complete.

set (ENABLE_SANITIZER "" CACHE STRING
  "Comma-separated list of sanitizers (GCC/Clang), e.g.: address,undefined")

if (ENABLE_SANITIZER)
  if (NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU|Clang")
    message (FATAL_ERROR
      "ENABLE_SANITIZER is only supported with GCC or Clang"
      " (got ${CMAKE_CXX_COMPILER_ID})")
  endif ()
  add_compile_options (-fsanitize=${ENABLE_SANITIZER} -fno-omit-frame-pointer -g)
  add_link_options (-fsanitize=${ENABLE_SANITIZER})
  message (STATUS "- Sanitizers enabled: ${ENABLE_SANITIZER}")
endif (ENABLE_SANITIZER)

## Create the `sanitizer` target: nested ASan+UBSan build + test run.
function (http_client_lite_sanitizer_target)
  get_property (TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
  if (NOT TEST_EXES)
    return ()
  endif ()

  set (SANITIZER_BUILD_DIR ${CMAKE_BINARY_DIR}/sanitizer)

  set (SANITIZER_CONFIGURE_ARGS
    "-S" "${CMAKE_SOURCE_DIR}"
    "-B" "${SANITIZER_BUILD_DIR}"
    "-G" "${CMAKE_GENERATOR}"
    "-DCMAKE_BUILD_TYPE=Debug"
    "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${SANITIZER_BUILD_DIR}/bin"
    "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${SANITIZER_BUILD_DIR}/bin"
    "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${SANITIZER_BUILD_DIR}/bin"
    "-DHTTP_CLIENT_LITE_BUILD_TESTS=${HTTP_CLIENT_LITE_BUILD_TESTS}"
    "-DHTTP_CLIENT_LITE_BUILD_EXAMPLES=${HTTP_CLIENT_LITE_BUILD_EXAMPLES}"
    "-DHTTP_CLIENT_LITE_AMALGAMATE=${HTTP_CLIENT_LITE_AMALGAMATE}"
    "-DHTTP_CLIENT_LITE_STYLE_CHECK=OFF"
    "-DENABLE_SANITIZER=address,undefined"
  )

  # Reuse the already fetched Catch2 sources in the nested build.
  if (DEFINED catch2test_SOURCE_DIR)
    list (APPEND SANITIZER_CONFIGURE_ARGS
      "-DFETCHCONTENT_SOURCE_DIR_CATCH2TEST=${catch2test_SOURCE_DIR}")
  endif (DEFINED catch2test_SOURCE_DIR)

  add_custom_target (
    sanitizer
    COMMAND
      ${CMAKE_COMMAND} -E echo
        "[sanitizer] configure nested ASan+UBSan build: ${SANITIZER_BUILD_DIR}"
    COMMAND
      ${CMAKE_COMMAND} ${SANITIZER_CONFIGURE_ARGS}
    COMMAND
      ${CMAKE_COMMAND} -E echo "[sanitizer] build"
    COMMAND
      ${CMAKE_COMMAND} --build "${SANITIZER_BUILD_DIR}" --parallel
    COMMAND
      ${CMAKE_COMMAND} -E echo "[sanitizer] run tests"
    COMMAND
      ${CMAKE_COMMAND} -E env
        "ASAN_OPTIONS=halt_on_error=1:detect_leaks=1"
        "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
        "${CMAKE_CTEST_COMMAND}"
          --test-dir "${SANITIZER_BUILD_DIR}" --output-on-failure
    COMMAND
      ${CMAKE_COMMAND} -E echo "[sanitizer] done"
    COMMENT
      "Nested sanitizer (ASan+UBSan) build and test run"
    USES_TERMINAL
    VERBATIM
  )
endfunction (http_client_lite_sanitizer_target)

if (HTTP_CLIENT_LITE_BUILD_TESTS)
  cmake_language (DEFER DIRECTORY "${CMAKE_SOURCE_DIR}"
    CALL http_client_lite_sanitizer_target)
endif (HTTP_CLIENT_LITE_BUILD_TESTS)
