# warnings.cmake
#
# Compiler warning flags (GCC/Clang/MSVC) for every configuration, plus
# the optional HTTP_CLIENT_LITE_WERROR switch that turns them into
# errors. Each flag below is explained on its own line.
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


option (HTTP_CLIENT_LITE_WERROR
  "Treat compiler warnings as errors (-Werror)" OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options (
    # The baseline set of "almost always useful" diagnostics: unused
    # variables/functions, uninitialized reads, format string issues,
    # missing return statements, etc.
    -Wall
    # A step beyond -Wall: unused *parameters*, sign-compare in
    # conditions, misleading indentation, deprecated declarations.
    -Wextra
    # Pedantic ISO C++ conformance (GNU extensions, anonymous structs,
    # extra semicolons, ...). The "--" prefix (not "-W..."): both gcc and
    # clang spell it --pedantic-errors — such findings stop the build
    # instead of being printed as warnings.
    --pedantic-errors
    # A local declaration hides an outer variable/parameter/class member
    # — a classic source of "wrong variable" bugs.
    -Wshadow
    # A class with virtual functions but no virtual destructor is deleted
    # through a base pointer — undefined behaviour.
    -Wnon-virtual-dtor
    # A derived class method hides a virtual method of a base class with
    # a different signature — calls through the base pointer silently
    # reach the wrong overload.
    -Woverloaded-virtual
    # A dereference of a pointer that the compiler can prove is null.
    -Wnull-dereference
    # An implicit float -> double promotion (surprises in variadic
    # functions and costs precision/performance silently).
    -Wdouble-promotion
  )

  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # gcc only: -Wimplicit-fallthrough is part of -Wextra, but the
    # switch tables in core/grammar.hpp rely on intentional fallthrough
    # — switch the diagnostic off (level 0) instead of littering the
    # tables with [[fallthrough]] attributes.
    add_compile_options (-Wimplicit-fallthrough=0)
  endif ()

  if (HTTP_CLIENT_LITE_WERROR)
    add_compile_options (-Werror)
  endif (HTTP_CLIENT_LITE_WERROR)
elseif (MSVC)
  # /W3: the recommended warning level (MSVC /Wall is unusably noisy).
  add_compile_options (/W3)
  if (HTTP_CLIENT_LITE_WERROR)
    add_compile_options (/WX)
  endif (HTTP_CLIENT_LITE_WERROR)
endif ()

if (HTTP_CLIENT_LITE_WERROR)
  message (STATUS "- Warnings as errors: ON (-Werror)")
endif (HTTP_CLIENT_LITE_WERROR)
