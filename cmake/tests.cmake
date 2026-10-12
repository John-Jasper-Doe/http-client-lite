# tests.cmake
#
# Test infrastructure: fetches Catch2, defines the tests, and creates the
# `run_tests` gate — an always-run step that executes ctest before the
# rest of the build, so a failing test stops the build. Examples wait for
# the gate as well.
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


message (STATUS "Build with tests...")

enable_testing ()
include (catch2)
include (add-test)
add_subdirectory (tests)

# Build step: run the tests before the rest of the build; the build
# stops when any test fails.
add_custom_target (run_tests ALL
  COMMAND ${CMAKE_COMMAND} -E echo "[tests] run before the build"
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  COMMENT
    "Running tests"
  USES_TERMINAL
  VERBATIM
)
add_dependencies (run_tests version)
get_property (TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
add_dependencies (run_tests ${TEST_EXES})

# The rest of the build (examples) starts only after the tests pass.
get_property (EXAMPLE_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_EXAMPLE_EXES)
foreach (EXAMPLE_EXE IN LISTS EXAMPLE_EXES)
  add_dependencies (${EXAMPLE_EXE} run_tests)
endforeach (EXAMPLE_EXE IN LISTS EXAMPLE_EXES)

# The amalgamated header must exist before the test binaries compile the
# smoke test that includes it.
if (TARGET amalgamate)
  foreach (TEST_EXE IN LISTS TEST_EXES)
    add_dependencies (${TEST_EXE} amalgamate)
  endforeach (TEST_EXE IN LISTS TEST_EXES)
endif (TARGET amalgamate)
