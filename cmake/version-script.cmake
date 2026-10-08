# version-script.cmake — the git version banner (runs as `cmake -P`; it is
# invoked by cmake/version.cmake at configure time and on every build).
#
# Inputs (passed with -D):
#   VERSION_SOURCE_DIR — the git working tree (the project source directory)
#   VERSION_FILE       — optional; stores the bare version string and is
#                        rewritten only when the value actually changes
#   GIT_EXECUTABLE     — optional; the path to git found at configure time
#
# Version layout (Major.Minor.Patch taken from git plus build metadata):
#   - Major.Minor — the newest version tag reachable from HEAD (tag 0.1 -> 0.1);
#   - Patch       — the number of commits since that tag;
#   - master      — release, no suffix:   0.1.26
#   - develop     — dev plus short hash:  0.1.26+dev.g99b3881
#   - anything else (incl. detached HEAD):
#                   dirty plus short hash: 0.1.26+dirty.g99b3881
#
# Without git, outside a repository or when a git call fails, the version
# degrades gracefully (`unknown`, empty parts); the banner never fails the
# build.

cmake_minimum_required (VERSION 3.20)

if (NOT DEFINED VERSION_SOURCE_DIR)
  set (VERSION_SOURCE_DIR ".")
endif (NOT DEFINED VERSION_SOURCE_DIR)

# The build date is stamped on every run of this script.
string (TIMESTAMP VERSION_DATE "%Y-%m-%dT%H:%M:%SZ" UTC)

# run_git(<out-var> <git-args...>): git in VERSION_SOURCE_DIR; the result
# variable is empty when the command fails.
function (run_git OUT_VAR)
  execute_process (
    COMMAND ${GIT_EXE} -C "${VERSION_SOURCE_DIR}" ${ARGN}
    OUTPUT_VARIABLE GIT_OUT
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE GIT_RC
  )
  if (GIT_RC EQUAL 0)
    set (${OUT_VAR} "${GIT_OUT}" PARENT_SCOPE)
  else ()
    set (${OUT_VAR} "" PARENT_SCOPE)
  endif (GIT_RC EQUAL 0)
endfunction (run_git)

if (DEFINED GIT_EXECUTABLE AND GIT_EXECUTABLE AND EXISTS "${GIT_EXECUTABLE}")
  set (GIT_EXE "${GIT_EXECUTABLE}")
else ()
  find_program (GIT_EXE NAMES git)
endif ()

set (VERSION_STR "unknown")

if (GIT_EXE)
  run_git (IS_REPOSITORY rev-parse --git-dir)
  if (IS_REPOSITORY)
    # Major.Minor come from the newest version tag (a leading "v" is tolerated).
    run_git (VERSION_TAG describe --tags --abbrev=0
      --match "[0-9]*" --match "v[0-9]*"
    )
    # rev-list needs the raw ref name; only the numeric part is "v"-stripped.
    string (REGEX REPLACE "^v" "" VERSION_TAG_NUM "${VERSION_TAG}")

    set (MAJOR 0)
    set (MINOR 0)
    if (VERSION_TAG_NUM MATCHES "^([0-9]+)\\.([0-9]+)")
      set (MAJOR "${CMAKE_MATCH_1}")
      set (MINOR "${CMAKE_MATCH_2}")
    elseif (VERSION_TAG_NUM MATCHES "^([0-9]+)")
      set (MAJOR "${CMAKE_MATCH_1}")
    endif (VERSION_TAG_NUM MATCHES "^([0-9]+)\\.([0-9]+)")

    # Patch: the number of commits since that tag (the whole history without one).
    if (VERSION_TAG)
      run_git (PATCH_COUNT rev-list --count "${VERSION_TAG}..HEAD")
    else ()
      run_git (PATCH_COUNT rev-list --count HEAD)
    endif (VERSION_TAG)
    if (NOT PATCH_COUNT MATCHES "^[0-9]+$")
      set (PATCH_COUNT 0)
    endif (NOT PATCH_COUNT MATCHES "^[0-9]+$")

    run_git (VERSION_HASH rev-parse --short HEAD)
    run_git (VERSION_BRANCH rev-parse --abbrev-ref HEAD)

    set (VERSION_STR "${MAJOR}.${MINOR}.${PATCH_COUNT}")
    if (VERSION_BRANCH STREQUAL "master")
      # Release: the version stays bare.
    elseif (VERSION_BRANCH STREQUAL "develop")
      set (VERSION_STR "${VERSION_STR}+dev")
    else ()
      set (VERSION_STR "${VERSION_STR}+dirty")
    endif (VERSION_BRANCH STREQUAL "master")
    if (NOT VERSION_BRANCH STREQUAL "master" AND VERSION_HASH)
      string (APPEND VERSION_STR ".g${VERSION_HASH}")
    endif (NOT VERSION_BRANCH STREQUAL "master" AND VERSION_HASH)
  endif (IS_REPOSITORY)
endif (GIT_EXE)

# The bare version string: stable (no date), so a consumer of the file is
# rebuilt only when the version itself changes.
if (DEFINED VERSION_FILE AND VERSION_FILE)
  set (VERSION_OLD "")
  if (EXISTS "${VERSION_FILE}")
    file (READ "${VERSION_FILE}" VERSION_OLD)
    string (STRIP "${VERSION_OLD}" VERSION_OLD)
  endif (EXISTS "${VERSION_FILE}")
  if (NOT VERSION_OLD STREQUAL VERSION_STR)
    file (WRITE "${VERSION_FILE}" "${VERSION_STR}\n")
  endif (NOT VERSION_OLD STREQUAL VERSION_STR)
endif (DEFINED VERSION_FILE AND VERSION_FILE)

message (STATUS "[version] ${VERSION_STR} (built ${VERSION_DATE})")
