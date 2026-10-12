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

## Toolchain, project settings and version.
include (compiler)
include (warnings)
include (settings)
include (version)
include (GNUInstallDirs)

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
## directory properties; the `sanitizer`/`coverage`/`profiler` targets are
## deferred until all directories are processed)
include (sanitizer)
include (coverage)

## The library itself (headers, interface target, optional amalgamation).
include (library)
include (amalgamation)

## Examples
if (HTTP_CLIENT_LITE_BUILD_EXAMPLES)
	message (STATUS "Build with examples...")
	include (add-example)
	add_subdirectory (examples)
endif (HTTP_CLIENT_LITE_BUILD_EXAMPLES)

## Tests
if (HTTP_CLIENT_LITE_BUILD_TESTS)
	include (tests)
endif (HTTP_CLIENT_LITE_BUILD_TESTS)

## Code checks (the `run_checks` step gates the test/example binaries)
include (run-checks)

## Runtime analysis targets (valgrind memcheck, callgrind profiling)
include (valgrind)
include (profiler)

## Installation rules and the CMake package config (find_package support)
include (install)

## Packaging (CPack: DEB and, when rpmbuild is available, RPM)
include (packaging)
