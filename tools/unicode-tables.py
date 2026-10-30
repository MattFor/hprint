#!/usr/bin/env python3
"""Prints the width tables in include/hprint.hpp from this Python's Unicode database: python3 tools/unicode-tables.py"""

import unicodedata

# Visible despite being format characters: the soft hyphen and the prepended concatenation marks
VISIBLE_FORMAT = {0x00AD, *range(0x0600, 0x0606), 0x06DD, 0x070F, 0x0890, 0x0891, 0x08E2, 0x110BD, 0x110CD}


def wide(cp: int) -> bool:
	return unicodedata.east_asian_width(chr(cp)) in "WF"


def zero(cp: int) -> bool:
	category = unicodedata.category(chr(cp))
	if category in ("Mn", "Me") or (category == "Cf" and cp not in VISIBLE_FORMAT):
		return True
	# Hangul medial vowels and final consonants join the syllable before them
	return 0x1160 <= cp <= 0x11FF or 0xD7B0 <= cp <= 0xD7FF


def ranges(predicate) -> list[tuple[int, int]]:
	# Unassigned code points never end a range, and are never at either end of one
	found: list[tuple[int, int]] = []
	start = last = None
	for cp in range(0x110000):
		if unicodedata.category(chr(cp)) == "Cn" and not (0x1160 <= cp <= 0x11FF or 0xD7B0 <= cp <= 0xD7FF):
			continue
		if predicate(cp):
			start = cp if start is None else start
			last = cp
		elif start is not None:
			found.append((start, last))
			start = None
	if start is not None:
		found.append((start, last))
	return found


def table(name: str, predicate) -> str:
	cells = [f"{{ 0x{a:05X}, 0x{b:05X} }}" for a, b in ranges(predicate)]
	rows = [", ".join(cells[i : i + 4]) for i in range(0, len(cells), 4)]
	body = ",\n\t\t\t".join(rows)
	return f"\t\tinline constexpr codepoint_range {name}[] = {{\n\t\t\t{body}\n\t\t}};"


print(f"\t\t// Unicode {unicodedata.unidata_version}, from tools/unicode-tables.py")
print("\t\t// clang-format off")
print(table("wide_ranges", wide))
print()
print(table("zero_width_ranges", zero))
print("\t\t// clang-format on")
