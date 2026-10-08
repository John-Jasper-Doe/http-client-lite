# Static analysis flags with cppcheck. The tool itself is located and
# verified by check-tools.cmake; these flags are consumed by the
# check-pipeline.sh wrapper (wired up in clang-tidy.cmake): the wrapper
# runs cppcheck before clang-tidy for every TU, both before compiling.

set (CPPCHECK_FLAGS
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
