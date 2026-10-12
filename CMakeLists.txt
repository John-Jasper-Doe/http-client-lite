## Root CMake file for Http Client Lite
## ---------------------------------------
##
## Copyright 2021... by Maxim Gusev
##
## https://github.com/John-Jasper-Doe/http-client-lite
##
## Distributed under the MIT License.
## (See accompanying file LICENSE or copy at https://mit-license.org/)


cmake_minimum_required (VERSION 3.20)
project (http_client_lite LANGUAGES CXX)


## Adding a path with "cmake" modules to the system search paths.
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (compiler)
include (version)
include (GNUInstallDirs)

## Settings...
# Nested tooling builds (sanitizer/coverage targets) override the output
# directories via -D; the defaults below apply only when not provided.
if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
if (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif (NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
if (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif (NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)

# Used by the run_checks step (clang-tidy looks the flags up per source).
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

## Options...
option (HTTP_CLIENT_LITE_BUILD_TESTS "Build tests" ON)
option (HTTP_CLIENT_LITE_BUILD_EXAMPLES "Build examples" OFF)
option (HTTP_CLIENT_LITE_STYLE_CHECK
  "Check code style (clang-format) as part of the run_checks step" ON)
option (HTTP_CLIENT_LITE_AMALGAMATE
  "Group the library into a single header file" OFF)

## Checks...
include (check-tools)
include (cppcheck)

## Runtime instrumentation (before the targets are created: the flags are
## directory properties; the `sanitizer`/`coverage` targets are deferred
## until all directories are processed)
include (sanitizer)
include (coverage)


## Create file lists of project ...
get_filename_component (CURRENT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR} NAME)

file (GLOB_RECURSE ${CURRENT_DIRECTORY}_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/*.h*)

list (APPEND ${PROJECT_NAME}_HEADERS ${${CURRENT_DIRECTORY}_HEADERS})

# Interface library
add_library (${PROJECT_NAME} INTERFACE)
# The headers are listed as INTERFACE sources for IDE visibility only: the
# BUILD_INTERFACE wrapper keeps them out of the installed export file, which
# rejects source-directory paths.
foreach (${PROJECT_NAME}_HEADER IN LISTS ${PROJECT_NAME}_HEADERS)
	target_sources (
		${PROJECT_NAME}
		INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_HEADER}>
	)
endforeach (${PROJECT_NAME}_HEADER IN LISTS ${PROJECT_NAME}_HEADERS)
target_include_directories (
    ${PROJECT_NAME}
    INTERFACE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

## Single-header amalgamation (optional): the generated header is offered to
## the consumers of the target as <httpclientlite.hpp> and is regenerated
## only when one of the source headers or the script itself changes.
if (HTTP_CLIENT_LITE_AMALGAMATE)
	set (AMALGAMATED_HEADER ${CMAKE_BINARY_DIR}/generated/httpclientlite.hpp)
	add_custom_command (
		OUTPUT ${AMALGAMATED_HEADER}
		COMMAND ${CMAKE_COMMAND}
			"-DHEADER_DIR=${CMAKE_CURRENT_SOURCE_DIR}/include"
			"-DOUTPUT_FILE=${AMALGAMATED_HEADER}"
			-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/amalgamate.cmake
		DEPENDS ${${PROJECT_NAME}_HEADERS}
			${CMAKE_CURRENT_SOURCE_DIR}/cmake/amalgamate.cmake
		COMMENT
			"Amalgamating the headers into httpclientlite.hpp"
		VERBATIM
	)
	add_custom_target (amalgamate ALL DEPENDS ${AMALGAMATED_HEADER})
	target_include_directories (${PROJECT_NAME} INTERFACE
		"$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/generated>"
	)
	message (STATUS "Build with the amalgamated single header...")
endif (HTTP_CLIENT_LITE_AMALGAMATE)


## Examples
if (HTTP_CLIENT_LITE_BUILD_EXAMPLES)
	message (STATUS "Build with examples...")
	add_subdirectory (examples)
endif (HTTP_CLIENT_LITE_BUILD_EXAMPLES)

## Tests
if (HTTP_CLIENT_LITE_BUILD_TESTS)
	message (STATUS "Build with tests...")
	enable_testing ()
	include (catch2)
	add_subdirectory (tests)

	# Build step: run the tests before the rest of the build; the build
	# stops when any test fails.
	add_custom_target (run_tests ALL
		COMMAND ${CMAKE_COMMAND} -E echo "[tests] run before the build"
		COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
		COMMENT
			"Running tests"
		USES_TERMINAL
		VERBATIM
	)
	add_dependencies (run_tests version)
	get_property (TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
	add_dependencies (run_tests ${TEST_EXES})

	# The rest of the build (examples) starts only after the tests pass.
	get_property (EXAMPLE_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_EXAMPLE_EXES)
	foreach (EXAMPLE_EXE IN LISTS EXAMPLE_EXES)
		add_dependencies (${EXAMPLE_EXE} run_tests)
	endforeach (EXAMPLE_EXE IN LISTS EXAMPLE_EXES)
endif (HTTP_CLIENT_LITE_BUILD_TESTS)

## Code checks (the `run_checks` step gates the test/example binaries)
include (run-checks)
if (TARGET run_checks)
	add_dependencies (run_checks version)
endif (TARGET run_checks)

# The amalgamated header must exist before the test binaries compile the
# smoke test that includes it.
if (TARGET amalgamate)
	get_property (AMALGAM_TEST_EXES GLOBAL PROPERTY HTTP_CLIENT_LITE_TEST_EXES)
	foreach (TEST_EXE IN LISTS AMALGAM_TEST_EXES)
		add_dependencies (${TEST_EXE} amalgamate)
	endforeach (TEST_EXE IN LISTS AMALGAM_TEST_EXES)
endif (TARGET amalgamate)

## Runtime analysis (separate build targets: valgrind; sanitizer/coverage
## targets are created by the deferred functions from the modules above)
include (valgrind)

## Installation rules and the CMake package config (find_package support)
include (install)

## Packaging (CPack: DEB and, when rpmbuild is available, RPM)
include (packaging)

