#include "catch.hpp"
#include "gubg/terminal/Color.hpp"
#include <iostream>

TEST_CASE("gubg::terminal::Color tests", "[ut][terminal][Color]")
{
    for (auto i = 16u; i < 256; ++i)
    {
        std::cout << gubg::terminal::Color(i) << '*';
        if (i%12 == 3)
            std::cout << std::endl;
    }
}
