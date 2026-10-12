# run-examples.cmake — script mode (cmake -P): runs every example_* binary
# found in a directory and fails on the first non-zero exit code. Used by
# the nested sanitizer/coverage builds, where the example binaries live in
# the nested build tree and are not registered with ctest.
#
# Usage: cmake -DEXAMPLES_DIR=<dir> -P run-examples.cmake

if (NOT DEFINED EXAMPLES_DIR)
  message (FATAL_ERROR "run-examples.cmake: EXAMPLES_DIR is required")
endif (NOT DEFINED EXAMPLES_DIR)

file (GLOB EXAMPLE_BINARIES "${EXAMPLES_DIR}/example_*")
if (NOT EXAMPLE_BINARIES)
  message (FATAL_ERROR
    "run-examples.cmake: no example binaries in ${EXAMPLES_DIR}"
  )
endif (NOT EXAMPLE_BINARIES)
list (SORT EXAMPLE_BINARIES)

foreach (EXAMPLE_BINARY IN LISTS EXAMPLE_BINARIES)
  message (STATUS "[examples] run: ${EXAMPLE_BINARY}")
  execute_process (
    COMMAND "${EXAMPLE_BINARY}"
    RESULT_VARIABLE EXAMPLE_RESULT
  )
  if (EXAMPLE_RESULT)
    message (FATAL_ERROR
      "run-examples.cmake: ${EXAMPLE_BINARY} failed"
      " (exit code ${EXAMPLE_RESULT})"
    )
  endif (EXAMPLE_RESULT)
endforeach (EXAMPLE_BINARY IN LISTS EXAMPLE_BINARIES)

list (LENGTH EXAMPLE_BINARIES EXAMPLE_COUNT)
message (STATUS "[examples] ${EXAMPLE_COUNT} example(s) passed")
