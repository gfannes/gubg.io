#include "catch.hpp"
#include "gubg/svg/Turtle.hpp"
#include <fstream>

TEST_CASE("svg::Turtle", "[ut][turtle]")
{
    gubg::svg::Turtle t;

    t.down();

    /* t.run(10).turn(0.25).run(10); */

    t.run(10).turn(0.25).run(10).turn(0.25).run(10).turn(0.25).run(10).turn(0.25);
    t.up();

    if (true)
    {
        t.to(100,100, 0);

        t.down();
        /* t.zoom(4); */

        const double h = 10;
        const double w = 20;
        const double s = 4;
        auto ripple_a = [&](){
            t.color("red").run(w);
        };
        auto ripple_aa = [&](){
            t.color("brown").run(w-s);
        };
        auto ripple_b = [&](){
            t.color("blue").run(s).turn(-0.25).run(h).turn(-0.25).run(s).turn(0.25).run(h).turn(0.25);
        };
        auto ripple_c = [&](){
            t.color("green").run(w).turn(0.25).run(2*h).turn(-0.25);
        };
        auto ripple_cc = [&](){
            t.color("brown").run(w+s).turn(0.25).run(2*h).turn(-0.25);
        };

        int nr = 20;

        for (int i = 0; i < nr; ++i)
        {
            ripple_a();
            ripple_b();
            ripple_c();
        }
        t.color("black").turn(0.25).run(200).turn(0.25);
        for (int i = 0; i < nr; ++i)
        {
            if (i == 0)
            {
                ripple_aa();
                ripple_b();
                ripple_c();
            }
            else if (i == nr-1)
            {
                ripple_a();
                ripple_b();
                ripple_cc();
            }
            else
            {
                ripple_a();
                ripple_b();
                ripple_c();
            }
        }
        t.color("black").turn(0.25).run(200).turn(0.25);
    }

    std::ofstream fo("turtle.svg");
    REQUIRE(t.stream(fo, 800,600));
}
