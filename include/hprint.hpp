#ifndef HPRINT_HPP_
#define HPRINT_HPP_

#pragma once

#define HPRINT_VERSION_MAJOR 1
#define HPRINT_VERSION_MINOR 2
#define HPRINT_VERSION_PATCH 0
#define HPRINT_VERSION_STRING "1.2.0"

#include <print>
#include <format>
#include <string>
#include <cstddef>
#include <utility>
#include <algorithm>
#include <string_view>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#define HPRINT_UNDEF_WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#define HPRINT_UNDEF_NOMINMAX
#endif

#include <windows.h>

#ifdef HPRINT_UNDEF_NOMINMAX
#undef NOMINMAX
#undef HPRINT_UNDEF_NOMINMAX
#endif

#ifdef HPRINT_UNDEF_WIN32_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#undef HPRINT_UNDEF_WIN32_LEAN_AND_MEAN
#endif
#elif defined(__unix__) || defined(__APPLE__)
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

        inline constexpr codepoint_range wide_ranges[] = {
            {
                .first = 0x01100,
                .last = 0x0115F
            },
            {
                .first = 0x02E80,
                .last = 0x0303E
            },
            {
                .first = 0x03041,
                .last = 0x033FF
            },
            {
                .first = 0x03400,
                .last = 0x04DBF
            },
            {
                .first = 0x04E00,
                .last = 0x09FFF
            },
            {
                .first = 0x0A000,
                .last = 0x0A4CF
            },
            {
                .first = 0x0A960,
                .last = 0x0A97F
            },
            {
                .first = 0x0AC00,
                .last = 0x0D7A3
            },
            {
                .first = 0x0F900,
                .last = 0x0FAFF
            },
            {
                .first = 0x0FE10,
                .last = 0x0FE19
            },
            {
                .first = 0x0FE30,
                .last = 0x0FE6F
            },
            {
                .first = 0x0FF00,
                .last = 0x0FF60
            },
            {
                .first = 0x0FFE0,
                .last = 0x0FFE6
            },
            {
                .first = 0x1F300,
                .last = 0x1F64F
            },
            {
                .first = 0x1F680,
                .last = 0x1F6FF
            },
            {
                .first = 0x1F900,
                .last = 0x1F9FF
            },
            {
                .first = 0x20000,
                .last = 0x3FFFD
            }
        };

        inline constexpr codepoint_range zero_width_ranges[] = {
            {
                .first = 0x00300,
                .last = 0x0036F
            },
            {
                .first = 0x00483,
                .last = 0x00489
            },
            {
                .first = 0x00591,
                .last = 0x005BD
            },
            {
                .first = 0x00610,
                .last = 0x0061A
            },
            {
                .first = 0x0064B,
                .last = 0x0065F
            },
            {
                .first = 0x006D6,
                .last = 0x006DC
            },
            {
                .first = 0x00E31,
                .last = 0x00E31
            },
            {
                .first = 0x00E34,
                .last = 0x00E3A
            },
            {
                .first = 0x00EB1,
                .last = 0x00EB1
            },
            {
                .first = 0x00EB4,
                .last = 0x00EBC
            },
            {
                .first = 0x01AB0,
                .last = 0x01AFF
            },
            {
                .first = 0x01DC0,
                .last = 0x01DFF
            },
            {
                .first = 0x0200B,
                .last = 0x0200F
            },
            {
                .first = 0x020D0,
                .last = 0x020F0
            },
            {
                .first = 0x0FE00,
                .last = 0x0FE0F
            },
            {
                .first = 0x0FE20,
                .last = 0x0FE2F
            },
            {
                .first = 0x0FEFF,
                .last = 0x0FEFF
            }
        };

        template <std::size_t N>
        [[nodiscard]] constexpr bool in_ranges(const char32_t codepoint, const codepoint_range (&ranges)[N]) noexcept
        {
            return std::ranges::any_of(ranges, [codepoint](const auto& range) noexcept
            {
                return codepoint >= range.first && codepoint <= range.last;
            });
        }

        // Decodes the UTF-8 sequence at `pos` and advances `pos` past it
        [[nodiscard]] constexpr char32_t next_codepoint(const std::string_view text, std::size_t& pos) noexcept
        {
            constexpr char32_t replacement = 0xFFFD;

            const auto lead = static_cast<unsigned char>(text[pos]);

            std::size_t continuations = 0;
            char32_t    codepoint     = 0;

            if (lead < 0x80)
            {
                ++pos;
                return lead;
            }

            if (( lead & 0xE0 ) == 0xC0)
            {
                continuations = 1;
                codepoint     = lead & 0x1FU;
            }
            else if (( lead & 0xF0 ) == 0xE0)
            {
                continuations = 2;
                codepoint     = lead & 0x0FU;
            }
            else if (( lead & 0xF8 ) == 0xF0)
            {
                continuations = 3;
                codepoint     = lead & 0x07U;
            }
            else
            {
                ++pos;
                return replacement;
            }

            if (pos + continuations >= text.size())
            {
                ++pos;
                return replacement;
            }

            for (std::size_t i = 1; i <= continuations; ++i)
            {
                const auto continuation = static_cast<unsigned char>(text[pos + i]);

                if (( continuation & 0xC0 ) != 0x80)
                {
                    ++pos;
                    return replacement;
                }

                codepoint = ( codepoint << 6 ) | ( continuation & 0x3FU );
            }

            pos += continuations + 1;

            return codepoint;
        }
    } // namespace detail

    // Number of terminal columns `text` occupies
    [[nodiscard]] constexpr std::size_t display_width(const std::string_view text) noexcept
    {
        std::size_t width = 0;
        std::size_t pos   = 0;

        while (pos < text.size())
        {
            const char32_t codepoint = detail::next_codepoint(text, pos);

            // ESC '[' ... final byte
            if (codepoint == 0x1B && pos < text.size() && text[pos] == '[')
            {
                ++pos;

                while (pos < text.size() && ( static_cast<unsigned char>(text[pos]) < 0x40 || static_cast<unsigned char>(text[pos]) > 0x7E ))
                {
                    ++pos;
                }

                if (pos < text.size())
                {
                    ++pos;
                }

                continue;
            }

            if (codepoint < 0x20 || codepoint == 0x7F || detail::in_ranges(codepoint, detail::zero_width_ranges))
            {
                continue;
            }

            width += detail::in_ranges(codepoint, detail::wide_ranges)
                         ? std::size_t{
                             2
                         }
                         : std::size_t{
                             1
                         };
        }

        return width;
    }

    // Width of the attached terminal
    [[nodiscard]] inline int terminal_width() noexcept
    {
#if defined(_WIN32)
        const HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE); if (handle != nullptr && handle != INVALID_HANDLE_VALUE)
        {
            CONSOLE_SCREEN_BUFFER_INFO csbi{};

            if (GetConsoleScreenBufferInfo(handle, &csbi))
            {
                const int columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;

                if (columns > 0)
                {
                    return columns;
                }
            }
        }
