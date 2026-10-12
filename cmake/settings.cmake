# settings.cmake
#
# Build-tree settings that are not tied to the compiler itself: where the
# binaries land and the compile database for the static analyzers.
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


# Nested tooling builds (sanitizer/coverage/profiler targets) override the
# output directories via -D; the defaults below apply only when not provided.
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
