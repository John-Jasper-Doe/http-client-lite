# HTTP Client lite: C++ Cross-platform header-only library

This is a lite, C++ cross-platform header-only client library for http request based 
on [csachs/picohttpclient](https://github.com/csachs/picohttpclient) project.

A Lightweight HTTP 1.1 client where to quickly do very simple HTTP requests, 
without adding larger dependencies to a project.


## License

http client lite  is distributed under the [MIT License](https://github.com/john-jasper-doe/http-client-lite/blob/master/LICENSE).


## Headers

```C++
#include <httpclientlite/common/common.hpp>
#include <httpclientlite/core/components.hpp>
#include <httpclientlite/core/grammar.hpp>
#include <httpclientlite/core/parser.hpp>
#include <httpclientlite/core/rule.hpp>
```


## Building

Tests are built and run **by default**, before the rest of the build
(the library itself is header-only, so the rest is examples and other
auxiliary targets). The `run_tests` build step runs `ctest` after the
test binaries are built; the build stops if any test fails, and the
remaining targets are not built:

```bash
cmake -B build
cmake --build build    # builds the tests, runs them, then builds the rest
```

Disable the whole test step with:

```bash
cmake -B build -DHTTP_CLIENT_LITE_BUILD_TESTS=OFF
```

The tests can still be run manually at any time:

```bash
ctest --test-dir build
```


## Code style

The code is formatted with [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
(`.clang-format`: 100 columns, 2-space indent, attached braces, single-statement
bodies wrapped in braces, `else` on its own line).

Before starting work, enable the pre-commit hook — it formats staged C/C++
files automatically on every commit (enable once per clone):

```bash
git config core.hooksPath hooks
```

Alternatively, keep the `HTTP_CLIENT_LITE_STYLE_CHECK` option enabled (it
is ON by default): the `run_checks` build step verifies the whole codebase
with `clang-format --dry-run` before the test/example binaries are built,
and the build fails on any violation. Disable the style stage with:

```bash
cmake -B build -DHTTP_CLIENT_LITE_STYLE_CHECK=OFF
```


## Code checks (cppcheck + clang-tidy)

[cppcheck](https://cppcheck.sourceforge.io/) and
[clang-tidy](https://clang.llvm.org/extra/clang-tidy/) run as the
`run_checks` build step, before the test and example binaries are
compiled: any finding fails the step and stops the build, so nothing is
built from unchecked sources. The step is stamped and re-runs only when
the checked sources or the `.clang-tidy` configuration change. The
configuration fails when a required tool is missing — all missing tools
are reported at once with an installation hint
(`sudo apt-get install cppcheck clang-tidy clang-format`).

The order per run is fixed: `clang-format` (when `HTTP_CLIENT_LITE_STYLE_CHECK`
is ON) -> `cppcheck` -> `clang-tidy`; each step is echoed into the build
log (`[check] clang-format start`, `[check] cppcheck start`,
`[check] clang-tidy start`). The set of analyzed sources follows what is
about to be built (the test sources when tests are enabled, the example
sources when examples are enabled).

The cppcheck check is pedantic: `--inconclusive`, `--check-level=exhaustive`,
`--force` (all `#ifdef` configurations); warnings, style, performance,
portability and information findings are treated as errors
(`--error-exitcode=1`).

The clang-tidy check is configured by the `.clang-tidy` file at the
repository root: analyzer, bugprone, modernize, performance, readability and
core-guideline checks, every warning is treated as an error
(`WarningsAsErrors: '*'`), and header diagnostics are limited to
`include/httpclientlite/`.

To silence a false positive, put an inline suppression on the line above
(the build stays green):

```c++
// cppcheck-suppress <check-id>
// NOLINT(<check-name>)
```


## Runtime analysis (valgrind / sanitizer / coverage)

Separate build targets run the test binaries under external tools. A
missing tool only disables the corresponding target with a setup hint at
configure time:

```bash
cmake --build build --target valgrind    # memcheck every test binary
cmake --build build --target sanitizer   # nested ASan+UBSan build + tests
cmake --build build --target coverage    # nested --coverage build + summary
```

- `valgrind` runs `valgrind --leak-check=full` on each test binary; any
  error or definite/possible leak fails the target
  (`sudo apt-get install valgrind`).
- `sanitizer` configures a nested Debug build with
  `-fsanitize=address,undefined`, builds it and runs the tests with
  `ASAN_OPTIONS=halt_on_error=1:detect_leaks=1` and
  `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`; any finding fails
  the target. The nested build lives in `build/sanitizer/` and never
  overwrites the main build binaries. The set of sanitizers can be
  changed manually with `-DENABLE_SANITIZER=...`.
- `coverage` builds a nested instrumented Debug build (`--coverage`),
  runs the tests and prints a per-file line/branch coverage summary for
  `include/httpclientlite/` into the log. When
  [gcovr](https://gcovr.com/) is installed, an HTML report is written to
  `build/coverage/html/` as well (`sudo apt-get install gcovr`).

The nested builds reuse the already fetched Catch2 sources and skip the
style stage of `run_checks` (`-DHTTP_CLIENT_LITE_STYLE_CHECK=OFF`).


