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
bodies wrapped in braces).

Manual formatting via CMake target:

```bash
cmake --build build --target clang_format
```

Pre-commit hook formats staged C/C++ files automatically. Enable it once per clone:

```bash
git config core.hooksPath hooks
```


