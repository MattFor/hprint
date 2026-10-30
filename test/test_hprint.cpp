#include <hprint>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <string_view>

namespace
{
	int failures = 0;

	void check( const bool condition, const std::string_view expression, const int line )
	{
		if ( !condition )
		{
			++failures;
			std::println( stderr, "FAIL {}: {}", line, expression );
		}
	}

	void check_eq( const std::string_view actual, const std::string_view expected, const int line )
	{
		if ( actual != expected )
		{
			++failures;
			std::println( stderr, R"(FAIL {}: expected "{}", got "{}")", line, expected, actual );
		}
	}

	std::string read_all( std::FILE* const file )
	{
		std::string text;
		if ( std::fseek( file, 0, SEEK_SET ) != 0 )
		{
			return text;
		}

		for ( int c = std::fgetc( file ); c != EOF; c = std::fgetc( file ) )
		{
			text.push_back( static_cast<char>( c ) );
		}

		return text;
	}
} // namespace

#define CHECK( expr ) check( ( expr ), #expr, __LINE__ )
#define CHECK_EQ( lhs, rhs ) check_eq( ( lhs ), ( rhs ), __LINE__ ) // NOLINT(cppcoreguidelines-macro-usage): needs the caller's __LINE__

static_assert( std::string_view( HPRINT_VERSION_STRING ) == HPRINT_EXPECTED_VERSION, "hprint.hpp and CMakeLists.txt disagree" );
static_assert( hp::display_width( "" ) == 0 );
static_assert( hp::display_width( "ascii" ) == 5 );
static_assert( hp::display_width( "ü" ) == 1 );
static_assert( hp::display_width( "日" ) == 2 );
static_assert( hp::display_width( "é" ) == 1 );
static_assert( hp::display_width( "\x1b[31mRed\x1b[0m" ) == 3 );

// Wide and zero-width code points from the Unicode tables
static_assert( hp::display_width( "⌚☕🀄🈁🫠" ) == 10 );
static_assert( hp::display_width( "e\u0301" ) == 1 );
static_assert( hp::display_width( "\u0915\u094D" ) == 1 );
static_assert( hp::display_width( "a\u200Bb" ) == 2 );
static_assert( hp::display_width( "\u1100\u1161" ) == 2 );
static_assert( hp::display_width( "x\U000E0100" ) == 1 );

// Emoji sequences are drawn as one emoji
static_assert( hp::display_width( "👨‍👩‍👧" ) == 2 );
static_assert( hp::display_width( "❤️‍🔥" ) == 2 );
static_assert( hp::display_width( "🏳️‍🌈" ) == 2 );
static_assert( hp::display_width( "🏴󠁧󠁢󠁳󠁣󠁴󠁿" ) == 2 );
static_assert( hp::display_width( "👍🏽" ) == 2 );
static_assert( hp::display_width( "🏽" ) == 2 );
static_assert( hp::display_width( "❤️" ) == 2 );
static_assert( hp::display_width( "❤" ) == 1 );
static_assert( hp::display_width( "1️⃣" ) == 2 );
static_assert( hp::display_width( "🇵🇱" ) == 2 );
static_assert( hp::display_width( "a\u200Db" ) == 2 );

// Escape sequences other than CSI take no columns
static_assert( hp::display_width( "\x1b]8;;https://example.com\x1b\\link\x1b]8;;\x1b\\" ) == 4 );
static_assert( hp::display_width( "\x1b]0;title\atext" ) == 4 );
static_assert( hp::display_width( "\x1b(B\x1b[m" ) == 0 );
static_assert( hp::display_width( "\x1b"
                                  "7x\x1b"
                                  "8" ) == 1 );
static_assert( hp::display_width( "\x1bPq#0\x1b\\a" ) == 1 );
static_assert( hp::display_width( "\x1b]8;;unterminated" ) == 0 );
static_assert( hp::display_width( "a\x1b" ) == 1 );

// Header lines can be built at compile time, with UTF-8 borders
static_assert( hp::make_header_line( "hi", 10 ) == "=== hi ===" );
static_assert( hp::make_header_line<'[', ']'>( "hi", 10 ) == "[[[ hi ]]]" );
static_assert( hp::make_header_line<"─">( "hi", 10 ) == "─── hi ───" );
static_assert( hp::make_header_line<"«", "»">( "hi", 10 ) == "««« hi »»»" );
static_assert( hp::make_header_line<"＝">( "hi", 11 ) == "＝  hi ＝＝" );
static_assert( hp::make_header_line<"-=">( "x", 11 ) == "-=-= x -=-=" );
static_assert( hp::make_header_line<"">( "x", 7 ) == "   x   " );

