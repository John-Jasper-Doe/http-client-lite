# Configure-time verification of the external tools required by the checks.
# All missing required tools are reported at once with an installation hint;
# configuration fails if anything is missing. The tools are executed by the
# `run_checks` build step (run-checks.cmake).

message (STATUS "Checking the required tools ...")

find_program (CPPCHECK "cppcheck")
find_program (CLANG_TIDY "clang-tidy")
find_program (CLANG_FORMAT "clang-format")

set (MISSING_TOOLS "")

if (NOT CPPCHECK)
  list (APPEND MISSING_TOOLS "cppcheck")
endif ()

if (NOT CLANG_TIDY)
  list (APPEND MISSING_TOOLS "clang-tidy")
endif ()

# clang-format is only needed when the style stage of run_checks is enabled.
if (HTTP_CLIENT_LITE_STYLE_CHECK AND NOT CLANG_FORMAT)
  list (APPEND MISSING_TOOLS "clang-format")
endif ()

if (MISSING_TOOLS)
  list (JOIN MISSING_TOOLS ", " MISSING_TOOLS_TEXT)
  message (FATAL_ERROR
    "Missing required tools: ${MISSING_TOOLS_TEXT}.\n"
    "These tools are needed by the run_checks build step.\n"
    "Install them, e.g.: sudo apt-get install cppcheck clang-tidy clang-format"
  )
endif ()

message (STATUS "- Found cppcheck:   ${CPPCHECK}")
message (STATUS "- Found clang-tidy: ${CLANG_TIDY}")
if (HTTP_CLIENT_LITE_STYLE_CHECK)
  message (STATUS "- Found clang-format: ${CLANG_FORMAT}")
endif ()
