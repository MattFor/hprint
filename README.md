# hprint

**Minimal, header-only C++ utility for printing centered terminal headers.**

## Features

* Header-only (simply include and go)
* Centered text with customisable borders
* Compile-time border characters
* Optional formatted input
* Automatic terminal width detection

## Usage

```cpp
#include <hprint/hprint.hpp>

int main()
{
    hp::print<'>'>("Build started");
    hp::printf<'#'>("Phase {}", 1);
    hp::printf<'[', ']'>("User: {}", "mattfor");
    hp::print("Custom Width Test", 120);
}
```

Example output:

```
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Build started >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
################################### Phase 1 ####################################
[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[ User: mattfor ]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
================================================== Custom Width Test ===================================================
```

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

Formatted version using `std::format`.

Width can be manually overridden

## How to get it / Enable it 

1. Through FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
    hprint
    GIT_REPOSITORY https://github.com/MattFor/hprint.git
    GIT_TAG v1.0.0
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

* C++23 or higher
* POSIX system preferably but works on Windows as well

## License

MIT License © 2026

See [LICENSE](LICENSE) for details.

Version 1.0.0