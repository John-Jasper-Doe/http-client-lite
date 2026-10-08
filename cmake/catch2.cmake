# Description of the logic for connecting the Catch2 library for writing project tests

message (STATUS "Getting the \"Catch2\" library from the repository")

include(FetchContent)

FetchContent_Declare (
	catch2test
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG v3.10.0
)

FetchContent_MakeAvailable (catch2test)

# Treat Catch2 headers as system headers so that static analyzers do not
# report findings in third-party code (clang-analyzer's EnumCastOutOfRange
# ignores header filters: llvm-project#179094)
foreach (_catch2_target Catch2::Catch2 Catch2::Catch2WithMain)
	if (TARGET ${_catch2_target})
		get_target_property (_catch2_real ${_catch2_target} ALIASED_TARGET)
		if (_catch2_real)
			set (_catch2_target ${_catch2_real})
		endif ()
		get_target_property (_catch2_incs ${_catch2_target} INTERFACE_INCLUDE_DIRECTORIES)
		if (_catch2_incs)
			set_property (TARGET ${_catch2_target} PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_catch2_incs}")
			set_property (TARGET ${_catch2_target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "")
		endif ()
	endif ()
endforeach ()
unset (_catch2_target)
unset (_catch2_real)
unset (_catch2_incs)

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

