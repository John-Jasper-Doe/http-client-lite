# additional target to perform clang-format run, requires clang-format

# get all project files
file(
  GLOB_RECURSE
  ALL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/examples/*.cpp
    ${CMAKE_SOURCE_DIR}/examples/*.hpp
    ${CMAKE_SOURCE_DIR}/tests/*.cpp
    ${CMAKE_SOURCE_DIR}/tests/*.hpp
)

add_custom_target(
  clangformat
  ALL
  COMMAND
    clang-format -style=file -i ${ALL_SOURCE_FILES}  ##-style=LLVM
)
