#include "catch.hpp"
#include "gubg/xml/Builder.hpp"
#include <fstream>
using namespace gubg::xml::builder;

TEST_CASE("SVG tests", "[ut][svg]")
{
    std::ofstream fo("test.svg");

    {
        Header header(fo);
        header.attr("version", "1.0");
        header.attr("encoding", "UTF-8");
    }
    {
        Comment(fo) << "Created by gubg" << " (http://www.github.com/gfannes/gubg)";
    }
    Tag svg(fo, "svg");
    svg.attr("width", 800);
    svg.attr("height", 600);
    auto g = svg.tag("g");

    double d = 10;
    std::ostringstream oss;
    auto M = [&](int x, int y){oss << "M" << d*x << "," << d*y << " ";};
    auto m = [&](int x, int y){oss << "m" << d*x << "," << d*y << " ";};
    auto arc = [&](int x, int y, bool b){oss << "a" << d*2 << "," << d*2 << " 0 0," << (b?1:0) << d*x << "," << d*y << " ";};
    auto a = [&](int x, int y){arc(x, y, true);};
    auto aa = [&](int x, int y){arc(x, y, false);};
    auto l = [&](int x, int y){oss << "l" << d*x << "," << d*y << " ";};
    
    //a
    {
        oss.str("");
        M(0,16);
        m(0,-6);
        a(2,-2);
        l(4,0);
        a(2,2);
        l(0,6);
        m(0,-4);
        l(-6,0);
        aa(-2,2);
        aa(2,2);
        l(4,0);
        aa(2,-2);
        g.tag("path").attr("d", oss.str()).attr("fill", "none").attr("stroke", "blue");
    }
    //b
    {
        oss.str("");
        M(8,16);
        l(0,-16);
        m(0,8);
        l(6,0);
        a(2,2);
        l(0,4);
        a(-2,2);
        l(-4,0);
        a(-2,-2);
        g.tag("path").attr("d", oss.str()).attr("fill", "none").attr("stroke", "blue");
    }
    //c
    {
        oss.str("");
        M(16,16);
        m(8,-2);
        a(-2,2);
        l(-4,0);
        a(-2,-2);
        l(0,-4);
        a(2,-2);
        l(4,0);
        a(2,2);
        g.tag("path").attr("d", oss.str()).attr("fill", "none").attr("stroke", "blue");
    }
}
