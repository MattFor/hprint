# Contributing

## Building

GCC 14, Clang 20 or MSVC 2022 (C++23 `<print>`), CMake 3.23 or newer.

```sh
cmake -S . -B build -G Ninja
cmake --build build
ctest --test-dir build --output-on-failure
```

## Before a pull request

```sh
git ls-files '*.cpp' '*.hpp' include/hprint | xargs clang-format --dry-run --Werror
cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
clang-tidy -p build --warnings-as-errors='*' test/test_hprint.cpp example/demo.cpp
```

## Releasing

The version lives in `CMakeLists.txt` (`project(... VERSION x.y.z)`); `.github/bump-version x.y.z` copies it into the
header's `HPRINT_VERSION_*` macros, and a test fails if the two disagree. Rename `## [Unreleased]` in `CHANGELOG.md` to
the new version, commit `Release vx.y.z`, tag `vx.y.z` and push the tag: the release workflow publishes it.
