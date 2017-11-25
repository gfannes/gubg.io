#include "catch.hpp"
#include "gubg/svg/Turtle.hpp"
#include <fstream>

TEST_CASE("svg::Turtle", "[ut][turtle]")
{
    gubg::svg::Turtle t;

    t.down();

    const double min_th = 5;

    const double floor_th = 4;

    const double tooth_w = 20;
    const double tooth_ww = tooth_w/2;
    const double tooth_th = 5;

    const double front_th = 4;
    const double back_th = 4;

    const double side_th = 4;

    const unsigned int hnrw = 2;
    const unsigned int wnrw = 15;

    const double side_height = min_th+floor_th+hnrw*tooth_w-tooth_ww+min_th+2*tooth_th;

    const double h = 8;
    const double s = 8;
    const double d = 8;
    const unsigned int nr = 9;
    const double l = wnrw*tooth_w+tooth_ww;

    auto side = [&](bool b)
    {
        const double x = 0;
        const double y = 0 + b*450;

        const double offset = (b ? -tooth_ww : tooth_ww/2);

        auto ripple_a = [&](double delta = 0.0){
            t.color("red").run(tooth_w+delta);
        };
        auto ripple_b = [&](){
            t.color("blue").run(tooth_ww).turn(-0.25).run(tooth_th).turn(-0.25).run(tooth_ww).turn(0.25).run(tooth_th).turn(0.25);
        };
        auto ripple_bb = [&](){
            t.color("blue").turn(-0.25).run(tooth_th).turn(-0.25).run(tooth_ww).turn(0.25).run(tooth_th).turn(0.25).run(tooth_ww);
        };
        auto ripple_c = [&](){
            t.color("green").run(tooth_w).turn(0.25).run(2*tooth_th).turn(-0.25);
        };

        t.up().to(x,y, 0).down();
        for (int i = 0; i < nr; ++i)
        {
            ripple_a((i == 0 ? offset : 0.0));
            ripple_b();
            ripple_c();
        }
        t.color("black").run(tooth_w-offset).turn(0.25).run(side_height);

        t.up().to(x,y, 0).down();
        t.color("black").turn(0.25).run(side_height).turn(-0.25).run(tooth_ww+offset);
        for (int i = 0; i < nr; ++i)
        {
            ripple_a();
            ripple_bb();
            ripple_c();
        }
        t.color("black").run(tooth_ww-offset);

        t.up().to(x,y, 0).turn(0.25).run(min_th).turn(-0.25);
        for (int i = 0; i < nr; ++i)
        {
            t.run(tooth_w).down().run(tooth_w).turn(0.25).run(floor_th).turn(0.25).run(tooth_w).turn(0.25).run(floor_th).turn(0.25).up().run(tooth_w);
        }

        t.up().to(x,y, 0).run(min_th+back_th).turn(0.25).run(min_th+floor_th);
        for (int i = 0; i < hnrw; ++i)
        {
            t.down().run(tooth_ww).turn(0.25).run(back_th).turn(0.25).run(tooth_ww).turn(0.25).run(back_th).turn(0.25).up().run(tooth_w);
        }

        t.up().to(x,y, 0).run(nr*tooth_w*2+tooth_w-min_th).turn(0.25).run(min_th+floor_th);
        for (int i = 0; i < hnrw; ++i)
        {
            t.down().run(tooth_ww).turn(0.25).run(front_th).turn(0.25).run(tooth_ww).turn(0.25).run(front_th).turn(0.25).up().run(tooth_w);
        }
    };

    side(true);
    side(false);

    auto front = [&](){
        const auto y = 550.0;

        t.up().to(0,y, 0);

        t.turn(0.25);
        t.down();
        for (int i = 0; i < hnrw; ++i)
            t.turn(0.25).run(2*side_th).turn(-0.25).run(tooth_ww).turn(-0.25).run(2*side_th).turn(0.25).run(tooth_ww);
        t.run(side_height-hnrw*tooth_w-floor_th);
        t.turn(-0.25).run(l);

        t.up().to(0,y, 0);
        t.down();
        for (int i = 0; i < wnrw; ++i)
            t.run(tooth_ww).turn(-0.25).run(floor_th).turn(0.25).run(tooth_ww).turn(0.25).run(floor_th).turn(-0.25);
        t.run(tooth_ww).turn(0.25);
        for (int i = 0; i < hnrw; ++i)
            t.turn(-0.25).run(2*side_th).turn(0.25).run(tooth_ww).turn(0.25).run(2*side_th).turn(-0.25).run(tooth_ww);
        t.run(side_height-hnrw*tooth_w-floor_th);
    };

    front();

    auto floor = [&](){
        const auto x = 0.0;
        const auto y = 100.0;

        t.up().to(x,y, 0);
        t.down();
        for (int i = 0; i < nr; ++i)
            t.run(tooth_w).turn(-0.25).run(8).turn(0.25).run(tooth_w).turn(0.25).run(8).turn(-0.25);
        t.run(tooth_w);
        t.turn(0.25).run(l);

        t.up().to(x,y, 0);
        t.down();
        t.turn(0.5).run(min_th).turn(-0.25).run(l).turn(-0.25).run(min_th);
        for (int i = 0; i < nr; ++i)
            t.run(tooth_w).turn(0.25).run(8).turn(-0.25).run(tooth_w).turn(-0.25).run(8).turn(0.25);
        t.run(tooth_w);

        t.up().to(x,y, 0).run(min_th+back_th).turn(0.25).run(min_th+floor_th);
        for (int i = 0; i < wnrw; ++i)
            if (i%2 == 0)
                t.down().run(tooth_ww).turn(0.25).run(back_th).turn(0.25).run(tooth_ww).turn(0.25).run(back_th).turn(0.25).up().run(tooth_w);
            else
                t.down().run(tooth_ww).turn(0.25).run(back_th+min_th).turn(0.25).run(tooth_ww).turn(0.25).run(back_th+min_th).turn(0.25).up().run(tooth_w);

        t.up().to(x,y, 0).run(nr*tooth_w*2+tooth_w-min_th).turn(0.25).run(min_th+floor_th);
        for (int i = 0; i < wnrw; ++i)
            t.down().run(tooth_ww).turn(0.25).run(front_th).turn(0.25).run(tooth_ww).turn(0.25).run(front_th).turn(0.25).up().run(tooth_w);

        t.up().to(x,y, 0).turn(-0.25).run(tooth_w).turn(0.25).run(tooth_w-tooth_ww/2);
        for (int i = 0; i < 2*nr; ++i)
            t.down().run(tooth_ww).turn(0.25).run(front_th).turn(0.25).run(tooth_ww).turn(0.25).run(front_th).turn(0.25).up().run(tooth_w);
    };

    floor();

    auto divider = [&](){
        const auto y = 650.0;

        t.up().to(0,y, 0);

        t.turn(0.25);
        t.down().run(hnrw*tooth_w);
        t.turn(-0.25).run(l);

        t.up().to(0,y, 0);
        t.down();
        for (int i = 0; i < wnrw; ++i)
            t.run(tooth_ww).turn(-0.25).run(floor_th).turn(0.25).run(tooth_ww).turn(0.25).run(floor_th).turn(-0.25);
        t.run(tooth_ww).turn(0.25);
        t.run(hnrw*tooth_w);
    };

    divider();

    auto divider2 = [&](){
        const auto y = 750.0;

        t.up().to(0,y, 0);

        const auto ll = (2*nr+1)*tooth_w - 2*min_th - back_th - front_th;

        t.turn(0.25);
        t.down().run(hnrw*tooth_w);
        t.turn(-0.25).run(ll);

        t.up().to(0,y, 0);
        t.down().run(tooth_w-tooth_ww/2-min_th-back_th);
        for (int i = 0; i < 2*nr; ++i)
        {
            if (i > 0)
                t.run(tooth_ww);
            t.turn(-0.25).run(floor_th).turn(0.25).run(tooth_ww).turn(0.25).run(floor_th).turn(-0.25);
        }
        t.run(tooth_w-tooth_ww/2-min_th-back_th).turn(0.25);
        t.run(hnrw*tooth_w);
    };

    divider2();

    std::ofstream fo("turtle.svg");
    REQUIRE(t.stream(fo, 800,600));
}
