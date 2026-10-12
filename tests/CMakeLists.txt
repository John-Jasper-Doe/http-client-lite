message (STATUS "Configure test cases...")


add_subdirectory (common)
add_subdirectory (core)

if (HTTP_CLIENT_LITE_AMALGAMATE)
	add_subdirectory (amalgamation)
endif (HTTP_CLIENT_LITE_AMALGAMATE)
