# Changelog

## [1.2.0] - 2026-08-22

### Added

* `hp::printfw` formatted header at an explicit width.
* `hp::display_width`  UTF-8 aware column count, usable in constant expressions.
* `hp::default_width` constant replacing the hard-coded `80` literals.
* `HPRINT_VERSION_MAJOR` / `_MINOR` / `_PATCH` / `_STRING` macros.
* Added tests.
* CI covering Linux, macOS and Windows.
* `HPRINT_BUILD_EXAMPLES`, `HPRINT_BUILD_TESTS` and `HPRINT_INSTALL` options.

### Fixed

* Misaligned headers on non-ascii headers.
* Missing standard includes.
* `<windows.h>` was included without `WIN32_LEAN_AND_MEAN` / `NOMINMAX`, leaking the `min` and `max` macros. Oops...
* `terminal_width()` on Windows did not reject a null handle or a non-positive computed column count.
* `$<INSTALL_INTERFACE:include>` was hard-coded instead of using `${CMAKE_INSTALL_INCLUDEDIR}`.
* `make_header_line` under-reserved its buffer for multi-byte titles.

### Changed

* `hprintConfig.cmake.in` now calls `check_required_components`.
* The version file is marked `ARCH_INDEPENDENT`, so a package built on one architecture is accepted on another.
* `std::print("{}\n", ...)` replaced with `std::println("{}", ...)`.

## [1.1.0] - 2026-04-13

### Added

* Ability to include the library as `<hprint>` like a system header.

## [1.0.0] - 2026-03-31

* Initial release.

[1.2.0]: https://github.com/MattFor/hprint/releases/tag/v1.2.0
[1.1.0]: https://github.com/MattFor/hprint/releases/tag/v1.1.0
[1.0.0]: https://github.com/MattFor/hprint/releases/tag/v1.0.0
