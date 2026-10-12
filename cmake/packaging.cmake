# packaging.cmake
#
# CPack packaging (DEB and RPM)
# -------------------------------
#
# Produces a header-only package:
#   DEB  libhttp-client-lite-dev_<version>_all.deb
#   RPM  http-client-lite-devel-<version>-<release>.noarch.rpm (when rpmbuild
#        is installed; the generator and the `rpm` target are enabled then
#        automatically at configure time)
#
# The version comes from git via cmake/version.cmake (version.txt). Every
# cpack run — the stock `package` target, `cpack -G ...` or the `deb`/`rpm`
# targets below — re-reads version.txt at package time through
# CPACK_PROJECT_CONFIG_FILE (cmake/cpack-project-config.cmake.in), so
# `commit && make deb` packages the actual HEAD.

message (STATUS "Configuring packaging ...")

# Debian versions must start with a digit: degrade an unavailable git version.
set (PACKAGE_VERSION "${HTTP_CLIENT_LITE_VERSION}")
if (NOT PACKAGE_VERSION MATCHES "^[0-9]")
  set (PACKAGE_VERSION "0.0.0+unknown")
endif (NOT PACKAGE_VERSION MATCHES "^[0-9]")
message (STATUS "- Package version: ${PACKAGE_VERSION}")

set (CPACK_PACKAGE_NAME "http-client-lite")
set (CPACK_PACKAGE_VENDOR "John-Jasper-Doe")
set (CPACK_PACKAGE_CONTACT "john <jasper@jdoe.ru>")
set (CPACK_PACKAGE_HOMEPAGE_URL
  "https://github.com/John-Jasper-Doe/http-client-lite"
)
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY
  "Lite C++ cross-platform header-only HTTP 1.1 client library"
)
set (CPACK_PACKAGE_VERSION "${PACKAGE_VERSION}")
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")

## Debian...
set (CPACK_DEBIAN_PACKAGE_NAME "libhttp-client-lite-dev")
set (CPACK_DEBIAN_PACKAGE_MAINTAINER "${CPACK_PACKAGE_CONTACT}")
set (CPACK_DEBIAN_PACKAGE_SECTION "libdevel")
# Header-only: an architecture-independent package.
set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "all")
set (CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
message (STATUS "- DEB: ${CPACK_DEBIAN_PACKAGE_NAME} (Architecture: all)")

## RPM...
set (CPACK_RPM_PACKAGE_NAME "http-client-lite-devel")
set (CPACK_RPM_PACKAGE_LICENSE "MIT")
set (CPACK_RPM_PACKAGE_ARCHITECTURE "noarch")
set (CPACK_RPM_PACKAGE_AUTOREQPROV "no")
set (CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
set (CPACK_RPM_PACKAGE_URL "${CPACK_PACKAGE_HOMEPAGE_URL}")
# The Version tag keeps plain numeric components; the build metadata after
# '+' (git hash etc.) moves into the Release part.
if (PACKAGE_VERSION MATCHES "^(.*)\\+(.*)$")
  set (CPACK_RPM_PACKAGE_VERSION "${CMAKE_MATCH_1}")
  set (CPACK_RPM_PACKAGE_RELEASE "1.${CMAKE_MATCH_2}")
else ()
  set (CPACK_RPM_PACKAGE_VERSION "${PACKAGE_VERSION}")
  set (CPACK_RPM_PACKAGE_RELEASE "1")
endif (PACKAGE_VERSION MATCHES "^(.*)\\+(.*)$")

## The generator list: DEB always; RPM as soon as rpmbuild is available.
set (CPACK_GENERATOR "DEB")
find_program (RPMBUILD_EXECUTABLE NAMES rpmbuild)
if (RPMBUILD_EXECUTABLE)
  list (APPEND CPACK_GENERATOR "RPM")
  message (STATUS
    "- RPM: ${CPACK_RPM_PACKAGE_NAME} (${CPACK_RPM_PACKAGE_VERSION}"
    "-${CPACK_RPM_PACKAGE_RELEASE}, BuildArch: noarch)"
  )
else (RPMBUILD_EXECUTABLE)
  message (STATUS "- RPM: disabled (rpmbuild not found: sudo apt-get install rpm)")
endif (RPMBUILD_EXECUTABLE)

# Fresh-version hook: cpack includes this file per generator after
# CPackConfig.cmake, i.e. after the configure-time set() calls, and overrides
# the version with the current content of version.txt. It must be set before
# include (CPack): everything starting with CPACK is encoded into the config.
configure_file (
  ${CMAKE_CURRENT_LIST_DIR}/cpack-project-config.cmake.in
  ${CMAKE_BINARY_DIR}/cpack-project-config.cmake
  @ONLY
)
set (CPACK_PROJECT_CONFIG_FILE "${CMAKE_BINARY_DIR}/cpack-project-config.cmake")

include (CPack)

# Packaging entry points with a project-style banner and a clear setup hint
# for a missing rpmbuild (the `rpm` target exists regardless of the enabled
# generator list).
foreach (PACKAGING_TARGET IN ITEMS deb rpm)
  add_custom_target (${PACKAGING_TARGET}
    COMMAND ${CMAKE_COMMAND}
      "-DBINARY_DIR=${CMAKE_BINARY_DIR}"
      "-DVERSION_FILE=${CMAKE_BINARY_DIR}/version.txt"
      "-DGENERATOR_NAME=${PACKAGING_TARGET}"
      "-DCPACK_COMMAND=${CMAKE_CPACK_COMMAND}"
      -P ${CMAKE_CURRENT_LIST_DIR}/run-cpack.cmake
    COMMENT
      "Packaging (${PACKAGING_TARGET})"
    VERBATIM
  )
  add_dependencies (${PACKAGING_TARGET} version)
  # The packages ship only sources, but the project gates every build step
  # behind the checks/tests, so the packaging targets do the same.
  if (TARGET run_tests)
    add_dependencies (${PACKAGING_TARGET} run_tests)
  elseif (TARGET run_checks)
    add_dependencies (${PACKAGING_TARGET} run_checks)
  endif (TARGET run_tests)
endforeach (PACKAGING_TARGET IN ITEMS deb rpm)
