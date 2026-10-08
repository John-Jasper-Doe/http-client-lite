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


## Building tests

```bash
cmake -B build -DHTTP_CLIENT_LITE_BUILD_TESTS=ON
cmake --build build
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

Alternatively, keep the `STYLE_CHECK` option enabled (it is ON by default):
before every build the whole codebase is verified with `clang-format --dry-run`,
and the build fails on any violation. Disable it with:

```bash
cmake -B build -DSTYLE_CHECK=OFF
```


## Static analysis (cppcheck + clang-tidy)

[cppcheck](https://cppcheck.sourceforge.io/) and
[clang-tidy](https://clang.llvm.org/extra/clang-tidy/) run automatically
before every compilation and cannot be turned off per target: the build
fails on any finding, and the configuration fails when a required tool is
missing — all missing tools are reported at once with an installation hint
(`sudo apt-get install cppcheck clang-tidy clang-format`). The
`HTTP_CLIENT_LITE_ENABLE_CHECKS` option (ON by default) exists only for
the nested tooling builds of the `sanitizer` and `coverage` targets.

The order per translation unit is fixed: `cppcheck` -> `clang-tidy` ->
compilation. Both checks are executed by the `cmake/check-pipeline.sh`
wrapper, and each step is echoed into the build log
(`[check] cppcheck start: ...`, `[check] clang-tidy start: ...`).

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
static analysis pipeline (`-DHTTP_CLIENT_LITE_ENABLE_CHECKS=OFF`) for
speed.


