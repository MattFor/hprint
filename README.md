# hprint

**Minimal, header-only C++ utility for printing centered terminal headers.**

## Features

* Header-only (simply include and go)
* Centered text with customizable borders
* Compile-time border characters
* Optional formatted input
* Automatic terminal width detection
* UTF-8 aware
* No dependencies beyond the standard library

## Usage

```cpp
#include <hprint>

int main()
{
    hp::print<'>'>("Build started");
    hp::printf<'#'>("Phase {}", 1);
    hp::printf<'[', ']'>("User: {}", "mattfor");
    hp::print("Custom Width Test", 120);

    hp::print<'~'>("Ünïcödé", 60);
    hp::print<'*'>("日本語のヘッダー", 60);

    hp::printfw<'-'>(60, "Done in {:.2f}s", 1.5);
}
```

Example output:

```
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Build started >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
################################### Phase 1 ####################################
[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[ User: mattfor ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
================================================== Custom Width Test ===================================================
~~~~~~~~~~~~~~~~~~~~~~~~~ Ünïcödé ~~~~~~~~~~~~~~~~~~~~~~~~~~
********************* 日本語のヘッダー *********************
---------------------- Done in 1.50s -----------------------
```

## What it has

### `hp::print`

```cpp
template<char L = '=', char R = L>
void print(std::string_view title, int width = terminal_width());
```

Print a centered header.

### `hp::printf`

```cpp
template<char L = '=', char R = L, class... Args>
void printf(std::format_string<Args...> fmt, Args&&... args);
```

Formatted version using `std::format`, at the detected terminal width.

### `hp::printfw`

```cpp
template<char L = '=', char R = L, class... Args>
void printfw(int width, std::format_string<Args...> fmt, Args&&... args);
```

Formatted version at an explicit width.

### `hp::make_header_line`

```cpp
template<char L = '=', char R = L>
[[nodiscard]] std::string make_header_line(std::string_view title, int width = terminal_width());
```

Builds the line instead of printing it. Returns `title` unchanged when it does not fit in `width`. A non-positive
`width` falls back to `hp::default_width`.

### `hp::terminal_width`

```cpp
[[nodiscard]] int terminal_width() noexcept;
```

Columns of the attached terminal, or `hp::default_width` (80) when stdout is not a terminal or the size cannot be
queried.

### `hp::display_width`

```cpp
[[nodiscard]] constexpr std::size_t display_width(std::string_view text) noexcept;
```

## How to get it / Enable it

1. Through FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
        hprint
        GIT_REPOSITORY https://github.com/MattFor/hprint.git
        GIT_TAG v1.2.0
)

FetchContent_MakeAvailable(hprint)

target_link_libraries(target_name PRIVATE hp::hprint)
```

2. Or already installed

```cmake
find_package(hprint CONFIG REQUIRED)

target_link_libraries(target_name PRIVATE hp::hprint)
```

## Requirements

* C++23 or higher (`<print>` and `<format>`)
* GCC 14+, Clang 18+ with libc++/libstdc++, or MSVC 19.40+
* Any platform - terminal size is detected on POSIX and Windows, and falls back to 80 columns elsewhere

## Notes and limitations

* `hp::display_width` is a pragmatic approximation, not a full UAX #11 implementation. Emoji ZWJ sequences are measured
  per-codepoint, so a multi-codepoint emoji cluster may be counted wider than it renders.
* `hp::printf` is unrelated to C's `printf`. If you write `using namespace hp;`
  the C function still wins overload resolution for string-literal calls, so prefer calling it qualified as
  `hp::printf`.
* On Windows, `<windows.h>` is included with `WIN32_LEAN_AND_MEAN` and
  `NOMINMAX` set only if they were not already defined, and they are restored afterwards.

## License

MIT License © 2026

See [LICENSE](LICENSE) for details.

Version 1.2.0