int main()
{
	// Rendered line occupies exactly the requested width.
	for ( const std::string_view title : {
				  "ASCII",
				  "Ünïcödé",
				  "日本語",
				  "a",
				  "🚀 Deploy 👨‍👩‍👧" } )
	{
		for ( const int width : {
					  20,
					  40,
					  79,
					  80,
					  120 } )
		{
			const auto expected = static_cast<std::size_t>( width );
			CHECK( hp::display_width( hp::make_header_line( title, width ) ) == expected );
			CHECK( hp::display_width( hp::make_header_line<"─">( title, width ) ) == expected );
			CHECK( hp::display_width( hp::make_header_line<"＝", "-=">( title, width ) ) == expected );
		}
	}

	// Border characters and symmetry.
	CHECK_EQ( hp::make_header_line<'='>( "hi", 10 ), "=== hi ===" );
	CHECK_EQ( ( hp::make_header_line<'[', ']'>( "hi", 10 ) ), "[[[ hi ]]]" );
	CHECK_EQ( hp::make_header_line<'>'>( "ab", 11 ), ">>> ab >>>>" );

	// Titles that do not fit are returned unchanged.
	CHECK_EQ( hp::make_header_line( "toolongtitle", 5 ), "toolongtitle" );
	CHECK_EQ( hp::make_header_line( "exact", 7 ), "exact" ); // width == title + 2 spaces
	CHECK_EQ( hp::make_header_line( "fits", 7 ), " fits =" );

	// Non-positive widths fall back to the default.
	CHECK( hp::display_width( hp::make_header_line( "x", 0 ) ) == static_cast<std::size_t>( hp::default_width ) );
	CHECK( hp::display_width( hp::make_header_line( "x", -42 ) ) == static_cast<std::size_t>( hp::default_width ) );

	// Malformed UTF-8 terminates and never over-counts.
	CHECK( hp::display_width( "\xff\xfe" ) == 2 );
	CHECK( hp::display_width( "\xe6\x97" ) == 2 );
	CHECK( hp::display_width( "\x80" ) == 1 );

	// Empty title still produces a full-width rule.
	CHECK( hp::display_width( hp::make_header_line( "", 20 ) ) == 20 );

	CHECK( hp::terminal_width() > 0 );
	CHECK( hp::terminal_width( nullptr ) == hp::default_width );

#ifndef _WIN32
	// A stream that is not a terminal is COLUMNS wide, when that is a positive number
	unsetenv( "COLUMNS" );
#endif

	// Printing to a stream other than stdout
	std::FILE* const file = std::tmpfile();
	CHECK( file != nullptr );

	if ( file != nullptr )
	{
		CHECK( hp::terminal_width( file ) == hp::default_width );

		hp::print( file, "hi", 10 );
		hp::print<"─">( file, "hi", 10 );
		hp::printfw<'['>( file, 12, "n={}", 4 );
		hp::printf<'#'>( file, "{}", "x" );
		hp::print( file, "y" );

		CHECK_EQ( read_all( file ), "=== hi ===\n─── hi ───\n[[[ n=4 [[[[\n" + hp::make_header_line<'#'>( "x", hp::default_width ) + "\n" + hp::make_header_line( "y", hp::default_width ) + "\n" );

#ifndef _WIN32
		for ( const auto& [value, expected] : {
					  std::pair{ "123", 123 },
					  std::pair{ "abc", hp::default_width },
					  std::pair{ "0", hp::default_width },
					  std::pair{ "-5", hp::default_width },
					  std::pair{ "12x", hp::default_width } } )
		{
			setenv( "COLUMNS", value, 1 );
			CHECK( hp::terminal_width( file ) == expected );
		}

		unsetenv( "COLUMNS" );
#endif

		static_cast<void>( std::fclose( file ) );
	}

	if ( failures == 0 )
	{
		std::println( "All tests passed" );
	}

	return failures == 0 ? 0 : 1;
}
