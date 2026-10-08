#!/usr/bin/env bash
# Co-compile pipeline: cppcheck -> clang-tidy (both before compiling).
#
# Invoked by CMake's `cmake -E __run_co_compile --tidy=...` handler as:
#
#   check-pipeline.sh --cpp <cppcheck flags...> --run-tidy <tidy flags...> \
#                     <source> -- <compiler command...>
#
# CMake's native co-compile slot order is tidy -> cppcheck, which is the
# reverse of the required order, so both tools are hosted in this wrapper
# (CMAKE_CXX_CPPCHECK stays unset): cppcheck runs first, then clang-tidy,
# and on success CMake runs the compiler itself.
#
# The wrapper's stdout is always relayed by CMake to stderr, while its
# stderr is shown only on a non-zero exit. Therefore banners and merged
# tool output go to stdout, and a failure summary goes to stderr.
# Third-party sources under _deps/ are skipped.

set -u

fail () {
  printf '[check] pipeline: %s\n' "$*" >&2
  exit 1
}

## Split the arguments on the first bare "--": options + source before it,
## the original compile command after it.
opt_args=()
compiler_cmd=()
in_compiler=0
for arg in "$@"; do
  if (( in_compiler )); then
    compiler_cmd+=("$arg")
  elif [[ "$arg" == "--" ]]; then
    in_compiler=1
  else
    opt_args+=("$arg")
  fi
done

(( ${#opt_args[@]} > 0 )) || fail "missing options/source arguments"
(( ${#compiler_cmd[@]} > 0 )) || fail "missing compile command after --"

## The last option argument is the source file (appended by CMake).
src="${opt_args[$((${#opt_args[@]} - 1))]}"
unset "opt_args[$((${#opt_args[@]} - 1))]"

## Split the remaining options into the two tool sections.
cpp_section=()
tidy_section=()
mode=""
for arg in "${opt_args[@]}"; do
  case "$arg" in
    --cpp) mode="cpp" ;;
    --run-tidy) mode="tidy" ;;
    *)
      case "$mode" in
        cpp) cpp_section+=("$arg") ;;
        tidy) tidy_section+=("$arg") ;;
        *) fail "unexpected argument before --cpp: $arg" ;;
      esac
      ;;
  esac
done

(( ${#cpp_section[@]} > 0 )) || fail "empty cppcheck command"
(( ${#tidy_section[@]} > 0 )) || fail "empty clang-tidy command"

## Third-party code (e.g. FetchContent's _deps) is not checked.
if [[ "$src" == *'/_deps/'* ]]; then
  printf '[check] skip third-party: %s\n' "$src"
  exit 0
fi

## Extract the -D/-I/-U options from the compile line (mirrors CMake's
## native HandleCppCheck behaviour) and append the source file.
extracted=()
for opt in "${compiler_cmd[@]}"; do
  if (( ${#opt} > 2 )) && [[ "$opt" == -* ]]; then
    case "${opt:1:1}" in
      D|I|U) extracted+=("$opt") ;;
    esac
  fi
done

printf '[check] cppcheck start: %s\n' "$src"
"${cpp_section[@]}" "${extracted[@]}" "$src" 2>&1
rc=$?
if (( rc != 0 )); then
  printf '[check] cppcheck failed (rc=%d): %s\n' "$rc" "$src" >&2
  exit "$rc"
fi
printf '[check] cppcheck done: %s\n' "$src"

printf '[check] clang-tidy start: %s\n' "$src"
"${tidy_section[@]}" "$src" -- "${compiler_cmd[@]}" 2>&1
rc=$?
if (( rc != 0 )); then
  printf '[check] clang-tidy failed (rc=%d): %s\n' "$rc" "$src" >&2
  exit "$rc"
fi
printf '[check] clang-tidy done: %s\n' "$src"
