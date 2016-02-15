#include "catch.hpp"
#include "gubg/xml/Builder.hpp"
#include "gubg/mss.hpp"
#include <iostream>
using namespace gubg::xml::builder;
using namespace std;

TEST_CASE("xml builder tests", "[ut][xml]")
{
    {
        Header header(cout);
        header.attr("version", "1.0");
        header.attr("encoding", "UTF-8");
    }
    {
        Comment(cout) << "Created by gubg" << " (http://www.github.com/gfannes/gubg)";
    }
    Tag svg(cout, "svg");
    svg.attr("width", 800);
    svg.attr("height", 600);
    auto g = svg.tag("g");
    g.tag("circle").attr("cx", 100).attr("cy", 100).attr("r", 50);
}
