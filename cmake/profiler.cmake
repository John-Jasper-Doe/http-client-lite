# profiler.cmake
#
# CPU profiling with callgrind: the `profiler` build target performs a
# dedicated nested RelWithDebInfo build (frame pointers, debug symbols),
# runs every test and example binary under `valgrind --tool=callgrind` and
# writes a text report per binary with callgrind_annotate:
#
#   <build>/profile/callgrind.<binary>.out  — the raw call graph
#   <build>/profile/report.<binary>.txt     — the annotated report
#
# NOTE: this module must be included after the test/example targets are
# created (it reads the global binary lists); the `profiler` target itself
# is created after all directories are processed (DEFER).
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


## Create the `profiler` target: nested build + callgrind run + reports.
function (http_client_lite_profiler_target)
  get_property (TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
  get_property (EXAMPLE_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_EXAMPLE_EXES)
  if (NOT TEST_EXES AND NOT EXAMPLE_EXES)
    return ()
  endif ()

  find_program (VALGRIND "valgrind")
  find_program (CALLGRIND_ANNOTATE "callgrind_annotate")
  if (NOT VALGRIND OR NOT CALLGRIND_ANNOTATE)
    message (STATUS
      "- valgrind/callgrind_annotate not found: the 'profiler' target is"
      " disabled (sudo apt-get install valgrind)")
    return ()
  endif ()
  message (STATUS "- Found callgrind: ${VALGRIND}, ${CALLGRIND_ANNOTATE}")

  set (PROFILE_DIR ${CMAKE_BINARY_DIR}/profile)
  set (PROFILE_BUILD_DIR ${PROFILE_DIR}/build)

  set (PROFILE_CONFIGURE_ARGS
    "-S" "${CMAKE_SOURCE_DIR}"
    "-B" "${PROFILE_BUILD_DIR}"
    "-G" "${CMAKE_GENERATOR}"
    "-DCMAKE_BUILD_TYPE=RelWithDebInfo"
    # Frame pointers give precise call graphs; the debug symbols put the
    # line numbers into the callgrind_annotate report.
    "-DCMAKE_CXX_FLAGS=-fno-omit-frame-pointer"
    "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${PROFILE_BUILD_DIR}/bin"
    "-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${PROFILE_BUILD_DIR}/bin"
    "-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY=${PROFILE_BUILD_DIR}/bin"
    "-DHTTP_CLIENT_LITE_BUILD_TESTS=${HTTP_CLIENT_LITE_BUILD_TESTS}"
    "-DHTTP_CLIENT_LITE_BUILD_EXAMPLES=${HTTP_CLIENT_LITE_BUILD_EXAMPLES}"
    "-DHTTP_CLIENT_LITE_AMALGAMATE=${HTTP_CLIENT_LITE_AMALGAMATE}"
    "-DHTTP_CLIENT_LITE_STYLE_CHECK=OFF"
  )

  # Reuse the already fetched Catch2 sources in the nested build.
  if (DEFINED catch2test_SOURCE_DIR)
    list (APPEND PROFILE_CONFIGURE_ARGS
      "-DFETCHCONTENT_SOURCE_DIR_CATCH2TEST=${catch2test_SOURCE_DIR}")
  endif (DEFINED catch2test_SOURCE_DIR)

  set (CALLGRIND_FLAGS
    "--tool=callgrind"
    # Inclusive/exclusive jump edges in the call graph.
    "--collect-jumps=yes"
  )

  set (PROFILE_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E echo
      "[profiler] configure nested RelWithDebInfo build: ${PROFILE_BUILD_DIR}"
    COMMAND
      ${CMAKE_COMMAND} ${PROFILE_CONFIGURE_ARGS}
    COMMAND
      ${CMAKE_COMMAND} -E echo "[profiler] build"
    COMMAND
      ${CMAKE_COMMAND} --build "${PROFILE_BUILD_DIR}" --parallel
    COMMAND
      ${CMAKE_COMMAND} -E make_directory "${PROFILE_DIR}"
  )

  # Run every binary under callgrind and annotate its profile. The tests
  # live in <build>/tests, the examples in <build>/bin (output directories
  # are overridden above).
  foreach (PROFILE_EXE IN LISTS TEST_EXES)
    list (APPEND PROFILE_COMMANDS
      COMMAND ${CMAKE_COMMAND} -E echo "[profiler] profile: ${PROFILE_EXE}"
      COMMAND ${VALGRIND} ${CALLGRIND_FLAGS}
        "--callgrind-out-file=${PROFILE_DIR}/callgrind.${PROFILE_EXE}.out"
        "${PROFILE_BUILD_DIR}/tests/${PROFILE_EXE}"
      COMMAND bash -c
        "'${CALLGRIND_ANNOTATE}' '${PROFILE_DIR}/callgrind.${PROFILE_EXE}.out' > '${PROFILE_DIR}/report.${PROFILE_EXE}.txt' 2>&1"
    )
  endforeach (PROFILE_EXE IN LISTS TEST_EXES)

  foreach (PROFILE_EXE IN LISTS EXAMPLE_EXES)
    list (APPEND PROFILE_COMMANDS
      COMMAND ${CMAKE_COMMAND} -E echo "[profiler] profile: ${PROFILE_EXE}"
      COMMAND ${VALGRIND} ${CALLGRIND_FLAGS}
        "--callgrind-out-file=${PROFILE_DIR}/callgrind.${PROFILE_EXE}.out"
        "${PROFILE_BUILD_DIR}/bin/${PROFILE_EXE}"
      COMMAND bash -c
        "'${CALLGRIND_ANNOTATE}' '${PROFILE_DIR}/callgrind.${PROFILE_EXE}.out' > '${PROFILE_DIR}/report.${PROFILE_EXE}.txt' 2>&1"
    )
  endforeach (PROFILE_EXE IN LISTS EXAMPLE_EXES)

  list (APPEND PROFILE_COMMANDS
    COMMAND ${CMAKE_COMMAND} -E echo
      "[profiler] reports in ${PROFILE_DIR}/report.*.txt"
  )

  add_custom_target (
    profiler
    ${PROFILE_COMMANDS}
    COMMENT
      "Nested profiled build, callgrind run and annotated reports"
    USES_TERMINAL
    VERBATIM
  )
endfunction (http_client_lite_profiler_target)

if (HTTP_CLIENT_LITE_BUILD_TESTS OR HTTP_CLIENT_LITE_BUILD_EXAMPLES)
  cmake_language (DEFER DIRECTORY "${CMAKE_SOURCE_DIR}"
    CALL http_client_lite_profiler_target)
endif (HTTP_CLIENT_LITE_BUILD_TESTS OR HTTP_CLIENT_LITE_BUILD_EXAMPLES)
