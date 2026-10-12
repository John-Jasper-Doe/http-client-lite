# The `run_checks` build step: clang-format (when HTTP_CLIENT_LITE_STYLE_CHECK
# is ON), cppcheck and clang-tidy over the sources that are about to be built
# (tests and/or examples). The test/example binaries depend on this step, so
# any finding stops the build before anything is compiled.
#
# The step is stamped: it re-runs only when the checked sources or the
# .clang-tidy configuration change. Tool presence is verified at configure
# time by check-tools.cmake; clang-tidy reads the compile flags from the
# generated compile_commands.json (CMAKE_EXPORT_COMPILE_COMMANDS).

get_property (CHECK_SOURCES GLOBAL PROPERTY HTTP_CLIENT_LITE_CHECK_SOURCES)
if (NOT CHECK_SOURCES)
  return ()
endif ()

set (CHECK_STAMP ${CMAKE_BINARY_DIR}/run_checks.stamp)

# Headers are checked through the sources; they must still invalidate the
# stamp so that a header edit re-runs the checks.
file (GLOB_RECURSE CHECK_HEADERS
  ${CMAKE_SOURCE_DIR}/include/*.hpp
  ${CMAKE_SOURCE_DIR}/tests/*.hpp
  ${CMAKE_SOURCE_DIR}/examples/*.hpp
)

set (CHECK_STAGES "cppcheck, clang-tidy")
if (HTTP_CLIENT_LITE_STYLE_CHECK)
  set (CHECK_STAGES "clang-format, ${CHECK_STAGES}")
endif (HTTP_CLIENT_LITE_STYLE_CHECK)

set (CHECK_COMMANDS "")
if (HTTP_CLIENT_LITE_STYLE_CHECK)
  file (GLOB_RECURSE STYLE_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/tests/*.cpp
    ${CMAKE_SOURCE_DIR}/tests/*.hpp
    ${CMAKE_SOURCE_DIR}/examples/*.cpp
    ${CMAKE_SOURCE_DIR}/examples/*.hpp
  )
  list (APPEND CHECK_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E echo "[check] clang-format start"
    COMMAND ${CLANG_FORMAT} -style=file --dry-run -Werror ${STYLE_SOURCE_FILES}
  )
endif (HTTP_CLIENT_LITE_STYLE_CHECK)
list (APPEND CHECK_COMMANDS
  COMMAND ${CMAKE_COMMAND} -E echo "[check] cppcheck start"
  COMMAND ${CPPCHECK} ${CPPCHECK_FLAGS} ${CHECK_SOURCES}
  COMMAND ${CMAKE_COMMAND} -E echo "[check] clang-tidy start"
  COMMAND ${CLANG_TIDY} -p ${CMAKE_BINARY_DIR}
    # clang-tidy drives the clang frontend, which does not know some gcc
    # spellings in the compile commands (e.g. -Wimplicit-fallthrough=0);
    # without this the diagnostics would fail the -Werror builds.
    -extra-arg=-Wno-unknown-warning-option
    ${CHECK_SOURCES}
  COMMAND ${CMAKE_COMMAND} -E echo
    "[check] run_checks passed: ${CHECK_STAGES}"
  COMMAND ${CMAKE_COMMAND} -E touch ${CHECK_STAMP}
)

# The amalgamated header is generated: the checks over the smoke test must
# see a fresh file, so the stamp gets an explicit file-level dependency on
# it (the stamp recipe is where cppcheck/clang-tidy actually run).
set (CHECK_GENERATED_DEPS "")
if (AMALGAMATED_HEADER)
  list (APPEND CHECK_GENERATED_DEPS ${AMALGAMATED_HEADER})
endif (AMALGAMATED_HEADER)

add_custom_command (
  OUTPUT ${CHECK_STAMP}
  ${CHECK_COMMANDS}
  DEPENDS
    ${CHECK_SOURCES}
    ${CHECK_HEADERS}
    ${CMAKE_SOURCE_DIR}/.clang-tidy
    ${CHECK_GENERATED_DEPS}
  COMMENT
    "Running code checks (${CHECK_STAGES})"
  VERBATIM
)

# Notice for the builds where nothing was re-checked: the stage banners and
# the summary above are printed only when the stamp is rebuilt; otherwise a
# single "up-to-date" line reports that the checks are current.
set (CHECK_MARKER ${CMAKE_BINARY_DIR}/run_checks.marker)
add_custom_target (run_checks
  COMMAND bash -c
    "if [ '${CHECK_STAMP}' -nt '${CHECK_MARKER}' ]; then touch '${CHECK_MARKER}'; else echo '[check] run_checks: up-to-date (nothing re-checked)'; fi"
  DEPENDS ${CHECK_STAMP}
  VERBATIM
)

# The binaries build only after the checks pass.
get_property (TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
foreach (TEST_EXE IN LISTS TEST_EXES)
  add_dependencies (${TEST_EXE} run_checks)
endforeach (TEST_EXE IN LISTS TEST_EXES)

# When the tests are disabled there is no run_tests gate for the examples.
get_property (EXAMPLE_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_EXAMPLE_EXES)
if (NOT TARGET run_tests)
  foreach (EXAMPLE_EXE IN LISTS EXAMPLE_EXES)
    add_dependencies (${EXAMPLE_EXE} run_checks)
  endforeach (EXAMPLE_EXE IN LISTS EXAMPLE_EXES)
endif (NOT TARGET run_tests)

# The version banner prints before the checks.
if (TARGET version)
  add_dependencies (run_checks version)
endif (TARGET version)
