## CMake file for TestCases
## ---------------------------
##
## Copyright 2021... by Maxim Gusev
##
## https://github.com/John-Jasper-Doe/http-client-lite
##
## Distributed under the MIT License.
## (See accompanying file LICENSE or copy at https://mit-license.org/)


cmake_minimum_required(VERSION 3.3)
project(tests LANGUAGES CXX)

message(STATUS "Configure test cases...")

## Function for adding test cases.
function(addtest TEST_NAME MAIN_FILE)
  set(EXEC_TEST_NAME "${PROJECT_NAME}_${TEST_NAME}")
  add_executable(${EXEC_TEST_NAME} ${MAIN_FILE} testhelper.hpp)

  add_test(
    NAME ${EXEC_TEST_NAME}
    COMMAND ${EXEC_TEST_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
  )

  set_target_properties(${EXEC_TEST_NAME}
    PROPERTIES
      CXX_STANDARD 11
      CXX_STANDARD_REQUIRED ON
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
      EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/tests
  )

  target_link_libraries(${EXEC_TEST_NAME} PRIVATE http_client_lite)

  message(STATUS "- Append test case: ${EXEC_TEST_NAME}")
endfunction()


addtest(tokenizer tokenizer.cpp)
