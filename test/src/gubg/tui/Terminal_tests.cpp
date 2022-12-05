#include <gubg/tui/Terminal.hpp>

#include <catch.hpp>
#include <iostream>

using namespace gubg;

TEST_CASE("", "[ut][tui][Terminal]")
{
    tui::Terminal t;

    tui::Terminal::CharSize char_size;
    REQUIRE(t.get(char_size));
    std::cout << char_size << std::endl;

    t.clear();

    for (auto i = 0u; i < 10; ++i)
        t.print("kaka", {i, 10}, tui::Style::attention(i), 3);

    if (true)
    {
        std::optional<char> ch;
        for (auto i = 0; i < 10; ++i)
        {
            REQUIRE(t.read(ch));
            if (ch)
                std::cout << std::string(10, *ch) << std::endl;
        }
    }
}
