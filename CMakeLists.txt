## Root CMake file for Http Client Lite
## ---------------------------------------
##
## Copyright 2021... by Maxim Gusev
##
## https://github.com/John-Jasper-Doe/http-client-lite
##
## Distributed under the MIT License.
## (See accompanying file LICENSE or copy at https://mit-license.org/)


cmake_minimum_required(VERSION 3.3)
project(http_client_lite LANGUAGES CXX)

## Adding a path with "cmake" modules to the system search paths.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(compiler)
include(clang-format)

## Set output binary
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

## Options...
option(HTTP_CLIENT_LITE_OPT_BUILD_TESTS    "Build tests" OFF)
option(HTTP_CLIENT_LITE_OPT_BUILD_EXAMPLES "Build examples" OFF )

## Examples
if(HTTP_CLIENT_LITE_OPT_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

## Tests
if(HTTP_CLIENT_LITE_OPT_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# Interface library
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/jdl/httpclientlite.hpp)
add_custom_target(${PROJECT_NAME}.hdr SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/include/jdl/httpclientlite.hpp)
target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
