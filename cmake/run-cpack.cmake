# run-cpack.cmake — the `deb`/`rpm` build targets (runs as `cmake -P`;
# invoked by cmake/packaging.cmake).
#
# Inputs (passed with -D):
#   BINARY_DIR     — the configured build tree (CPackConfig.cmake lives there)
#   VERSION_FILE   — the version file refreshed by the `version` target
#   GENERATOR_NAME — packaging target name: deb | rpm
#   CPACK_COMMAND  — the cpack executable
#
# The current version reaches the package through CPACK_PROJECT_CONFIG_FILE
# (cmake/cpack-project-config.cmake.in): cpack re-reads version.txt itself
# while packaging, so this wrapper only provides the banner, the generator
# selection and a clear setup hint for a missing rpmbuild.

cmake_minimum_required (VERSION 3.20)

if (NOT DEFINED BINARY_DIR
    OR NOT DEFINED VERSION_FILE
    OR NOT DEFINED GENERATOR_NAME
    OR NOT DEFINED CPACK_COMMAND)
  message (FATAL_ERROR
    "run-cpack.cmake: BINARY_DIR, VERSION_FILE, GENERATOR_NAME and"
    " CPACK_COMMAND are required"
  )
endif (NOT DEFINED BINARY_DIR
    OR NOT DEFINED VERSION_FILE
    OR NOT DEFINED GENERATOR_NAME
    OR NOT DEFINED CPACK_COMMAND)

string (TOUPPER "${GENERATOR_NAME}" CPACK_GEN)

if (NOT EXISTS "${VERSION_FILE}")
  message (FATAL_ERROR "[${GENERATOR_NAME}] ${VERSION_FILE} not found")
endif (NOT EXISTS "${VERSION_FILE}")

# A clear hint instead of the generator's own FATAL error about rpmbuild.
if (CPACK_GEN STREQUAL "RPM")
  find_program (RPMBUILD_EXECUTABLE NAMES rpmbuild)
  if (NOT RPMBUILD_EXECUTABLE)
    message (FATAL_ERROR
      "[rpm] rpmbuild not found: sudo apt-get install rpm"
    )
  endif (NOT RPMBUILD_EXECUTABLE)
endif (CPACK_GEN STREQUAL "RPM")

file (READ "${VERSION_FILE}" PACKAGE_VERSION)
string (STRIP "${PACKAGE_VERSION}" PACKAGE_VERSION)
message (STATUS "[${GENERATOR_NAME}] package version: ${PACKAGE_VERSION}")

execute_process (
  COMMAND ${CPACK_COMMAND} -G ${CPACK_GEN}
  WORKING_DIRECTORY ${BINARY_DIR}
  RESULT_VARIABLE CPACK_RC
)
if (NOT CPACK_RC EQUAL 0)
  message (FATAL_ERROR "[${GENERATOR_NAME}] cpack failed (exit ${CPACK_RC})")
endif (NOT CPACK_RC EQUAL 0)
