#ifndef HPRINT_HPP_
#define HPRINT_HPP_

#pragma once

// Macros, so the version can be tested with #if
// NOLINTBEGIN(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum,modernize-macro-to-enum)
#define HPRINT_VERSION_MAJOR 1
#define HPRINT_VERSION_MINOR 2
#define HPRINT_VERSION_PATCH 0
#define HPRINT_VERSION_STRING "1.2.0"
// NOLINTEND(cppcoreguidelines-macro-usage,cppcoreguidelines-macro-to-enum,modernize-macro-to-enum)

#include <array>
#include <print>
#include <cstdio>
#include <format>
#include <string>
#include <cstddef>
#include <utility>
#include <algorithm>
#include <string_view>

#ifdef _WIN32
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
		#define HPRINT_UNDEF_WIN32_LEAN_AND_MEAN
	#endif

	#ifndef NOMINMAX
		#define NOMINMAX
		#define HPRINT_UNDEF_NOMINMAX
	#endif

	#include <io.h>
	#include <windows.h>

	#ifdef HPRINT_UNDEF_NOMINMAX
		#undef NOMINMAX
		#undef HPRINT_UNDEF_NOMINMAX
	#endif

	#ifdef HPRINT_UNDEF_WIN32_LEAN_AND_MEAN
		#undef WIN32_LEAN_AND_MEAN
		#undef HPRINT_UNDEF_WIN32_LEAN_AND_MEAN
	#endif
#elif defined( __unix__ ) || defined( __APPLE__ )
	#include <cstdlib>
	#include <charconv>
	#include <unistd.h>
	#include <sys/ioctl.h>
#endif

namespace hp
{
	inline constexpr int default_width = 80;

	namespace detail
	{
		struct codepoint_range
		{
			char32_t first;
			char32_t last;
		};

		// Unicode 16.0.0, from tools/unicode-tables.py
		// clang-format off
		inline constexpr codepoint_range wide_ranges[] = {
			{ 0x01100, 0x0115F }, { 0x0231A, 0x0231B }, { 0x02329, 0x0232A }, { 0x023E9, 0x023EC },
			{ 0x023F0, 0x023F0 }, { 0x023F3, 0x023F3 }, { 0x025FD, 0x025FE }, { 0x02614, 0x02615 },
			{ 0x02630, 0x02637 }, { 0x02648, 0x02653 }, { 0x0267F, 0x0267F }, { 0x0268A, 0x0268F },
			{ 0x02693, 0x02693 }, { 0x026A1, 0x026A1 }, { 0x026AA, 0x026AB }, { 0x026BD, 0x026BE },
			{ 0x026C4, 0x026C5 }, { 0x026CE, 0x026CE }, { 0x026D4, 0x026D4 }, { 0x026EA, 0x026EA },
			{ 0x026F2, 0x026F3 }, { 0x026F5, 0x026F5 }, { 0x026FA, 0x026FA }, { 0x026FD, 0x026FD },
			{ 0x02705, 0x02705 }, { 0x0270A, 0x0270B }, { 0x02728, 0x02728 }, { 0x0274C, 0x0274C },
			{ 0x0274E, 0x0274E }, { 0x02753, 0x02755 }, { 0x02757, 0x02757 }, { 0x02795, 0x02797 },
			{ 0x027B0, 0x027B0 }, { 0x027BF, 0x027BF }, { 0x02B1B, 0x02B1C }, { 0x02B50, 0x02B50 },
			{ 0x02B55, 0x02B55 }, { 0x02E80, 0x0303E }, { 0x03041, 0x03247 }, { 0x03250, 0x0A4C6 },
			{ 0x0A960, 0x0A97C }, { 0x0AC00, 0x0D7A3 }, { 0x0F900, 0x0FAD9 }, { 0x0FE10, 0x0FE19 },
			{ 0x0FE30, 0x0FE6B }, { 0x0FF01, 0x0FF60 }, { 0x0FFE0, 0x0FFE6 }, { 0x16FE0, 0x1B2FB },
			{ 0x1D300, 0x1D376 }, { 0x1F004, 0x1F004 }, { 0x1F0CF, 0x1F0CF }, { 0x1F18E, 0x1F18E },
			{ 0x1F191, 0x1F19A }, { 0x1F200, 0x1F320 }, { 0x1F32D, 0x1F335 }, { 0x1F337, 0x1F37C },
			{ 0x1F37E, 0x1F393 }, { 0x1F3A0, 0x1F3CA }, { 0x1F3CF, 0x1F3D3 }, { 0x1F3E0, 0x1F3F0 },
			{ 0x1F3F4, 0x1F3F4 }, { 0x1F3F8, 0x1F43E }, { 0x1F440, 0x1F440 }, { 0x1F442, 0x1F4FC },
			{ 0x1F4FF, 0x1F53D }, { 0x1F54B, 0x1F54E }, { 0x1F550, 0x1F567 }, { 0x1F57A, 0x1F57A },
			{ 0x1F595, 0x1F596 }, { 0x1F5A4, 0x1F5A4 }, { 0x1F5FB, 0x1F64F }, { 0x1F680, 0x1F6C5 },
			{ 0x1F6CC, 0x1F6CC }, { 0x1F6D0, 0x1F6D2 }, { 0x1F6D5, 0x1F6DF }, { 0x1F6EB, 0x1F6EC },
			{ 0x1F6F4, 0x1F6FC }, { 0x1F7E0, 0x1F7F0 }, { 0x1F90C, 0x1F93A }, { 0x1F93C, 0x1F945 },
			{ 0x1F947, 0x1F9FF }, { 0x1FA70, 0x1FAF8 }, { 0x20000, 0x323AF }
		};

