# Function for adding examples.
function (add_examples TARGET_NAME FILE_NAME)
	set (EXAMPLE_NAME "example_${TARGET_NAME}")
	add_executable (${EXAMPLE_NAME} ${FILE_NAME})
	target_link_libraries (${EXAMPLE_NAME} PRIVATE http_client_lite)
	message (STATUS "- Append examples: ${EXAMPLE_NAME}")
endfunction (add_examples)


message (STATUS "Configure examples...")

add_examples (uri_parse uri_parse.cpp)
