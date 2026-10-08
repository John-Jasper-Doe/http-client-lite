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

## Options...
option (HTTP_CLIENT_LITE_BUILD_TESTS "Build tests" OFF)
option (HTTP_CLIENT_LITE_BUILD_EXAMPLES "Build examples" OFF)
option (STYLE_CHECK "Check code style before every build" ON)
option (HTTP_CLIENT_LITE_ENABLE_CHECKS
  "Static analysis and style checks before every build" ON)

## Checks...
# The option exists for nested tooling builds (sanitizer/coverage targets);
# keep it ON for regular builds.
if (HTTP_CLIENT_LITE_ENABLE_CHECKS)
  include (check-tools)
  include (cppcheck)
  include (clang-tidy)
  include (style-check)
endif (HTTP_CLIENT_LITE_ENABLE_CHECKS)

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
target_sources (
	${PROJECT_NAME}
	INTERFACE
		${${PROJECT_NAME}_HEADERS}
)
target_include_directories (
    ${PROJECT_NAME}
    INTERFACE
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
      "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)


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
endif (HTTP_CLIENT_LITE_BUILD_TESTS)

## Runtime analysis (separate build target: valgrind; sanitizer/coverage
## targets are created by the deferred functions from the modules above)
include (valgrind)

