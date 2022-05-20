# additional target to perform clang-format run, requires clang-format

# get all project files
file(
  GLOB_RECURSE
  ALL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/include/*.hpp
    ${CMAKE_SOURCE_DIR}/examples/*.cpp
    ${CMAKE_SOURCE_DIR}/examples/*.hpp
    ${CMAKE_SOURCE_DIR}/test/*.cpp
    ${CMAKE_SOURCE_DIR}/test/*.hpp
)

add_custom_target(
        clangformat
        COMMAND clang-format
        -style=file    ##-style=LLVM
        -i
        ${ALL_SOURCE_FILES}
)
