#include "catch.hpp"
#include "gubg/gnuplot/Stream.hpp"
#include <cmath>

TEST_CASE("gnuplot tests", "[ut][gnuplot]")
{
    gubg::gnuplot::Stream gs;

    gs.name(0, "sinus data");
    gs.name(0, 0, "x").name(0, 1, "sinus(x)");
    gs.name(1, "cosinus data");
    gs.name(1, 0, "x").name(1, 1, "cosinus(x)");
    for (double t = -3.0; t <= 3.0; t += 0.01)
    {
        gs.data() << t << std::sin(t);
        gs.data(1) << t << std::cos(t);
    }

    gs.pause();
    gs.name(42, "the answer");
    gs.name(42, 0, "x").name(42, 1, "round(sin(x))").name(42, 2, "sin(round(x))");
    for (double t = -10.0; t <= 10.0; t += 0.1)
        gs.data(42) << t << std::round(std::sin(t)) << std::sin(std::round(t));

    gs.save("test.gnuplot");
}