		inline constexpr codepoint_range zero_width_ranges[] = {
			{ 0x00300, 0x0036F }, { 0x00483, 0x00489 }, { 0x00591, 0x005BD }, { 0x005BF, 0x005BF },
			{ 0x005C1, 0x005C2 }, { 0x005C4, 0x005C5 }, { 0x005C7, 0x005C7 }, { 0x00610, 0x0061A },
			{ 0x0061C, 0x0061C }, { 0x0064B, 0x0065F }, { 0x00670, 0x00670 }, { 0x006D6, 0x006DC },
			{ 0x006DF, 0x006E4 }, { 0x006E7, 0x006E8 }, { 0x006EA, 0x006ED }, { 0x00711, 0x00711 },
			{ 0x00730, 0x0074A }, { 0x007A6, 0x007B0 }, { 0x007EB, 0x007F3 }, { 0x007FD, 0x007FD },
			{ 0x00816, 0x00819 }, { 0x0081B, 0x00823 }, { 0x00825, 0x00827 }, { 0x00829, 0x0082D },
			{ 0x00859, 0x0085B }, { 0x00897, 0x0089F }, { 0x008CA, 0x008E1 }, { 0x008E3, 0x00902 },
			{ 0x0093A, 0x0093A }, { 0x0093C, 0x0093C }, { 0x00941, 0x00948 }, { 0x0094D, 0x0094D },
			{ 0x00951, 0x00957 }, { 0x00962, 0x00963 }, { 0x00981, 0x00981 }, { 0x009BC, 0x009BC },
			{ 0x009C1, 0x009C4 }, { 0x009CD, 0x009CD }, { 0x009E2, 0x009E3 }, { 0x009FE, 0x00A02 },
			{ 0x00A3C, 0x00A3C }, { 0x00A41, 0x00A51 }, { 0x00A70, 0x00A71 }, { 0x00A75, 0x00A75 },
			{ 0x00A81, 0x00A82 }, { 0x00ABC, 0x00ABC }, { 0x00AC1, 0x00AC8 }, { 0x00ACD, 0x00ACD },
			{ 0x00AE2, 0x00AE3 }, { 0x00AFA, 0x00B01 }, { 0x00B3C, 0x00B3C }, { 0x00B3F, 0x00B3F },
			{ 0x00B41, 0x00B44 }, { 0x00B4D, 0x00B56 }, { 0x00B62, 0x00B63 }, { 0x00B82, 0x00B82 },
			{ 0x00BC0, 0x00BC0 }, { 0x00BCD, 0x00BCD }, { 0x00C00, 0x00C00 }, { 0x00C04, 0x00C04 },
			{ 0x00C3C, 0x00C3C }, { 0x00C3E, 0x00C40 }, { 0x00C46, 0x00C56 }, { 0x00C62, 0x00C63 },
			{ 0x00C81, 0x00C81 }, { 0x00CBC, 0x00CBC }, { 0x00CBF, 0x00CBF }, { 0x00CC6, 0x00CC6 },
			{ 0x00CCC, 0x00CCD }, { 0x00CE2, 0x00CE3 }, { 0x00D00, 0x00D01 }, { 0x00D3B, 0x00D3C },
			{ 0x00D41, 0x00D44 }, { 0x00D4D, 0x00D4D }, { 0x00D62, 0x00D63 }, { 0x00D81, 0x00D81 },
			{ 0x00DCA, 0x00DCA }, { 0x00DD2, 0x00DD6 }, { 0x00E31, 0x00E31 }, { 0x00E34, 0x00E3A },
			{ 0x00E47, 0x00E4E }, { 0x00EB1, 0x00EB1 }, { 0x00EB4, 0x00EBC }, { 0x00EC8, 0x00ECE },
			{ 0x00F18, 0x00F19 }, { 0x00F35, 0x00F35 }, { 0x00F37, 0x00F37 }, { 0x00F39, 0x00F39 },
			{ 0x00F71, 0x00F7E }, { 0x00F80, 0x00F84 }, { 0x00F86, 0x00F87 }, { 0x00F8D, 0x00FBC },
			{ 0x00FC6, 0x00FC6 }, { 0x0102D, 0x01030 }, { 0x01032, 0x01037 }, { 0x01039, 0x0103A },
			{ 0x0103D, 0x0103E }, { 0x01058, 0x01059 }, { 0x0105E, 0x01060 }, { 0x01071, 0x01074 },
			{ 0x01082, 0x01082 }, { 0x01085, 0x01086 }, { 0x0108D, 0x0108D }, { 0x0109D, 0x0109D },
			{ 0x01160, 0x011FF }, { 0x0135D, 0x0135F }, { 0x01712, 0x01714 }, { 0x01732, 0x01733 },
			{ 0x01752, 0x01753 }, { 0x01772, 0x01773 }, { 0x017B4, 0x017B5 }, { 0x017B7, 0x017BD },
			{ 0x017C6, 0x017C6 }, { 0x017C9, 0x017D3 }, { 0x017DD, 0x017DD }, { 0x0180B, 0x0180F },
			{ 0x01885, 0x01886 }, { 0x018A9, 0x018A9 }, { 0x01920, 0x01922 }, { 0x01927, 0x01928 },
			{ 0x01932, 0x01932 }, { 0x01939, 0x0193B }, { 0x01A17, 0x01A18 }, { 0x01A1B, 0x01A1B },
			{ 0x01A56, 0x01A56 }, { 0x01A58, 0x01A60 }, { 0x01A62, 0x01A62 }, { 0x01A65, 0x01A6C },
			{ 0x01A73, 0x01A7F }, { 0x01AB0, 0x01B03 }, { 0x01B34, 0x01B34 }, { 0x01B36, 0x01B3A },
			{ 0x01B3C, 0x01B3C }, { 0x01B42, 0x01B42 }, { 0x01B6B, 0x01B73 }, { 0x01B80, 0x01B81 },
			{ 0x01BA2, 0x01BA5 }, { 0x01BA8, 0x01BA9 }, { 0x01BAB, 0x01BAD }, { 0x01BE6, 0x01BE6 },
			{ 0x01BE8, 0x01BE9 }, { 0x01BED, 0x01BED }, { 0x01BEF, 0x01BF1 }, { 0x01C2C, 0x01C33 },
			{ 0x01C36, 0x01C37 }, { 0x01CD0, 0x01CD2 }, { 0x01CD4, 0x01CE0 }, { 0x01CE2, 0x01CE8 },
			{ 0x01CED, 0x01CED }, { 0x01CF4, 0x01CF4 }, { 0x01CF8, 0x01CF9 }, { 0x01DC0, 0x01DFF },
			{ 0x0200B, 0x0200F }, { 0x0202A, 0x0202E }, { 0x02060, 0x0206F }, { 0x020D0, 0x020F0 },
			{ 0x02CEF, 0x02CF1 }, { 0x02D7F, 0x02D7F }, { 0x02DE0, 0x02DFF }, { 0x0302A, 0x0302D },
			{ 0x03099, 0x0309A }, { 0x0A66F, 0x0A672 }, { 0x0A674, 0x0A67D }, { 0x0A69E, 0x0A69F },
			{ 0x0A6F0, 0x0A6F1 }, { 0x0A802, 0x0A802 }, { 0x0A806, 0x0A806 }, { 0x0A80B, 0x0A80B },
			{ 0x0A825, 0x0A826 }, { 0x0A82C, 0x0A82C }, { 0x0A8C4, 0x0A8C5 }, { 0x0A8E0, 0x0A8F1 },
			{ 0x0A8FF, 0x0A8FF }, { 0x0A926, 0x0A92D }, { 0x0A947, 0x0A951 }, { 0x0A980, 0x0A982 },
			{ 0x0A9B3, 0x0A9B3 }, { 0x0A9B6, 0x0A9B9 }, { 0x0A9BC, 0x0A9BD }, { 0x0A9E5, 0x0A9E5 },
			{ 0x0AA29, 0x0AA2E }, { 0x0AA31, 0x0AA32 }, { 0x0AA35, 0x0AA36 }, { 0x0AA43, 0x0AA43 },
			{ 0x0AA4C, 0x0AA4C }, { 0x0AA7C, 0x0AA7C }, { 0x0AAB0, 0x0AAB0 }, { 0x0AAB2, 0x0AAB4 },
			{ 0x0AAB7, 0x0AAB8 }, { 0x0AABE, 0x0AABF }, { 0x0AAC1, 0x0AAC1 }, { 0x0AAEC, 0x0AAED },
			{ 0x0AAF6, 0x0AAF6 }, { 0x0ABE5, 0x0ABE5 }, { 0x0ABE8, 0x0ABE8 }, { 0x0ABED, 0x0ABED },
			{ 0x0D7B0, 0x0D7FF }, { 0x0FB1E, 0x0FB1E }, { 0x0FE00, 0x0FE0F }, { 0x0FE20, 0x0FE2F },
			{ 0x0FEFF, 0x0FEFF }, { 0x0FFF9, 0x0FFFB }, { 0x101FD, 0x101FD }, { 0x102E0, 0x102E0 },
			{ 0x10376, 0x1037A }, { 0x10A01, 0x10A0F }, { 0x10A38, 0x10A3F }, { 0x10AE5, 0x10AE6 },
			{ 0x10D24, 0x10D27 }, { 0x10D69, 0x10D6D }, { 0x10EAB, 0x10EAC }, { 0x10EFC, 0x10EFF },
			{ 0x10F46, 0x10F50 }, { 0x10F82, 0x10F85 }, { 0x11001, 0x11001 }, { 0x11038, 0x11046 },
			{ 0x11070, 0x11070 }, { 0x11073, 0x11074 }, { 0x1107F, 0x11081 }, { 0x110B3, 0x110B6 },
			{ 0x110B9, 0x110BA }, { 0x110C2, 0x110C2 }, { 0x11100, 0x11102 }, { 0x11127, 0x1112B },
			{ 0x1112D, 0x11134 }, { 0x11173, 0x11173 }, { 0x11180, 0x11181 }, { 0x111B6, 0x111BE },
			{ 0x111C9, 0x111CC }, { 0x111CF, 0x111CF }, { 0x1122F, 0x11231 }, { 0x11234, 0x11234 },
			{ 0x11236, 0x11237 }, { 0x1123E, 0x1123E }, { 0x11241, 0x11241 }, { 0x112DF, 0x112DF },
			{ 0x112E3, 0x112EA }, { 0x11300, 0x11301 }, { 0x1133B, 0x1133C }, { 0x11340, 0x11340 },
			{ 0x11366, 0x11374 }, { 0x113BB, 0x113C0 }, { 0x113CE, 0x113CE }, { 0x113D0, 0x113D0 },
			{ 0x113D2, 0x113D2 }, { 0x113E1, 0x113E2 }, { 0x11438, 0x1143F }, { 0x11442, 0x11444 },
			{ 0x11446, 0x11446 }, { 0x1145E, 0x1145E }, { 0x114B3, 0x114B8 }, { 0x114BA, 0x114BA },
			{ 0x114BF, 0x114C0 }, { 0x114C2, 0x114C3 }, { 0x115B2, 0x115B5 }, { 0x115BC, 0x115BD },
			{ 0x115BF, 0x115C0 }, { 0x115DC, 0x115DD }, { 0x11633, 0x1163A }, { 0x1163D, 0x1163D },
			{ 0x1163F, 0x11640 }, { 0x116AB, 0x116AB }, { 0x116AD, 0x116AD }, { 0x116B0, 0x116B5 },
			{ 0x116B7, 0x116B7 }, { 0x1171D, 0x1171D }, { 0x1171F, 0x1171F }, { 0x11722, 0x11725 },
			{ 0x11727, 0x1172B }, { 0x1182F, 0x11837 }, { 0x11839, 0x1183A }, { 0x1193B, 0x1193C },
			{ 0x1193E, 0x1193E }, { 0x11943, 0x11943 }, { 0x119D4, 0x119DB }, { 0x119E0, 0x119E0 },
			{ 0x11A01, 0x11A0A }, { 0x11A33, 0x11A38 }, { 0x11A3B, 0x11A3E }, { 0x11A47, 0x11A47 },
			{ 0x11A51, 0x11A56 }, { 0x11A59, 0x11A5B }, { 0x11A8A, 0x11A96 }, { 0x11A98, 0x11A99 },
			{ 0x11C30, 0x11C3D }, { 0x11C3F, 0x11C3F }, { 0x11C92, 0x11CA7 }, { 0x11CAA, 0x11CB0 },
			{ 0x11CB2, 0x11CB3 }, { 0x11CB5, 0x11CB6 }, { 0x11D31, 0x11D45 }, { 0x11D47, 0x11D47 },
			{ 0x11D90, 0x11D91 }, { 0x11D95, 0x11D95 }, { 0x11D97, 0x11D97 }, { 0x11EF3, 0x11EF4 },
			{ 0x11F00, 0x11F01 }, { 0x11F36, 0x11F3A }, { 0x11F40, 0x11F40 }, { 0x11F42, 0x11F42 },
			{ 0x11F5A, 0x11F5A }, { 0x13430, 0x13440 }, { 0x13447, 0x13455 }, { 0x1611E, 0x16129 },
			{ 0x1612D, 0x1612F }, { 0x16AF0, 0x16AF4 }, { 0x16B30, 0x16B36 }, { 0x16F4F, 0x16F4F },
			{ 0x16F8F, 0x16F92 }, { 0x16FE4, 0x16FE4 }, { 0x1BC9D, 0x1BC9E }, { 0x1BCA0, 0x1BCA3 },
			{ 0x1CF00, 0x1CF46 }, { 0x1D167, 0x1D169 }, { 0x1D173, 0x1D182 }, { 0x1D185, 0x1D18B },
			{ 0x1D1AA, 0x1D1AD }, { 0x1D242, 0x1D244 }, { 0x1DA00, 0x1DA36 }, { 0x1DA3B, 0x1DA6C },
			{ 0x1DA75, 0x1DA75 }, { 0x1DA84, 0x1DA84 }, { 0x1DA9B, 0x1DAAF }, { 0x1E000, 0x1E02A },
			{ 0x1E08F, 0x1E08F }, { 0x1E130, 0x1E136 }, { 0x1E2AE, 0x1E2AE }, { 0x1E2EC, 0x1E2EF },
			{ 0x1E4EC, 0x1E4EF }, { 0x1E5EE, 0x1E5EF }, { 0x1E8D0, 0x1E8D6 }, { 0x1E944, 0x1E94A },
			{ 0xE0001, 0xE01EF }
		};
		// clang-format on

