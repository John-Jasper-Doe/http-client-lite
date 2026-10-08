# Static analysis with cppcheck, runs before compiling every TU.
# The check cannot be disabled: configuration fails if cppcheck is missing.

# Adding cppcheck integration if executable is found
find_program (CPPCHECK "cppcheck")

if (NOT CPPCHECK)
  message (FATAL_ERROR
    "cppcheck is required but was not found! Install it (e.g. sudo apt-get install cppcheck)"
  )
endif ()

message (STATUS "Cppcheck is found: ${CPPCHECK}")

set (CMAKE_CXX_CPPCHECK
  ${CPPCHECK}
  "--enable=warning,style,performance,portability,information,missingInclude"
  "--std=c++${CMAKE_CXX_STANDARD}"
  "--check-level=exhaustive"
  "--inconclusive"
  "--force"
  "--error-exitcode=1"
  "--inline-suppr"
  "--suppress=missingIncludeSystem"
  "--suppress=checkersReport"
  "--suppress=unmatchedSuppression"
  "--suppress=unreadVariable:*tests*"
  "--suppress=comparisonOfFuncReturningBoolError:*tests*"
  "--suppress=*:*build/_deps/*"
  "-I${CMAKE_SOURCE_DIR}/include"
  "--template={file}:{line}: {severity}: {message} [{id}]"
)
