# sanitizer.cmake
#
# Sanitizer instrumentation: the ENABLE_SANITIZER option (e.g.
# "address,undefined") adds -fsanitize flags to the current build, and the
# `sanitizer` build target performs a dedicated nested Debug build with
# ASan+UBSan and runs the tests/examples under it. The nested build lives
# in <build>/sanitizer and never overwrites the main build binaries.
#
# NOTE: this module must be included before the test/example targets are
# created (the flags are directory properties); the `sanitizer` target is
# created after all directories are processed (DEFER), when the global
# binary lists are complete.
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


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

## Create the `sanitizer` target: nested ASan+UBSan build + run.
function (http_client_lite_sanitizer_target)
  get_property (TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
  get_property (EXAMPLE_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_EXAMPLE_EXES)
  if (NOT TEST_EXES AND NOT EXAMPLE_EXES)
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

  set (SANITIZER_ENV
    "ASAN_OPTIONS=halt_on_error=1:detect_leaks=1"
    "UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1"
  )

  set (SANITIZER_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E echo
      "[sanitizer] configure nested ASan+UBSan build: ${SANITIZER_BUILD_DIR}"
    COMMAND
      ${CMAKE_COMMAND} ${SANITIZER_CONFIGURE_ARGS}
    COMMAND
      ${CMAKE_COMMAND} -E echo "[sanitizer] build"
    COMMAND
      ${CMAKE_COMMAND} --build "${SANITIZER_BUILD_DIR}" --parallel
  )

  # The tests run through ctest, the examples are executed directly (they
  # are not registered with ctest).
  if (HTTP_CLIENT_LITE_BUILD_TESTS)
    list (APPEND SANITIZER_COMMANDS
      COMMAND
        ${CMAKE_COMMAND} -E echo "[sanitizer] run tests"
      COMMAND
        ${CMAKE_COMMAND} -E env ${SANITIZER_ENV}
          "${CMAKE_CTEST_COMMAND}"
            --test-dir "${SANITIZER_BUILD_DIR}" --output-on-failure
    )
  endif (HTTP_CLIENT_LITE_BUILD_TESTS)

  if (HTTP_CLIENT_LITE_BUILD_EXAMPLES)
    list (APPEND SANITIZER_COMMANDS
      COMMAND
        ${CMAKE_COMMAND} -E echo "[sanitizer] run examples"
      COMMAND
        ${CMAKE_COMMAND} -E env ${SANITIZER_ENV}
          ${CMAKE_COMMAND}
            -DEXAMPLES_DIR=${SANITIZER_BUILD_DIR}/bin
            -P ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/run-examples.cmake
    )
  endif (HTTP_CLIENT_LITE_BUILD_EXAMPLES)

  list (APPEND SANITIZER_COMMANDS
    COMMAND
      ${CMAKE_COMMAND} -E echo "[sanitizer] done"
  )

  add_custom_target (
    sanitizer
    ${SANITIZER_COMMANDS}
    COMMENT
      "Nested sanitizer (ASan+UBSan) build and test/example run"
    USES_TERMINAL
    VERBATIM
  )
endfunction (http_client_lite_sanitizer_target)

if (HTTP_CLIENT_LITE_BUILD_TESTS OR HTTP_CLIENT_LITE_BUILD_EXAMPLES)
  cmake_language (DEFER DIRECTORY "${CMAKE_SOURCE_DIR}"
    CALL http_client_lite_sanitizer_target)
endif (HTTP_CLIENT_LITE_BUILD_TESTS OR HTTP_CLIENT_LITE_BUILD_EXAMPLES)