#elif defined(__unix__) || defined(__APPLE__)
        if (isatty(STDOUT_FILENO))
        {
            winsize ws{};

            if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
            {
                return ws.ws_col;
            }
        }
#endif

        return default_width;
    }

    template <char left_border = '=', char right_border = left_border>
    [[nodiscard]] std::string make_header_line(const std::string_view title, int width = terminal_width())
    {
        if (width <= 0)
        {
            width = default_width;
        }

        const std::size_t content = display_width(title) + 2;

        if (static_cast<std::size_t>(width) <= content)
        {
            return std::string(title);
        }

        const std::size_t fill  = static_cast<std::size_t>(width) - content;
        const std::size_t left  = fill / 2;
        const std::size_t right = fill - left;

        std::string out;
        out.reserve(title.size() + left + right + 2);
        out.append(left, left_border);
        out.push_back(' ');
        out.append(title);
        out.push_back(' ');
        out.append(right, right_border);

        return out;
    }

    // Prints a centred header
    template <char left_border = '=', char right_border = left_border>
    void print(const std::string_view title, const int width = terminal_width())
    {
        std::println("{}", make_header_line<left_border, right_border>(title, width));
    }

    // Prints a formatted centred header
    template <char left_border = '=', char right_border = left_border, class... Args>
    void printf(std::format_string<Args...> fmt, Args&&... args)
    {
        std::println("{}", make_header_line<left_border, right_border>(std::format(fmt, std::forward<Args>(args)...)));
    }

    // Prints a formatted centred header with a specified with
    template <char left_border = '=', char right_border = left_border, class... Args>
    void printfw(const int width, std::format_string<Args...> fmt, Args&&... args)
    {
        std::println("{}", make_header_line<left_border, right_border>(std::format(fmt, std::forward<Args>(args)...), width));
    }
} // namespace hp

#endif //!HPRINT_HPP_
