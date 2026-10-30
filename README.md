# HPrint

[![CI](https://github.com/MattFor/hprint/actions/workflows/ci.yml/badge.svg)](https://github.com/MattFor/hprint/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/MattFor/hprint)](https://github.com/MattFor/hprint/releases/latest)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

**Minimal, header-only C++ utility for printing centered terminal headers.**

## Features

- Header-only (simply include and go)
- Centered text with customizable borders
- Compile-time borders: a character, or a UTF-8 string such as `─`
- Optional formatted input
- Prints to stdout, stderr or any `std::FILE*`
- Automatic terminal width detection, with `COLUMNS` when the output is not a terminal
- UTF-8 aware: wide characters, combining marks, emoji sequences and escape sequences are measured as a terminal
  draws them
- Lines can be built at compile time
- No dependencies beyond the standard library

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

    hp::print<"─">("Box drawing", 60);
    hp::print<"«", "»">("🚀 Deploy 👨‍👩‍👧", 60);

    hp::printfw<'-'>(60, "Done in {:.2f}s", 1.5);
    hp::printf<'!'>(stderr, "{} warnings", 3);
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
─────────────────────── Box drawing ────────────────────────
««««««««««««««««««««««« 🚀 Deploy 👨‍👩‍👧 »»»»»»»»»»»»»»»»»»»»»»»
---------------------- Done in 1.50s -----------------------
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 3 warnings !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
```

## What it has

### `hp::border`

```cpp
struct border
{
    constexpr border(char character);
    template<std::size_t N> constexpr border(const char (&text)[N]); // at most 16 bytes
};
```

What the template parameters below take: `'='` or `"─"`. A border wider than one column is repeated as often as it
fits, and the columns left over are spaces next to the title, so the line is always exactly `width` wide. `""` gives
spaces only.

### `hp::print`

```cpp
template<hp::border L = '=', hp::border R = L>
void print(std::string_view title, int width = terminal_width());

template<hp::border L = '=', hp::border R = L>
void print(std::FILE* stream, std::string_view title);             // as wide as stream's terminal

template<hp::border L = '=', hp::border R = L>
void print(std::FILE* stream, std::string_view title, int width);
```

Print a centered header.

### `hp::printf`

```cpp
template<hp::border L = '=', hp::border R = L, class... Args>
void printf(std::format_string<Args...> fmt, Args&&... args);

template<hp::border L = '=', hp::border R = L, class... Args>
void printf(std::FILE* stream, std::format_string<Args...> fmt, Args&&... args);
```

Formatted version using `std::format`, at the detected terminal width.

### `hp::printfw`

```cpp
template<hp::border L = '=', hp::border R = L, class... Args>
void printfw(int width, std::format_string<Args...> fmt, Args&&... args);

template<hp::border L = '=', hp::border R = L, class... Args>
void printfw(std::FILE* stream, int width, std::format_string<Args...> fmt, Args&&... args);
```

Formatted version at an explicit width.

### `hp::make_header_line`

```cpp
template<hp::border L = '=', hp::border R = L>
[[nodiscard]] constexpr std::string make_header_line(std::string_view title, int width = terminal_width());
```

Builds the line instead of printing it, at compile time too:
`static_assert(hp::make_header_line("hi", 10) == "=== hi ===")`. Returns `title` unchanged when it does not fit in
`width`. A non-positive `width` falls back to `hp::default_width`.

### `hp::terminal_width`

```cpp
[[nodiscard]] int terminal_width() noexcept;                  // stdout
[[nodiscard]] int terminal_width(std::FILE* stream) noexcept;
```

Columns of the terminal the stream writes to. When it is not a terminal: `COLUMNS` if that is a positive number (POSIX),
otherwise `hp::default_width` (80).

### `hp::display_width`

```cpp
[[nodiscard]] constexpr std::size_t display_width(std::string_view text) noexcept;
```

Columns `text` takes in a terminal: wide characters count two, combining marks and escape sequences (colours,
hyperlinks, `tput sgr0`) none, and an emoji with a skin tone, `VS16` or ZWJ joins (👨‍👩‍👧) counts as one emoji.

## How to get it / Enable it

1. Through FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
        hprint
        GIT_REPOSITORY https://github.com/MattFor/hprint.git
        GIT_TAG v1.3.0
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

- C++23 or higher (`<print>` and `<format>`)
- GCC 14+, Clang 18+ with libc++/libstdc++, or MSVC 19.40+
- Any platform - terminal size is detected on POSIX and Windows, and falls back to 80 columns elsewhere

## Notes and limitations

- `hp::display_width` follows the Unicode 16 East Asian Width and combining-mark tables (regenerated with
  `python3 tools/unicode-tables.py`) plus the emoji rules above. It is not full grapheme segmentation (UAX #29), and
  terminals disagree on some emoji, so an unusual cluster may still be measured differently from how it renders.
- The strings are taken as UTF-8: with MSVC, compile with `/utf-8`.
- `hp::printf` is unrelated to C's `printf`. If you write `using namespace hp;`
  the C function still wins overload resolution for string-literal calls, so prefer calling it qualified as
  `hp::printf`.
- On Windows, `<windows.h>` is included with `WIN32_LEAN_AND_MEAN` and
  `NOMINMAX` set only if they were not already defined, and they are restored afterwards.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).

## License

MIT, see [LICENSE](LICENSE). By [MattFor](https://github.com/MattFor).
