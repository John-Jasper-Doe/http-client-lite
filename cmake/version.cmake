# version.cmake
#
# Project version from git
# --------------------------
#
# Computes Major.Minor.Patch from git (cmake/version-script.cmake) and prints
# the banner at configure time and then on every build, so the build log
# always carries the version and the build date:
#
#   [version] 0.1.26+dev.g99b3881 (built 2026-10-08T09:15:30Z)

message (STATUS "Configuring the project version ...")

set (VERSION_SCRIPT ${CMAKE_CURRENT_LIST_DIR}/version-script.cmake)

find_package (Git QUIET)
set (VERSION_GIT_ARGS "")
if (GIT_FOUND)
  list (APPEND VERSION_GIT_ARGS "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}")
endif (GIT_FOUND)

# Configure-time banner: the child process inherits our streams.
execute_process (
  COMMAND ${CMAKE_COMMAND}
    "-DVERSION_SOURCE_DIR=${CMAKE_SOURCE_DIR}"
    "-DVERSION_FILE=${CMAKE_BINARY_DIR}/version.txt"
    ${VERSION_GIT_ARGS}
    -P ${VERSION_SCRIPT}
)

# The version computed by the script above, shared with the install/packaging
# modules and refreshed automatically whenever it changes during a build
# (version.txt is rewritten only on an actual version change).
if (EXISTS "${CMAKE_BINARY_DIR}/version.txt")
  file (READ "${CMAKE_BINARY_DIR}/version.txt" HTTP_CLIENT_LITE_VERSION)
  string (STRIP "${HTTP_CLIENT_LITE_VERSION}" HTTP_CLIENT_LITE_VERSION)
else ()
  set (HTTP_CLIENT_LITE_VERSION "0.0.0")
endif (EXISTS "${CMAKE_BINARY_DIR}/version.txt")
set_property (DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
  "${CMAKE_BINARY_DIR}/version.txt"
)

# Build-time banner: `version` is an always-run target, so every build
# recomputes the version and the date. The bare version string goes to
# <build>/version.txt and is rewritten only when it changes, keeping the file
# a stable input for anything that may depend on it later.
add_custom_target (version ALL
  COMMAND ${CMAKE_COMMAND}
    "-DVERSION_SOURCE_DIR=${CMAKE_SOURCE_DIR}"
    "-DVERSION_FILE=${CMAKE_BINARY_DIR}/version.txt"
    ${VERSION_GIT_ARGS}
    -P ${VERSION_SCRIPT}
  COMMENT
    "Project version"
  VERBATIM
)
