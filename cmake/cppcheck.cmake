# Static analysis flags for cppcheck. The tool itself is located and
# verified by check-tools.cmake; the `run_checks` build step
# (run-checks.cmake) runs cppcheck over the sources that are about to be
# built, before anything is compiled.

set (CPPCHECK_FLAGS
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
  "-I${CMAKE_SOURCE_DIR}/include"
  "--template={file}:{line}: {severity}: {message} [{id}]"
)
