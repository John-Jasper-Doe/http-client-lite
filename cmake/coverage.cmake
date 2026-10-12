# coverage.cmake
#
# Coverage instrumentation: the ENABLE_COVERAGE option adds --coverage to
# the current build, and the `coverage` build target performs a dedicated
# nested Debug build, runs the tests/examples and prints a per-file
# line/branch summary for the project headers (cmake/coverage-summary.sh).
# When gcovr is installed the script also writes an HTML report.
#
# NOTE: this module must be included before the test/example targets are
# created (the flags are directory properties); the `coverage` target is
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


option (ENABLE_COVERAGE "Instrument the build for coverage (--coverage)" OFF)

if (ENABLE_COVERAGE)
  if (NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU|Clang")
    message (FATAL_ERROR
      "ENABLE_COVERAGE is only supported with GCC or Clang"
      " (got ${CMAKE_CXX_COMPILER_ID})")
  endif ()
  add_compile_options (--coverage -O0 -g)
  add_link_options (--coverage)
  message (STATUS "- Coverage instrumentation enabled (--coverage)")
endif (ENABLE_COVERAGE)

## Create the `coverage` target: nested build + test/example run + summary.
function (http_client_lite_coverage_target)
  get_property (TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
  get_property (EXAMPLE_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_EXAMPLE_EXES)
  if (NOT TEST_EXES AND NOT EXAMPLE_EXES)
    return ()
  endif ()

  find_program (GCOV "gcov")
  if (NOT GCOV)
    message (STATUS "- gcov not found: the 'coverage' target is disabled")
    return ()
  endif ()
  message (STATUS "- Found gcov: ${GCOV}")

  find_program (GCOVR "gcovr")
  if (GCOVR)
    message (STATUS "- Found gcovr: ${GCOVR} (an HTML report will be generated)")
  else ()
    message (STATUS
      "- gcovr not found: HTML report disabled (sudo apt-get install gcovr)")
  endif (GCOVR)

  set (COVERAGE_REPORT_DIR ${CMAKE_BINARY_DIR}/coverage)
  set (COVERAGE_BUILD_DIR ${COVERAGE_REPORT_DIR}/build)

  set (COVERAGE_CONFIGURE_ARGS
    "-S" "${CMAKE_SOURCE_DIR}"
    "-B" "${COVERAGE_BUILD_DIR}"
    "-G" "${CMAKE_GENERATOR}"
    "-DCMAKE_BUILD_TYPE=Debug"
    "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${COVERAGE_BUILD_DIR}/bin"
    "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${COVERAGE_BUILD_DIR}/bin"
    "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${COVERAGE_BUILD_DIR}/bin"
    "-DHTTP_CLIENT_LITE_BUILD_TESTS=${HTTP_CLIENT_LITE_BUILD_TESTS}"
    "-DHTTP_CLIENT_LITE_BUILD_EXAMPLES=${HTTP_CLIENT_LITE_BUILD_EXAMPLES}"
    "-DHTTP_CLIENT_LITE_AMALGAMATE=${HTTP_CLIENT_LITE_AMALGAMATE}"
    "-DHTTP_CLIENT_LITE_STYLE_CHECK=OFF"
    "-DENABLE_COVERAGE=ON"
  )

  # Reuse the already fetched Catch2 sources in the nested build.
  if (DEFINED catch2test_SOURCE_DIR)
    list (APPEND COVERAGE_CONFIGURE_ARGS
      "-DFETCHCONTENT_SOURCE_DIR_CATCH2TEST=${catch2test_SOURCE_DIR}")
  endif (DEFINED catch2test_SOURCE_DIR)

  set (COVERAGE_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E echo
      "[coverage] configure nested build: ${COVERAGE_BUILD_DIR}"
    COMMAND
      ${CMAKE_COMMAND} ${COVERAGE_CONFIGURE_ARGS}
    COMMAND
      ${CMAKE_COMMAND} -E echo "[coverage] build"
    COMMAND
      ${CMAKE_COMMAND} --build "${COVERAGE_BUILD_DIR}" --parallel
  )

  # The tests run through ctest, the examples are executed directly (they
  # are not registered with ctest); both feed the same .gcda counters.
  if (HTTP_CLIENT_LITE_BUILD_TESTS)
    list (APPEND COVERAGE_COMMANDS
      COMMAND
        ${CMAKE_COMMAND} -E echo "[coverage] run tests"
      COMMAND
        ${CMAKE_CTEST_COMMAND}
          --test-dir "${COVERAGE_BUILD_DIR}" --output-on-failure
    )
  endif (HTTP_CLIENT_LITE_BUILD_TESTS)

  if (HTTP_CLIENT_LITE_BUILD_EXAMPLES)
    list (APPEND COVERAGE_COMMANDS
      COMMAND
        ${CMAKE_COMMAND} -E echo "[coverage] run examples"
      COMMAND
        ${CMAKE_COMMAND}
          -DEXAMPLES_DIR=${COVERAGE_BUILD_DIR}/bin
          -P ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/run-examples.cmake
    )
  endif (HTTP_CLIENT_LITE_BUILD_EXAMPLES)

  list (APPEND COVERAGE_COMMANDS
    COMMAND
      ${CMAKE_COMMAND} -E echo "[coverage] summarize"
    COMMAND
      "${CMAKE_SOURCE_DIR}/cmake/coverage-summary.sh"
        "${COVERAGE_BUILD_DIR}"
        "${CMAKE_SOURCE_DIR}"
        "${COVERAGE_REPORT_DIR}"
  )

  add_custom_target (
    coverage
    ${COVERAGE_COMMANDS}
    COMMENT
      "Nested coverage build, test/example run and summary"
    USES_TERMINAL
    VERBATIM
  )
endfunction (http_client_lite_coverage_target)

if (HTTP_CLIENT_LITE_BUILD_TESTS OR HTTP_CLIENT_LITE_BUILD_EXAMPLES)
  cmake_language (DEFER DIRECTORY "${CMAKE_SOURCE_DIR}"
    CALL http_client_lite_coverage_target)
endif (HTTP_CLIENT_LITE_BUILD_TESTS OR HTTP_CLIENT_LITE_BUILD_EXAMPLES)