		template <std::size_t N>
		[[nodiscard]] constexpr bool in_ranges( const char32_t codepoint, const codepoint_range ( &ranges )[N] ) noexcept
		{
			const auto* const range = std::ranges::lower_bound( ranges, codepoint, {}, &codepoint_range::last );
			return range != std::ranges::end( ranges ) && range->first <= codepoint;
		}

		// Decodes the UTF-8 sequence at `pos` and advances `pos` past it
		[[nodiscard]] constexpr char32_t next_codepoint( const std::string_view text, std::size_t& pos ) noexcept
		{
			constexpr char32_t replacement = 0xFFFD;

			const auto lead = static_cast<unsigned char>( text[pos] );

			std::size_t continuations = 0;
			char32_t    codepoint     = 0;

			if ( lead < 0x80 )
			{
				++pos;
				return lead;
			}

			if ( ( lead & 0xE0 ) == 0xC0 )
			{
				continuations = 1;
				codepoint     = lead & 0x1FU;
			}
			else if ( ( lead & 0xF0 ) == 0xE0 )
			{
				continuations = 2;
				codepoint     = lead & 0x0FU;
			}
			else if ( ( lead & 0xF8 ) == 0xF0 )
			{
				continuations = 3;
				codepoint     = lead & 0x07U;
			}
			else
			{
				++pos;
				return replacement;
			}

			if ( pos + continuations >= text.size() )
			{
				++pos;
				return replacement;
			}

			for ( std::size_t i = 1; i <= continuations; ++i )
			{
				const auto continuation = static_cast<unsigned char>( text[pos + i] );

				if ( ( continuation & 0xC0 ) != 0x80 )
				{
					++pos;
					return replacement;
				}

				codepoint = ( codepoint << 6 ) | ( continuation & 0x3FU );
			}

			pos += continuations + 1;

			return codepoint;
		}

