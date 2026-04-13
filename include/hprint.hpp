#ifndef HPRINT_HPP_
#define HPRINT_HPP_

#pragma once

#include <print>

#if defined(_WIN32)
#include <windows.h>
#elif defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <sys/ioctl.h>
#endif

namespace hp
{
    [[nodiscard]] inline int terminal_width() noexcept
    {
#if defined(_WIN32)
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); if (h != INVALID_HANDLE_VALUE)
        {
            CONSOLE_SCREEN_BUFFER_INFO csbi{};

            if (GetConsoleScreenBufferInfo(h, &csbi))
            {
                return csbi.srWindow.Right - csbi.srWindow.Left + 1;
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

        return 80;
    }

    template <char left_border = '=', char right_border = left_border>
    [[nodiscard]] std::string make_header_line(const std::string_view title, int width = terminal_width())
    {
        if (width <= 0)
        {
            width = 80;
        }

        const int content = static_cast<int>(title.size()) + 2;
        if (width <= content)
        {
            return std::string(title);
        }

        const int fill  = width - content;
        const int left  = fill / 2;
        const int right = fill - left;

        std::string out;
        out.reserve(static_cast<std::size_t>(width));
        out.append(static_cast<std::size_t>(left), left_border);
        out.push_back(' ');
        out.append(title);
        out.push_back(' ');
        out.append(static_cast<std::size_t>(right), right_border);

        return out;
    }

    template <char left_border = '=', char right_border = left_border>
    void print(const std::string_view title, const int width = terminal_width())
    {
        std::print("{}\n", make_header_line<left_border, right_border>(title, width));
    }

    template <char left_border = '=', char right_border = left_border, class... Args>
    void printf(std::format_string<Args...> fmt, Args&&... args)
    {
        std::print("{}\n", make_header_line<left_border, right_border>(std::format(fmt, std::forward<Args>(args)...)));
    }
} // namespace hp

#endif //!HPRINT_HPP_