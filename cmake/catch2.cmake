# Description of the logic for connecting the Catch2 library for writing project tests

message (STATUS "Getting the \"Catch2\" library from the repository")

include(FetchContent)

FetchContent_Declare (
	catch2test
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v3.10.0
)

FetchContent_MakeAvailable (catch2test)

message (STATUS "FetchContent on: ${catch2test_SOURCE_DIR}")

## Function for adding test cases.
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
			CXX_CLANG_TIDY "${CLANG_TIDY}"
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
		EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/tests
	)

	target_include_directories (${EXECUTABLE_TEST_NAME} PRIVATE ${catch2test_SOURCE_DIR}/src)
	target_link_libraries (${EXECUTABLE_TEST_NAME} PRIVATE http_client_lite Catch2::Catch2WithMain)
	target_compile_definitions (${EXECUTABLE_TEST_NAME} PRIVATE NDEBUG)

	message (STATUS "- Append test case: ${EXECUTABLE_TEST_NAME}")
endfunction (add_test_executable)

