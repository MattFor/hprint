# Changelog

## [1.3.0] - 07:32 CEST 30-10-2026

### Added

- UTF-8 borders: `hp::print<"─">(...)`, `hp::print<"«", "»">(...)`. The border template parameters are now
  `hp::border`, which a `char` still converts to, so existing code compiles unchanged.
- `std::FILE*` overloads of `hp::print`, `hp::printf` and `hp::printfw`, to print to stderr or a file, and
  `hp::terminal_width(std::FILE*)`.
- `hp::make_header_line` is `constexpr`.
- `COLUMNS` sets the width when the output is not a terminal (POSIX).
- `tools/unicode-tables.py`, which generates the width tables.

### Fixed

- Wide characters missing from the hand-written table (⌚, ☕, 🀄, 🈁, 🫠 and others) are taken from Unicode 16, and
  all combining marks and invisible format characters now take no columns.
- Emoji ZWJ sequences (👨‍👩‍👧), skin tones (👍🏽) and `VS16` emoji (❤️) are measured as one emoji.
- Escape sequences other than CSI, such as hyperlinks (`ESC ] 8`) and the `ESC ( B` of `tput sgr0`, were counted as
  text.

### Changed

- `hp::display_width` looks code points up with a binary search instead of a linear scan.
- The tests and the demo are compiled with `/utf-8` on MSVC, as their strings are UTF-8.

## [1.2.0] - 15:30 CEST 22-08-2026

### Added

- `hp::printfw` formatted header at an explicit width.
- `hp::display_width` UTF-8 aware column count, usable in constant expressions.
- `hp::default_width` constant replacing the hard-coded `80` literals.
- `HPRINT_VERSION_MAJOR` / `_MINOR` / `_PATCH` / `_STRING` macros.
- Added tests.
- CI covering Linux, macOS and Windows.
- `HPRINT_BUILD_EXAMPLES`, `HPRINT_BUILD_TESTS` and `HPRINT_INSTALL` options.

### Fixed

- Misaligned headers on non-ascii headers.
- Missing standard includes.
- `<windows.h>` was included without `WIN32_LEAN_AND_MEAN` / `NOMINMAX`, leaking the `min` and `max` macros. Oops...
- `terminal_width()` on Windows did not reject a null handle or a non-positive computed column count.
- `$<INSTALL_INTERFACE:include>` was hard-coded instead of using `${CMAKE_INSTALL_INCLUDEDIR}`.
- `make_header_line` under-reserved its buffer for multi-byte titles.

### Changed

- `hprintConfig.cmake.in` now calls `check_required_components`.
- The version file is marked `ARCH_INDEPENDENT`, so a package built on one architecture is accepted on another.
- `std::print("{}\n", ...)` replaced with `std::println("{}", ...)`.

## [1.1.0] - 17:32 CEST 13-04-2026

### Added

- Ability to include the library as `<hprint>` like a system header.

## [1.0.0] - 19:43 CEST 31-03-2026

- Initial release.

[1.3.0]: https://github.com/MattFor/hprint/releases/tag/v1.3.0
[1.2.0]: https://github.com/MattFor/hprint/releases/tag/v1.2.0
[1.1.0]: https://github.com/MattFor/hprint/releases/tag/v1.1.0
[1.0.0]: https://github.com/MattFor/hprint/releases/tag/v1.0.0
