#include "catch.hpp"
#include "gubg/svg/Turtle.hpp"
#include <fstream>

TEST_CASE("svg::Turtle", "[ut][turtle]")
{
    gubg::svg::Turtle t;

    t.down();
    t.run(10);

    std::ofstream fo("turtle.svg");
    t.stream(fo, 800,600);
}
