#include <hprint>

int main()
{
    hp::print<'>'>("Build started");
    hp::printf<'#'>("Phase {}", 1);
    hp::printf<'[', ']'>("User: {}", "mattfor");
    hp::print("Custom Width Test", 120);

    hp::print<'~'>("Ünïcödé", 60);
    hp::print<'*'>("日本語のヘッダー", 60);

    hp::printfw<'-'>(60, "Done in {:.2f}s", 1.5);
}