		// Skips the ECMA-48 escape sequence whose ESC was just read
		constexpr void skip_escape( const std::string_view text, std::size_t& pos ) noexcept
		{
			const auto byte = [text]( const std::size_t i ) noexcept {
				return static_cast<unsigned char>( text[i] );
			};

			if ( pos >= text.size() )
			{
				return;
			}

			const char kind = text[pos];

			// CSI: parameters and intermediates, then a final byte
			if ( kind == '[' )
			{
				++pos;

				while ( pos < text.size() && ( byte( pos ) < 0x40 || byte( pos ) > 0x7E ) )
				{
					++pos;
				}

				pos = std::min( pos + 1, text.size() );
				return;
			}

			// OSC (hyperlinks, titles), DCS, SOS, PM and APC: a string ended by BEL or ESC backslash
			if ( kind == ']' || kind == 'P' || kind == 'X' || kind == '^' || kind == '_' )
			{
				++pos;

				while ( pos < text.size() && text[pos] != '\a' )
				{
					if ( text[pos] == '\x1b' && pos + 1 < text.size() && text[pos + 1] == '\\' )
					{
						++pos;
						break;
					}

					++pos;
				}

				pos = std::min( pos + 1, text.size() );
				return;
			}

			// Anything else: intermediates such as the ( of ESC ( B, then a final byte
			while ( pos < text.size() && byte( pos ) >= 0x20 && byte( pos ) <= 0x2F )
			{
				++pos;
			}

			if ( pos < text.size() && byte( pos ) >= 0x30 && byte( pos ) <= 0x7E )
			{
				++pos;
			}
		}
	} // namespace detail

