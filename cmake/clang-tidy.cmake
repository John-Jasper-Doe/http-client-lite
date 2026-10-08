# Static analysis with clang-tidy: for every TU cppcheck runs first, then
# clang-tidy, both before compiling. CMake's native co-compile slot order
# is tidy -> cppcheck (the reverse of the required order), so both tools
# are hosted in one wrapper (check-pipeline.sh) in the CMAKE_CXX_CLANG_TIDY
# slot and CMAKE_CXX_CPPCHECK stays unset. The check cannot be disabled:
# clang-tidy is located and verified by check-tools.cmake. The checks
# themselves are configured by the .clang-tidy file at the repository root.

set (CMAKE_CXX_CLANG_TIDY
  "${CMAKE_SOURCE_DIR}/cmake/check-pipeline.sh"
  "--cpp"
  ${CPPCHECK_FLAGS}
  "--run-tidy"
  ${CLANG_TIDY}
)
