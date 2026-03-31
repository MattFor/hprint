#include <hprint/hprint.hpp>

int main()
{
    hp::print<'>'>("Build started");
    hp::printf<'#'>("Phase {}", 1);
    hp::printf<'[', ']'>("User: {}", "mattfor");
    hp::print("Custom Width Test", 120);
}
