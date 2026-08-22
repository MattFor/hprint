#include <hprint>

#include <cstdio>
#include <string>
#include <string_view>

namespace
{
    int failures = 0;

    void check(const bool condition, const std::string_view expression, const int line)
    {
        if (!condition)
        {
            ++failures;
            std::println(stderr, "FAIL {}: {}", line, expression);
        }
    }

    void check_eq(const std::string_view actual, const std::string_view expected, const int line)
    {
        if (actual != expected)
        {
            ++failures;
            std::println(stderr, R"(FAIL {}: expected "{}", got "{}")", line, expected, actual);
        }
    }
} // namespace

#define CHECK(expr)             check((expr), #expr, __LINE__)
#define CHECK_EQ(lhs, rhs)      check_eq((lhs), (rhs), __LINE__)

static_assert(hp::display_width("") == 0);
static_assert(hp::display_width("ascii") == 5);
static_assert(hp::display_width("ü") == 1);
static_assert(hp::display_width("日") == 2);
static_assert(hp::display_width("é") == 1);
static_assert(hp::display_width("\x1b[31mRed\x1b[0m") == 3);

int main()
{
    // Rendered line occupies exactly the requested width.
    for (const std::string_view title : {
             "ASCII",
             "Ünïcödé",
             "日本語",
             "a"
         })
    {
        for (const int width : {
                 20,
                 40,
                 79,
                 80,
                 120
             })
        {
            CHECK(hp::display_width(hp::make_header_line(title, width)) == static_cast<std::size_t>(width));
        }
    }

    // Border characters and symmetry.
    CHECK_EQ(hp::make_header_line<'='>("hi", 10), "=== hi ===");
    CHECK_EQ((hp::make_header_line<'[', ']'>("hi", 10)), "[[[ hi ]]]");
    CHECK_EQ(hp::make_header_line<'>'>("ab", 11), ">>> ab >>>>");

    // Titles that do not fit are returned unchanged.
    CHECK_EQ(hp::make_header_line("toolongtitle", 5), "toolongtitle");
    CHECK_EQ(hp::make_header_line("exact", 7), "exact"); // width == title + 2 spaces
    CHECK_EQ(hp::make_header_line("fits", 7), " fits =");

    // Non-positive widths fall back to the default.
    CHECK(hp::display_width(hp::make_header_line("x", 0)) == static_cast<std::size_t>(hp::default_width));
    CHECK(hp::display_width(hp::make_header_line("x", -42)) == static_cast<std::size_t>(hp::default_width));

    // Malformed UTF-8 terminates and never over-counts.
    CHECK(hp::display_width("\xff\xfe") == 2);
    CHECK(hp::display_width("\xe6\x97") == 2);
    CHECK(hp::display_width("\x80") == 1);

    // Empty title still produces a full-width rule.
    CHECK(hp::display_width(hp::make_header_line("", 20)) == 20);

    CHECK(hp::terminal_width() > 0);

    if (failures == 0)
    {
        std::println("All tests passed");
    }

    return failures == 0 ? 0 : 1;
}
