# catch2.cmake
#
# Fetches the Catch2 framework for the tests (included only when
# HTTP_CLIENT_LITE_BUILD_TESTS is ON, see cmake/tests.cmake). The network
# is touched on the first configure and then only when GIT_TAG changes:
# FETCHCONTENT_UPDATES_DISCONNECTED skips the update step of the already
# populated content on the ordinary re-configures.
# --------------------------
##
# Copyright 2021... by Maxim Gusev
#
# https://github.com/John-Jasper-Doe/http-client-lite
#
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at https://mit-license.org/)


message (STATUS "Getting the \"Catch2\" library from the repository")

include (FetchContent)

# No `git fetch` on re-configure; the checkout is refreshed only when the
# declared GIT_TAG no longer matches the populated content.
set (FETCHCONTENT_UPDATES_DISCONNECTED ON)

FetchContent_Declare (
  catch2test
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.10.0
  # A single commit of the tagged release instead of the whole history.
  GIT_SHALLOW ON
)

# Fetch the framework without the project warning flags: third-party code
# is not ours to keep warning-clean (the flags are restored right after).
get_directory_property (_catch2_saved_compile_options COMPILE_OPTIONS)
set_directory_properties (PROPERTIES COMPILE_OPTIONS "")
FetchContent_MakeAvailable (catch2test)
set_directory_properties (
  PROPERTIES COMPILE_OPTIONS "${_catch2_saved_compile_options}"
)
unset (_catch2_saved_compile_options)

# Treat Catch2 headers as system headers so that static analyzers do not
# report findings in third-party code (clang-analyzer's EnumCastOutOfRange
# ignores header filters: llvm-project#179094)
foreach (_catch2_target Catch2::Catch2 Catch2::Catch2WithMain)
  if (TARGET ${_catch2_target})
    get_target_property (_catch2_real ${_catch2_target} ALIASED_TARGET)
    if (_catch2_real)
      set (_catch2_target ${_catch2_real})
    endif ()
    get_target_property (_catch2_incs ${_catch2_target} INTERFACE_INCLUDE_DIRECTORIES)
    if (_catch2_incs)
      set_property (TARGET ${_catch2_target} PROPERTY INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "${_catch2_incs}")
      set_property (TARGET ${_catch2_target} PROPERTY INTERFACE_INCLUDE_DIRECTORIES "")
    endif ()
  endif ()
endforeach ()
unset (_catch2_target)
unset (_catch2_real)
unset (_catch2_incs)

message (STATUS "FetchContent on: ${catch2test_SOURCE_DIR}")
