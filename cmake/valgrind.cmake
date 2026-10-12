# valgrind.cmake
#
# Memory analysis with valgrind memcheck: the `valgrind` build target runs
# every test and example binary under valgrind; any error or
# definite/possible leak fails the target. The target is created only when
# valgrind is installed and at least one binary exists (tests and/or
# examples).
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


get_property (TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
get_property (EXAMPLE_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_EXAMPLE_EXES)
set (VALGRIND_EXES ${TEST_EXES} ${EXAMPLE_EXES})
if (NOT VALGRIND_EXES)
  return ()
endif (NOT VALGRIND_EXES)

find_program (VALGRIND "valgrind")
if (NOT VALGRIND)
  message (STATUS
    "- valgrind not found: the 'valgrind' target is disabled"
    " (sudo apt-get install valgrind)")
  return ()
endif ()
message (STATUS "- Found valgrind: ${VALGRIND}")

set (VALGRIND_FLAGS
  "--leak-check=full"
  "--show-leak-kinds=definite,possible"
  "--errors-for-leak-kinds=definite,possible"
  "--error-exitcode=1"
)

set (VALGRIND_COMMANDS "")
foreach (VALGRIND_EXE IN LISTS VALGRIND_EXES)
  list (APPEND VALGRIND_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E echo "[valgrind] run: ${VALGRIND_EXE}"
    COMMAND ${VALGRIND} ${VALGRIND_FLAGS} $<TARGET_FILE:${VALGRIND_EXE}>
    COMMAND ${CMAKE_COMMAND} -E echo "[valgrind] done: ${VALGRIND_EXE}"
  )
endforeach (VALGRIND_EXE IN LISTS VALGRIND_EXES)

add_custom_target (
  valgrind
  ${VALGRIND_COMMANDS}
  COMMENT
    "Running valgrind memcheck on the test/example binaries"
  USES_TERMINAL
  VERBATIM
)
add_dependencies (valgrind ${VALGRIND_EXES})
