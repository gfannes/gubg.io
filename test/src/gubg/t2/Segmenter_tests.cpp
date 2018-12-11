#include "catch.hpp"
#include "gubg/t2/Segmenter.hpp"
#include <array>
using namespace gubg;

TEST_CASE("t2::Segmenter tests", "[ut][t2][Segmenter]")
{
    struct Scn
    {
        std::array<t2::Byte, 10> buffer;
        Scn()
        {
            buffer.fill(t2::md_som);
        }
    };
    struct Exp
    {
        unsigned int nr_segments = 0;
    };

    Scn scn;
    Exp exp;

    SECTION("all som") { }
    SECTION("empty segment")
    {
        scn.buffer[1] = t2::md_eom;
        exp.nr_segments = 1;
    }
    t2::Segmenter segmenter{scn.buffer.data(), scn.buffer.data()+scn.buffer.size()};

    unsigned int nr_segments = 0;
    auto lambda = [&](t2::Byte *begin, t2::Byte *end){
        ++nr_segments;
        return true;
    };

    for (auto b: scn.buffer)
    {
        segmenter.process(b, lambda);
    }
    REQUIRE(exp.nr_segments == nr_segments);
}
