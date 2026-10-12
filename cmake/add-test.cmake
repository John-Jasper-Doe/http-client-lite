# add-test.cmake
#
# Function for adding a test executable (Catch2) to the build: the binary
# runs before the rest of the build (run_tests gate), reports JUnit XML
# for the CI and is registered for the code checks.
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


function (add_test_executable TARGET_NAME FILE_NAME)
  set (EXECUTABLE_TEST_NAME "test_${TARGET_NAME}")
  add_executable (${EXECUTABLE_TEST_NAME} ${FILE_NAME})

  add_test (
    NAME ${EXECUTABLE_TEST_NAME}
    COMMAND ${EXECUTABLE_TEST_NAME} -o ${EXECUTABLE_TEST_NAME}_report.xml -r junit
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests
  )

  set_target_properties (
    ${EXECUTABLE_TEST_NAME}
    PROPERTIES
      CXX_STANDARD ${CMAKE_CXX_STANDARD}
      CXX_STANDARD_REQUIRED ${CMAKE_CXX_STANDARD_REQUIRED}
      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
      EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/tests
  )

  set_property (GLOBAL APPEND PROPERTY HTTP_CLIENT_LITE_TEST_EXES ${EXECUTABLE_TEST_NAME})

  set (CHECK_SOURCE ${FILE_NAME})
  cmake_path (ABSOLUTE_PATH CHECK_SOURCE BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  set_property (GLOBAL APPEND PROPERTY HTTP_CLIENT_LITE_CHECK_SOURCES ${CHECK_SOURCE})

  target_include_directories (${EXECUTABLE_TEST_NAME} SYSTEM PRIVATE ${catch2test_SOURCE_DIR}/src)
  target_link_libraries (${EXECUTABLE_TEST_NAME} PRIVATE http_client_lite Catch2::Catch2WithMain)
  target_compile_definitions (${EXECUTABLE_TEST_NAME} PRIVATE NDEBUG)

  message (STATUS "- Append test case: ${EXECUTABLE_TEST_NAME}")
endfunction (add_test_executable)