	// Number of terminal columns `text` occupies
	[[nodiscard]] constexpr std::size_t display_width( const std::string_view text ) noexcept
	{
		std::size_t width  = 0;
		std::size_t pos    = 0;
		std::size_t last   = 0; // columns of the last visible code point
		bool        joined = false;

		while ( pos < text.size() )
		{
			const char32_t codepoint = detail::next_codepoint( text, pos );

			if ( codepoint == 0x1B )
			{
				detail::skip_escape( text, pos );
				continue;
			}

			// An emoji ZWJ sequence (a family, a profession) is drawn as one emoji
			if ( codepoint == 0x200D )
			{
				joined = last == 2;
				continue;
			}

			// VS16 asks for the emoji presentation, two columns wide (a red heart)
			if ( codepoint == 0xFE0F )
			{
				if ( last == 1 )
				{
					++width;
					last = 2;
				}

				continue;
			}

			if ( codepoint < 0x20 || codepoint == 0x7F || detail::in_ranges( codepoint, detail::zero_width_ranges ) )
			{
				continue;
			}

			// Joined by a ZWJ, or a skin tone modifying the emoji before it
			if ( joined || ( codepoint >= 0x1F3FB && codepoint <= 0x1F3FF && last == 2 ) )
			{
				joined = false;
				continue;
			}

			last = detail::in_ranges( codepoint, detail::wide_ranges ) ? 2 : 1;
			width += last;
		}

		return width;
	}

