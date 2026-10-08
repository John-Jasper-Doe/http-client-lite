# Memory analysis with valgrind memcheck: the `valgrind` build target runs
# every test binary under valgrind; any error or definite/possible leak
# fails the target. The target is created only when valgrind is installed.

if (NOT HTTP_CLIENT_LITE_BUILD_TESTS)
  return ()
endif ()

get_property (TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
if (NOT TEST_EXES)
  return ()
endif ()

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
foreach (TEST_EXE IN LISTS TEST_EXES)
  list (APPEND VALGRIND_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E echo "[valgrind] run: ${TEST_EXE}"
    COMMAND ${VALGRIND} ${VALGRIND_FLAGS} $<TARGET_FILE:${TEST_EXE}>
    COMMAND ${CMAKE_COMMAND} -E echo "[valgrind] done: ${TEST_EXE}"
  )
endforeach ()

add_custom_target (
  valgrind
  ${VALGRIND_COMMANDS}
  COMMENT
    "Running valgrind memcheck on the test binaries"
  USES_TERMINAL
  VERBATIM
)
add_dependencies (valgrind ${TEST_EXES})
