# compiler.cmake
#
# Compiler settings: standard, extensions, build type, per-config
# optimization. Warning flags live in cmake/warnings.cmake.
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


message (STATUS "Configuring the compiler parameters ...")
message (STATUS
  "- Detected compiler id: ${CMAKE_CXX_COMPILER_ID} ver.: ${CMAKE_CXX_COMPILER_VERSION}"
)

## C++17 in its strict ISO flavour (no compiler extensions like gnu++17).
if (NOT CMAKE_CXX_STANDARD)
  set (CMAKE_CXX_STANDARD 17)
endif ()
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
message (STATUS "- Detected standard: c++${CMAKE_CXX_STANDARD} (ISO, no extensions)")

## A default build type for the single-config generators: nothing was
## chosen explicitly — build a debug variant (asserts, no optimization).
if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif ()
message (STATUS "- Build type: ${CMAKE_BUILD_TYPE}")

## Per-configuration optimization and NDEBUG come from the stock CMake
## flag sets — no manual duplication needed:
##   Debug:         "-g"            (asserts alive, no optimization)
##   Release:       "-O3 -DNDEBUG"
##   RelWithDebInfo: "-O2 -g -DNDEBUG"
##   MinSizeRel:    "-Os -DNDEBUG"
## The asserts stay alive in Debug exactly where they are useful.

## Threading support: -pthread enables the reentrant libc/libstdc++ and
## links libpthread (harmless for the header-only library itself, required
## by the C++ standard library of the test/example binaries).
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options (-pthread)
  add_link_options (-pthread)
elseif (MSVC)
  add_definitions (-D_CRT_SECURE_NO_WARNINGS)
else ()
  message (FATAL_ERROR "You are using an unsupported compiler!")
endif ()
