# additional target to perform clang-format run, requires clang-format

# Adding clang-format target if executable is found
find_program (CLANG_FORMAT "clang-format")

if (CLANG_FORMAT)
  message (STATUS "Clang-format is found!")

  file (
    GLOB_RECURSE
    ALL_SOURCE_FILES
      ${CMAKE_SOURCE_DIR}/include/*.hpp
      ${CMAKE_SOURCE_DIR}/examples/*.cpp
      ${CMAKE_SOURCE_DIR}/examples/*.hpp
      ${CMAKE_SOURCE_DIR}/tests/*.cpp
      ${CMAKE_SOURCE_DIR}/tests/*.hpp
  )

  add_custom_target (
    clang_format
    ALL
    COMMAND
      clang-format -style=file -i ${ALL_SOURCE_FILES}  ##-style=LLVM
  )
endif ()