	// A border: one character, or a UTF-8 string such as a box-drawing line
	struct border
	{
		std::array<char, 16> bytes{};
		std::size_t          size = 0;

		constexpr border( const char character ) noexcept
			:
			bytes{ character },
			size{ 1 }
		{
		}

		template <std::size_t N>
		constexpr border( const char ( &text )[N] ) noexcept
			:
			size{ N - 1 }
		{
			static_assert( N - 1 <= std::tuple_size_v<decltype( bytes )>, "a border is at most 16 bytes" );
			std::ranges::copy_n( std::begin( text ), N - 1, bytes.begin() );
		}

		[[nodiscard]] constexpr std::string_view view() const noexcept
		{
			return { bytes.data(), size };
		}
	};

	// Width of the terminal `stream` writes to
	[[nodiscard]] inline int terminal_width( std::FILE* const stream ) noexcept
	{
#ifdef _WIN32
		const int fd = stream != nullptr ? _fileno( stream ) : -1;
		if ( fd >= 0 )
		{
			const auto handle = reinterpret_cast<HANDLE>( _get_osfhandle( fd ) );
			if ( handle != nullptr && handle != INVALID_HANDLE_VALUE )
			{
				CONSOLE_SCREEN_BUFFER_INFO csbi{};

				if ( GetConsoleScreenBufferInfo( handle, &csbi ) )
				{
					const int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

					if ( columns > 0 )
					{
						return columns;
					}
				}
			}
		}
#elif defined( __unix__ ) || defined( __APPLE__ )
		const int fd = stream != nullptr ? fileno( stream ) : -1;
		if ( fd >= 0 && isatty( fd ) )
		{
			winsize ws{};

			if ( ioctl( fd, TIOCGWINSZ, &ws ) == 0 && ws.ws_col > 0 ) // NOLINT(cppcoreguidelines-pro-type-vararg): C API
			{
				return ws.ws_col;
			}
		}

		// Not a terminal: `COLUMNS=120 program | less`
		if ( const char* const columns = std::getenv( "COLUMNS" ) )
		{
			const std::string_view value( columns );
			int                    parsed = 0;
			const auto [end, error]       = std::from_chars( value.data(), value.data() + value.size(), parsed );

			if ( error == std::errc{} && end == value.data() + value.size() && parsed > 0 )
			{
				return parsed;
			}
		}
#endif

		return default_width;
	}

