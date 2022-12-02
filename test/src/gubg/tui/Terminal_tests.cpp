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

    if (false)
    {
        char ch;
        for (auto i = 0; i < 10; ++i)
        {
            REQUIRE(t.read(ch));
            std::cout << std::string(10, ch) << std::endl;
        }
    }
}
