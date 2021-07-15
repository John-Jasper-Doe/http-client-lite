## compiler.cmake
##
## Basic compiler settings
## --------------------------
##
## Copyright 2021... by Maxim Gusev
##
## https://github.com/John-Jasper-Doe/http-client-lite
##
## Distributed under the MIT License.
## (See accompanying file LICENSE or copy at https://mit-license.org/)


message(STATUS "Configuring the compiler parameters ...")
message(STATUS "- Detected compiler id: ${CMAKE_CXX_COMPILER_ID} ver.: ${CMAKE_CXX_COMPILER_VERSION}")

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
message(STATUS "- Detected standard: ${CMAKE_CXX_STANDARD}")

message(STATUS "- Build type: ${CMAKE_BUILD_TYPE}")

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-pthread -Wall -Wextra -Wpedantic -Wimplicit-fallthrough=0 -g -O0)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-pthread -Wall -Wextra -Wpedantic -Wimplicit-fallthrough=0 -O2)
  endif()
elseif(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -Wpedantic -g -O0)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  endif()
elseif(MSVC OR WIN32)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/Wall /WX /W3 /Zi /Od)
  elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(/Wall /WX /W3 /O2)
  endif()
else()
  message(FATAL_ERROR "You are using an unsupported compiler!")
endif()