	// Width of the terminal stdout writes to
	[[nodiscard]] inline int terminal_width() noexcept
	{
		return terminal_width( stdout );
	}

	template <border left_border = '=', border right_border = left_border>
	[[nodiscard]] constexpr std::string make_header_line( const std::string_view title, int width = terminal_width() )
	{
		if ( width <= 0 )
		{
			width = default_width;
		}

		const std::size_t content = display_width( title ) + 2;

		if ( static_cast<std::size_t>( width ) <= content )
		{
			return std::string( title );
		}

		// Whole borders, then spaces next to the title for the columns a wide border cannot fill
		const auto split = []( const std::size_t columns, const std::string_view pattern ) noexcept {
			const std::size_t unit = display_width( pattern );
			return unit == 0 ? std::pair{ std::size_t{ 0 }, columns } : std::pair{ columns / unit, columns % unit };
		};

		const std::size_t fill        = static_cast<std::size_t>( width ) - content;
		const auto [left, left_pad]   = split( fill / 2, left_border.view() );
		const auto [right, right_pad] = split( fill - ( fill / 2 ), right_border.view() );

		std::string out;
		out.reserve( title.size() + ( left * left_border.size ) + ( right * right_border.size ) + left_pad + right_pad + 2 );

		for ( std::size_t i = 0; i < left; ++i )
		{
			out.append( left_border.view() );
		}

		out.append( left_pad + 1, ' ' );
		out.append( title );
		out.append( right_pad + 1, ' ' );

		for ( std::size_t i = 0; i < right; ++i )
		{
			out.append( right_border.view() );
		}

		return out;
	}

