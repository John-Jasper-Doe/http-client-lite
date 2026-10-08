#!/usr/bin/env bash
# Summarize gcov coverage of the project headers.
#
# Usage: coverage-summary.sh <build-dir> <source-dir> <report-dir>
#
# Runs gcov over every .gcda file of an instrumented build, keeps hits
# inside <source-dir>/include, prints per-file line/branch coverage and
# an aggregate summary to the log, and fails when no project data is
# found. When gcovr is on PATH an HTML report is written to
# <report-dir>/html as well.

set -u

# Stable decimal separator in the printed percentages.
export LC_ALL=C

fail () {
  printf '[coverage] %s\n' "$*" >&2
  exit 1
}

command -v gcov > /dev/null || fail "gcov not found"
[ -d "${1:?build dir argument is required}" ] ||
  fail "build dir not found: $1"

# Absolute paths: gcov runs from the report directory.
build_dir=$(realpath "$1")
src_dir=$(realpath "$2")
mkdir -p "${3:?report dir argument is required}"
report_dir=$(realpath "$3")

gcov_dir=$report_dir/gcov
rm -rf "$gcov_dir"
mkdir -p "$gcov_dir"

mapfile -t gcda_files < <(find "$build_dir" -name '*.gcda' | sort)
[ ${#gcda_files[@]} -gt 0 ] ||
  fail "no .gcda files under $build_dir (was the build instrumented?)"

# gcov writes one .gcov file per object into the current directory;
# duplicates for the same source are overwritten, which is fine.
(
  cd "$gcov_dir" &&
  gcov -b "${gcda_files[@]}" > gcov.log 2>&1
) || fail "gcov failed (see $gcov_dir/gcov.log)"

total_lines=0
total_hit=0
total_branches=0
total_branch_hit=0
files=0

printf '[coverage] %-60s %14s %14s\n' "file" "lines" "branch"
for gcov_file in "$gcov_dir"/*.gcov; do
  [ -e "$gcov_file" ] || continue
  # First line: "        -:    0:Source:/path" (quotes optional by version)
  src_line=$(head -n 1 "$gcov_file")
  src=${src_line#*Source:}
  src=${src#\'}
  src=${src%\'}
  case $src in
    "$src_dir"/include/*) ;;
    *) continue ;;
  esac

  lines_hit=$(grep -cE '^[[:space:]]*[0-9]+:' "$gcov_file")
  lines_total=$(grep -cE '^[[:space:]]*([0-9]+|#####):' "$gcov_file")
  branch_total=$(grep -c '^branch' "$gcov_file")
  branch_hit=$(grep -cE '^branch .* taken [1-9][0-9]*%' "$gcov_file")

  total_lines=$((total_lines + lines_total))
  total_hit=$((total_hit + lines_hit))
  total_branches=$((total_branches + branch_total))
  total_branch_hit=$((total_branch_hit + branch_hit))
  files=$((files + 1))

  if [ "$lines_total" -gt 0 ]; then
    lines_pct=$(awk -v h="$lines_hit" -v t="$lines_total" \
      'BEGIN { printf "%.1f", 100 * h / t }')
  else
    lines_pct="n/a"
  fi
  if [ "$branch_total" -gt 0 ]; then
    branch_pct=$(awk -v h="$branch_hit" -v t="$branch_total" \
      'BEGIN { printf "%.1f", 100 * h / t }')
  else
    branch_pct="n/a"
  fi

  rel=${src#"$src_dir"/}
  printf '[coverage] %-60s %14s %14s\n' "$rel" \
    "${lines_hit}/${lines_total} (${lines_pct}%)" \
    "${branch_hit}/${branch_total} (${branch_pct}%)"
done

[ "$files" -gt 0 ] || fail "no coverage data for $src_dir/include"

if [ "$total_lines" -gt 0 ]; then
  total_pct=$(awk -v h="$total_hit" -v t="$total_lines" \
    'BEGIN { printf "%.1f", 100 * h / t }')
else
  total_pct="n/a"
fi
if [ "$total_branches" -gt 0 ]; then
  total_branch_pct=$(awk -v h="$total_branch_hit" -v t="$total_branches" \
    'BEGIN { printf "%.1f", 100 * h / t }')
else
  total_branch_pct="n/a"
fi

printf '[coverage] -----\n'
printf '[coverage] files:  %s\n' "$files"
printf '[coverage] lines:  %s/%s (%s%%)\n' \
  "$total_hit" "$total_lines" "$total_pct"
printf '[coverage] branch: %s/%s (%s%%)\n' \
  "$total_branch_hit" "$total_branches" "$total_branch_pct"

if command -v gcovr > /dev/null; then
  mkdir -p "$report_dir/html"
  if gcovr -r "$src_dir" --html-details -o "$report_dir/html/index.html" \
      "$build_dir" > "$report_dir/gcovr.log" 2>&1; then
    printf '[coverage] HTML report: %s\n' "$report_dir/html/index.html"
  else
    printf '[coverage] gcovr failed (see %s)\n' \
      "$report_dir/gcovr.log" >&2
  fi
fi

exit 0
