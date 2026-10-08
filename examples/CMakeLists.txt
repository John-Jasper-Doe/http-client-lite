# Function for adding examples.
function (add_examples TARGET_NAME FILE_NAME)
	set (EXAMPLE_NAME "example_${TARGET_NAME}")
	add_executable (${EXAMPLE_NAME} ${FILE_NAME})
	target_link_libraries (${EXAMPLE_NAME} PRIVATE http_client_lite)
	set_property (GLOBAL APPEND PROPERTY HTTP_CLIENT_LITE_EXAMPLE_EXES ${EXAMPLE_NAME})

	set (CHECK_SOURCE ${FILE_NAME})
	cmake_path (ABSOLUTE_PATH CHECK_SOURCE BASE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	set_property (GLOBAL APPEND PROPERTY HTTP_CLIENT_LITE_CHECK_SOURCES ${CHECK_SOURCE})
	message (STATUS "- Append examples: ${EXAMPLE_NAME}")
endfunction (add_examples)


message (STATUS "Configure examples...")

add_examples (uri_parse uri_parse.cpp)