	// Prints a centred header to `stream`
	template <border left_border = '=', border right_border = left_border>
	void print( std::FILE* const stream, const std::string_view title, const int width )
	{
		std::println( stream, "{}", make_header_line<left_border, right_border>( title, width ) );
	}

	// Prints a centred header to `stream`, as wide as its terminal
	template <border left_border = '=', border right_border = left_border>
	void print( std::FILE* const stream, const std::string_view title )
	{
		print<left_border, right_border>( stream, title, terminal_width( stream ) );
	}

	// Prints a centred header
	template <border left_border = '=', border right_border = left_border>
	void print( const std::string_view title, const int width = terminal_width() )
	{
		print<left_border, right_border>( stdout, title, width );
	}

	// Prints a formatted centred header to `stream` with a specified width
	template <border left_border = '=', border right_border = left_border, class... Args>
	void printfw( std::FILE* const stream, const int width, std::format_string<Args...> fmt, Args&&... args )
	{
		print<left_border, right_border>( stream, std::format( fmt, std::forward<Args>( args )... ), width );
	}

	// Prints a formatted centred header with a specified width
	template <border left_border = '=', border right_border = left_border, class... Args>
	void printfw( const int width, std::format_string<Args...> fmt, Args&&... args )
	{
		printfw<left_border, right_border>( stdout, width, fmt, std::forward<Args>( args )... );
	}

	// Prints a formatted centred header to `stream`
	template <border left_border = '=', border right_border = left_border, class... Args>
	void printf( std::FILE* const stream, std::format_string<Args...> fmt, Args&&... args )
	{
		printfw<left_border, right_border>( stream, terminal_width( stream ), fmt, std::forward<Args>( args )... );
	}

	// Prints a formatted centred header
	template <border left_border = '=', border right_border = left_border, class... Args>
	void printf( std::format_string<Args...> fmt, Args&&... args )
	{
		printf<left_border, right_border>( stdout, fmt, std::forward<Args>( args )... );
	}
} // namespace hp

#endif //! HPRINT_HPP_
