#include "catch.hpp"
#include "gubg/svg/Turtle.hpp"
#include <fstream>

TEST_CASE("svg::Turtle", "[ut][turtle]")
{
    gubg::svg::Turtle t;

    t.down();

    const double h = 8;
    const double w = 20;
    const double ww = w/2;
    const double s = 8;
    const double d = 8;
    const unsigned int nr = 9;
    const unsigned int hnrw = 2;
    const double l = 300;

    auto side = [&](bool b)
    {
        const double x = 0;
        const double y = 0 + b*300;

        const double offset = (b ? -s : s/2);

        auto ripple_a = [&](double delta = 0.0){
            t.color("red").run(w+delta);
        };
        auto ripple_b = [&](){
            t.color("blue").run(s).turn(-0.25).run(h).turn(-0.25).run(s).turn(0.25).run(h).turn(0.25);
        };
        auto ripple_bb = [&](){
            t.color("blue").turn(-0.25).run(h).turn(-0.25).run(s).turn(0.25).run(h).turn(0.25).run(s);
        };
        auto ripple_c = [&](){
            t.color("green").run(w).turn(0.25).run(2*h).turn(-0.25);
        };

        t.up().to(x,y, 0).down();
        for (int i = 0; i < nr; ++i)
        {
            ripple_a((i == 0 ? offset : 0.0));
            ripple_b();
            ripple_c();
        }
        t.color("black").run(w-offset).turn(0.25).run(hnrw*w+w+ww);

        t.up().to(x,y, 0).down();
        t.color("black").turn(0.25).run(hnrw*w+w+ww).turn(-0.25).run(s+offset);
        for (int i = 0; i < nr; ++i)
        {
            ripple_a();
            ripple_bb();
            ripple_c();
        }
        t.color("black").run(w-s-offset);

        t.up().to(x,y, 0).turn(0.25).run(h).turn(-0.25);
        for (int i = 0; i < nr; ++i)
        {
            t.run(w).down().run(w).turn(0.25).run(d).turn(0.25).run(w).turn(0.25).run(d).turn(0.25).up().run(w);
        }

        t.up().to(x,y, 0).run(h+d).turn(0.25).run(ww);
        for (int i = 0; i < hnrw; ++i)
        {
            t.run(ww).down().run(ww).turn(0.25).run(d).turn(0.25).run(ww).turn(0.25).run(d).turn(0.25).up().run(ww);
        }

        t.up().to(x,y, 0).run(nr*w*2+w-h).turn(0.25).run(ww);
        for (int i = 0; i < hnrw; ++i)
        {
            t.run(ww).down().run(ww).turn(0.25).run(d).turn(0.25).run(ww).turn(0.25).run(d).turn(0.25).up().run(ww);
        }
    };

    side(true);
    /* side(false); */

    auto front = [&](){

        t.up().to(0,0, 0);

        t.turn(0.25);
        t.down().run(w-2*d);
        for (int i = 0; i < hnrw; ++i)
            t.turn(0.25).run(ww).turn(-0.25).run(ww).turn(-0.25).run(ww).turn(0.25).run(ww);
        t.run(ww+8);
        t.turn(-0.25).run(l);

        t.up().to(0,0, 0);
        t.down().run(l);
        t.turn(0.25);
        t.run(w-2*d);
        for (int i = 0; i < hnrw; ++i)
            t.turn(-0.25).run(ww).turn(0.25).run(ww).turn(0.25).run(ww).turn(-0.25).run(ww);
        t.run(ww+8);
    };

    front();

    auto floor = [&](){
        t.up().to(0,100, 0);
        t.down();
        for (int i = 0; i < nr; ++i)
            t.run(w).turn(-0.25).run(8).turn(0.25).run(w).turn(0.25).run(8).turn(-0.25);
        t.run(w);
        t.turn(0.25).run(l);

        t.up().to(0,100, 0);
        t.down();
        t.turn(0.25).run(l).turn(-0.25);
        for (int i = 0; i < nr; ++i)
            t.run(w).turn(0.25).run(8).turn(-0.25).run(w).turn(-0.25).run(8).turn(0.25);
        t.run(w);
    };

    floor();

    std::ofstream fo("turtle.svg");
    REQUIRE(t.stream(fo, 800,600));
}
